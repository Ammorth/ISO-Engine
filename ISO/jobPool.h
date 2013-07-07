#pragma once
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <assert.h>

namespace ISO
{

// A class which allows you to create jobs and schedule them on parallel threads.
// supports job dependancies
class jobPool
{
public:

	class job
	{
	public:

		enum state
		{
			initialized,
			waiting,
			queued,
			working,
			complete
		};

		job(void)
		{
			curState = initialized;
		}

		virtual ~job(void)
		{
		}

		bool addDependancy(job* required)
		{
			stateMutex.lock();
			bool r = false;
			if(curState == initialized)
			{
				r = dependsOn.insert(required).second;
			}
			stateMutex.unlock();
			return r;
		}

		bool removeDependancy(job* notrequired)
		{
			stateMutex.lock();
			bool r = false;
			if(curState == initialized || curState == complete)
			{
				r = (dependsOn.erase(notrequired) == 1);
			}
			stateMutex.unlock();
			return r;
		}

		state getJobState()
		{
			stateMutex.lock();
			state s = curState;
			stateMutex.unlock();
			return s;
		}

		// derived classes need to overload this operator
		virtual void operator()()
		{
		}
	private:

		void setState(state s)
		{
			stateMutex.lock();
			curState = s;
			stateMutex.unlock();
		}

		bool canRun()
		{
			for(std::set<job*>::iterator I = dependsOn.begin(); I != dependsOn.end(); ++I)
			{
				job* j = *I;
				if(j->getJobState() != job::state::complete)
				{
					return false;
				}
			}
			return true;
		}

		friend class jobPool;
		state curState;
		std::set<job*> dependsOn;
		std::mutex stateMutex;
	};

	// default constructor
	jobPool()
	{
		// hold the master lock while we setup
		jobDone = false;
		workingThreads = 0;
		killMaster = false;
		masterMutex.lock();
		masterThread = std::thread(&ISO::jobPool::master_entry, this);

		unsigned int numThreads = std::thread::hardware_concurrency();
		if(numThreads == 0)
		{
			numThreads = 2;
		}
		worker.resize(numThreads);
		//workerMutex = std::vector<std::mutex>(numThreads);
		for(unsigned int i = 0; i < numThreads; ++i)
		{
			worker[i].kill = false;
			worker[i].thread = std::thread(&ISO::jobPool::worker_entry, this, i);
		}
		masterCond.notify_all();
		masterMutex.unlock();
	}

	// default destructor
	~jobPool(void)
	{
		// join all worker threads
		for(unsigned int i = 0; i < worker.size(); ++i)
		{
			worker[i].mutex.lock();
			worker[i].kill = true;
			worker[i].cond.notify_all();
			worker[i].mutex.unlock();
			worker[i].thread.join();
		}
		// and master thread
		masterMutex.lock();
		killMaster = true;
		masterCond.notify_all();
		masterMutex.unlock();
		masterThread.join();
	}

	// call this from main thread to add a job once it has been set up
	// Note: Make sure that all jobs which this job depend on are already added to the pool
	// Note: jobs will start running immediately, if possible.
	void addJobToPool(job* whichJob)
	{
		job::state current = whichJob->getJobState();
		if(current == job::state::initialized || current == job::state::complete)
		{
			masterMutex.lock();
			if(whichJob->canRun())
			{
				whichJob->setState(job::state::queued);
				// add to job queue
				jobQueue.push(whichJob);
			}else
			{
				whichJob->setState(job::state::waiting);
				// add to wait list
				waitList.insert(whichJob);
			}
			// wake up master thread
			masterCond.notify_all();
			masterMutex.unlock();
		}
	}

	// call this on the main thread once all jobs have been setup to wait for them all to finish
	void waitForJobs()
	{
		std::unique_lock<std::mutex> lock(masterMutex);
		while(!jobQueue.empty() || !waitList.empty() || workingThreads != 0)
		{
			externalCond.wait(lock);
		}
		// all done
		lock.unlock();
	}

private:
	std::thread masterThread;
	std::mutex masterMutex;
	std::condition_variable masterCond;
	bool killMaster;

	std::condition_variable externalCond;

	bool jobDone;
	std::queue<job*> jobQueue;
	std::set<job*> waitList;
	
	class worker
	{
	public:
		std::mutex mutex;
		std::condition_variable cond;
		std::thread thread;
		job* job;
		bool kill;

		worker::worker()
		{
			kill = false;
			job = NULL;
		}

		worker(worker const&){};
		worker& operator=(worker const& other)
		{
			job = other.job;
			kill = other.kill;
			return *this;
		};
	};

	std::vector<worker> worker;
	unsigned int workingThreads;

	void worker_entry(unsigned int id)
	{
		std::unique_lock<std::mutex> lock(worker[id].mutex);
		while(!worker[id].kill)
		{
			if(worker[id].job)
			{
				job* thisJob = worker[id].job;
				lock.unlock();
				// doing work
				
				(*thisJob)();
				// done work
				
				// we need to clear our job before we notify master so that it can give us a new job right away
				lock.lock();
				worker[id].job = NULL;
				lock.unlock();

				// notify master job is done
				masterMutex.lock();
				thisJob->setState(job::state::complete);	
				jobDone = true;
				workingThreads--;
				masterCond.notify_all();
				masterMutex.unlock();

				// and get back our mutex for the next loop
				lock.lock();
			}else
			{
				worker[id].cond.wait(lock);
			}
		}
		lock.unlock();
	}

	void master_entry()
	{
		std::unique_lock<std::mutex> lock(masterMutex);
		while(!killMaster)
		{
			// can any new jobs run?
			if(jobDone)
			{
				for(std::set<job*>::iterator I = waitList.begin(); I != waitList.end(); /* nothing here */)
				{
					job* j = *I;
					if(j->canRun())
					{
						// move it to the queue
						jobQueue.push(j);
						// and erase and increment
						waitList.erase(I++);
					}else
					{
						// otherwise check the next one
						++I;
					}
				}
				jobDone = false;
			}
			// debug check to make sure we don't have jobs which can't be satisfied
			assert( !(!waitList.empty() && jobQueue.empty() && workingThreads == 0) );
			// see if there are any threads we can assign work

			for(unsigned int i = 0; i < worker.size(); ++i)
			{
				// check again to make sure another thread didn't get the last job
				if(!jobQueue.empty())
				{
					// at this point, we have two mutexes, but it should be ok since this is the only time any thread will have 2.
					worker[i].mutex.lock();
					if(worker[i].job == NULL)
					{
						worker[i].job = jobQueue.front();
						worker[i].job->setState(job::state::working);
						jobQueue.pop();
						workingThreads++;
						worker[i].cond.notify_all();
					}
					worker[i].mutex.unlock();
				}else
				{
					break;
				}
			}	

			// see if all jobs are done
			if(jobQueue.empty() && waitList.empty() && workingThreads == 0)
			{
				// signal external threads
				externalCond.notify_all();
			}
			// now wait for more stuff to do
			masterCond.wait(lock);
		}
		lock.unlock();
	}

	// to prevent copy constructors
	jobPool(jobPool const&);
	jobPool& operator=(jobPool const&);


};

} // end ISO namespace