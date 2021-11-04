#pragma once
#include <vector>
#include <memory>

namespace BeastEngine
{
	class Light;

	//���C�g�Ǘ��N���X
	class Light_Manager
	{
	public:
		void Reset(); //���X�g�̃��Z�b�g
		void Add(const std::shared_ptr<Light>& light); //���X�g�ɒǉ�����
		std::vector<std::weak_ptr<Light>> light_list{};
	private:

	};
}