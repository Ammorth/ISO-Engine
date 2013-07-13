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

		job(void);

		virtual ~job(void);

		bool addDependancy(job* required);

		bool removeDependancy(job* notrequired);

		state getJobState();

		// derived classes need to overload this operator
		virtual void operator()()
		{
		}
	private:

		void setState(state s);

		bool canRun();

		friend class jobPool;
		state curState;
		std::set<job*> dependsOn;
		std::mutex stateMutex;
	};

	// default constructor
	jobPool();

	// default destructor
	~jobPool(void);

	// call this from main thread to add a job once it has been set up
	// Note: Make sure that all jobs which this job depend on are already added to the pool
	// Note: jobs will start running immediately, if possible.
	void addJobToPool(job* whichJob);

	// call this on the main thread once all jobs have been setup to wait for them all to finish
	void waitForJobs();

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
		}
	};

	std::vector<worker> worker;
	unsigned int workingThreads;

	void worker_entry(unsigned int id);

	void master_entry();

	// to prevent copy constructors
	jobPool(jobPool const&);
	jobPool& operator=(jobPool const&);


};

} // end ISO namespace