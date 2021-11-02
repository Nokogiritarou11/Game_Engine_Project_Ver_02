#include "Thread_Pool.h"

using namespace BeastEngine;
using namespace std;

Thread_Pool::Thread_Pool()
{
	const unsigned int num_threads = thread::hardware_concurrency();
	for (unsigned int i = 0; i < num_threads; ++i)
	{
		threads.emplace_back(thread([this] {this->Infinite_Loop_Function(); }));
	}
}

Thread_Pool::~Thread_Pool()
{
	if (!stopped)
	{
		Shut_Down();
	}
}

void Thread_Pool::Shut_Down()
{
	{
		unique_lock lock(queue_mutex);
		terminate_pool = true;
	}

	condition.notify_all();

	// 全スレッドをJoin
	for (thread& th : threads)
	{
		th.join();
	}

	threads.clear();
	stopped = true;
}

void Thread_Pool::Infinite_Loop_Function()
{
	while (true)
	{
		function<void()> job;
		{
			unique_lock lock(queue_mutex);

			condition.wait(lock, [this]() {
				return !job_queue.empty() || terminate_pool; });
			if (terminate_pool && job_queue.empty()) return;
			job = job_queue.front();
			job_queue.pop();
		}

		job();
	}
}

void Thread_Pool::Add_Job(const function<void()>& new_job)
{
	{
		unique_lock lock(queue_mutex);
		job_queue.push(new_job);
	}

	condition.notify_one();
}