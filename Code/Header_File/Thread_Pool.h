#pragma once
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>

namespace BeastEngine
{
	//スレッドプール エンジン内でマルチスレッドを行うためのプール
	class Thread_Pool
	{
	public:
		Thread_Pool();
		~Thread_Pool();

		[[nodiscard]] bool Get_Is_Empty_Pool() const { return job_queue.empty(); } //ジョブキューが空か

		void Shut_Down(); //終了時処理
		void Add_Job(const std::function<void()>& new_job); //関数ポインタからマルチスレッドに登録する

	private:
		std::vector<std::thread> threads;
		std::queue<std::function<void()>> job_queue;
		std::mutex queue_mutex;
		std::condition_variable condition;
		bool terminate_pool = false; //スレッドを終了するか
		bool stopped = false; //スレッドが止まっているか

		void Infinite_Loop_Function(); //各スレッド内でループし続けるメイン関数
	};
}
