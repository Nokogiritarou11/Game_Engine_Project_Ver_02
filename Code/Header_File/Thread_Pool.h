#pragma once
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>

namespace BeastEngine
{
	class Thread_Pool
	{
	public:
		Thread_Pool();
		~Thread_Pool();

		[[nodiscard]] bool Get_Is_Empty_Pool() const { return job_queue.empty(); }

		void Shut_Down();
		void Add_Job(const std::function<void()>& new_job);

	private:
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> job_queue;
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool terminate_pool = false;
		bool stopped = false;

		void Infinite_Loop_Function();
	};
}
