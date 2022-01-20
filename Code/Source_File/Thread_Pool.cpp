#include "Thread_Pool.h"

using namespace BeastEngine;
using namespace std;

Thread_Pool::Thread_Pool()
{
	//このPCで使用できるスレッドの最大数の分だけ追加する
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

void Thread_Pool::Wait_Job_Complete()
{
	//全スレッドの終了待ち(メインスレッドが止まる)
	if (doing_job_count > 0)
	{
		std::unique_lock lock(main_mutex);
		main_condition.wait(lock);
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
	//サブスレッドでは常に回っている
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

		//処理が終了したので稼働スレッド数を減らす
		if (--doing_job_count == 0)
		{
			main_condition.notify_one();
		}
	}
}

void Thread_Pool::Add_Job(const function<void()>& new_job)
{
	++doing_job_count;
	//サブスレッドに仕事を投げる
	{
		unique_lock lock(queue_mutex);
		job_queue.push(new_job);
	}

	condition.notify_one();
}