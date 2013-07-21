#pragma once
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>


namespace ISO
{

// A class which allows you to create jobs and schedule them on parallel threads.
// supports job dependancies
class JobPool
{
public:

	class Job
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

		Job(void);

		virtual ~Job(void);

		bool addDependancy(Job* required);

		bool removeDependancy(Job* notrequired);

		state getJobState();

		// derived classes need to overload this operator
		virtual void operator()()
		{
		}
	private:

		void setState(state s);

		bool canRun();

		friend class JobPool;
		state curState;
		std::set<Job*> dependsOn;
		std::mutex stateMutex;
	};

	// default constructor
	JobPool();

	JobPool(unsigned int threads);

	// default destructor
	~JobPool(void);

	// call this from main thread to add a job once it has been set up
	// Note: Make sure that all jobs which this job depend on are already added to the pool
	// Note: jobs will start running immediately, if possible.
	void addJobToPool(Job* whichJob);

	// call this on the main thread once all jobs have been setup to wait for them all to finish
	void waitForJobs();

private:
	std::thread masterThread;
	std::mutex masterMutex;
	std::condition_variable masterCond;
	bool killMaster;

	std::condition_variable externalCond;

	bool jobDone;
	std::queue<Job*> jobQueue;
	std::set<Job*> waitList;
	
	class worker
	{
	public:
		std::mutex mutex;
		std::condition_variable cond;
		std::thread thread;
		Job* job;
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
		}
	};

	std::vector<worker> worker;
	unsigned int workingThreads;

	void worker_entry(unsigned int id);

	void master_entry();

	// to prevent copy constructors
	JobPool(JobPool const&);
	JobPool& operator=(JobPool const&);


};

} // end ISO namespace