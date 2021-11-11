#pragma once
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <atomic>

namespace BeastEngine
{
	//�X���b�h�v�[�� �G���W�����Ń}���`�X���b�h���s�����߂̃v�[��
	class Thread_Pool
	{
	public:
		Thread_Pool();
		~Thread_Pool();

		[[nodiscard]] bool Get_Is_Empty_Pool() const { return job_queue.empty(); } //�W���u�L���[����
		void Wait_Job_Complete(); //�������Ă���X���b�h�����ׂďI���܂ő҂�

		void Shut_Down(); //�I��������
		void Add_Job(const std::function<void()>& new_job); //�֐��|�C���^����}���`�X���b�h�ɓo�^����

	private:
		std::vector<std::thread> threads;

		std::queue<std::function<void()>> job_queue;
		std::mutex queue_mutex;
		std::condition_variable condition;

		std::atomic<int> doing_job_count;
		std::mutex main_mutex;
		std::condition_variable main_condition;

		bool terminate_pool = false; //�X���b�h���I�����邩
		bool stopped = false; //�X���b�h���~�܂��Ă��邩

		void Infinite_Loop_Function(); //�e�X���b�h���Ń��[�v�������郁�C���֐�
	};
}
