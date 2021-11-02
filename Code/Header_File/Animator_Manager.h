#pragma once
#include <memory>
#include <vector>

namespace BeastEngine
{
	class Animator;

	//�A�j���[�^�[�Ǘ��N���X
	class Animator_Manager
	{
	public:
		void Reset(); //���X�g�̃��Z�b�g

		void Update(); //�X�V
		void Add(std::shared_ptr<Animator> animator); //���X�g�֒ǉ�

	private:
		std::vector<std::weak_ptr<Animator>> animator_list;
	};
}
