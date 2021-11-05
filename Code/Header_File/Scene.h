#pragma once
#include <string>
#include <memory>
#include "GameObject.h"
#include "MonoBehaviour.h"

namespace BeastEngine
{
	//�V�[��
	class Scene
	{
	public:
		std::weak_ptr<GameObject> Find(const std::string& name);         //�w�肵�����O�̃Q�[���I�u�W�F�N�g��Ԃ�
		std::weak_ptr<GameObject> Find_With_Tag(const std::string& tag); //�w�肵���^�O�̃Q�[���I�u�W�F�N�g��Ԃ�

		std::string name; //�V�[����

	private:
		std::shared_ptr<GameObject> Instance_GameObject(const std::string& name); //�V�[�����ɃI�u�W�F�N�g��z�u����
		void Destroy_GameObject(const std::shared_ptr<GameObject>& game_object); //�I�u�W�F�N�g���폜����
		void Destroy_Component(const std::shared_ptr<Component>& component); //�R���|�[�l���g���폜����

		void Initialize(); //������
		void Update(); //�X�V
		void Reset(); //�V�[���̃��Z�b�g

		void Processing_Start(); //MonoBehaviour��Start�����s����
		void Processing_Update(int state); //MonoBehaviour��Update�����s����

		std::vector<std::shared_ptr<GameObject>> gameobject_list; //�V�[�����̑S�Q�[���I�u�W�F�N�g

		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_start_list; //start�̎��s���X�g
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_start_next_list; //���̃t���[���ōs��start�̎��s���X�g
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_update_list; //update�̎��s���X�g
		std::vector<std::weak_ptr<MonoBehaviour>> monobehaviour_update_next_list; //���̃t���[���ōs��update�̎��s���X�g

		friend class Scene_Manager;
		friend class Object;
		friend class MonoBehaviour;
		friend class Resources;
		friend class Transform;
		friend class Editor;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(name, gameobject_list);
		}

	};
}

CEREAL_CLASS_VERSION(BeastEngine::Scene, 1)