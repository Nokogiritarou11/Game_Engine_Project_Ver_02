#pragma once
#include "Component.h"
#include <typeinfo>
#include <string>
#include <list>
#include <memory>

namespace BeastEngine
{
	class GameObject : public BeastEngine::Object
	{
	public:
		bool CompareTag(std::string _tag);								//�w�肵���^�O�Ɉ�v���邩�Ԃ�
		bool activeSelf();												//�Q�[���I�u�W�F�N�g���A�N�e�B�u��(�e�̏�Ԃ��l�����Ȃ�)
		bool activeInHierarchy();										//�q�G�����L�[��ŃA�N�e�B�u��(�e�̏�Ԃ��l������)
		void SetActive(bool value);										//�A�N�e�B�u��Ԃ��w�肷��

		int layer = 0;
		std::string tag = "Default";
		std::shared_ptr<BeastEngine::Transform> transform;
		std::vector<std::shared_ptr<BeastEngine::Component>> Component_List;

		template<class T>
		std::shared_ptr<T> GetComponent();								//�A�^�b�`�T��Ă���R���|�[�l���g���������Ԃ�(���݂��Ȃ��ꍇnull_ptr)
		template<class T>
		std::shared_ptr<T> AddComponent();								//�R���|�[�l���g���A�^�b�`����

		static std::weak_ptr<GameObject> Find(std::string Name);		//�V�[�����̃Q�[���I�u�W�F�N�g�𖼑O�Ō�������
		static std::weak_ptr<GameObject> FindWithTag(std::string Tag);	//�V�[�����̃Q�[���I�u�W�F�N�g���^�O�Ō�������

	private:

		void Initialize();
		void Release();
		void Set_Child_Active(bool value);
		bool Active = true;
		bool Old_Active = true;

		friend class Resources;
		friend class Scene;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive);
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::GameObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::GameObject)

template<class T>
std::shared_ptr<T> BeastEngine::GameObject::GetComponent()
{
	for (std::shared_ptr<BeastEngine::Component> com : Component_List)
	{
		std::shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			if (typeid(shared_ptr<T>) == typeid(buff))
			{
				return buff;
			}
		}
	}
	return nullptr;
}

template<class T>
std::shared_ptr<T> BeastEngine::GameObject::AddComponent()
{
	std::shared_ptr<T> buff = std::make_shared<T>();
	bool can_multiple = std::dynamic_pointer_cast<BeastEngine::Component>(buff)->CanMultiple();

	if (!can_multiple)
	{
		bool already_attach = false;
		for (std::shared_ptr<BeastEngine::Component> com : Component_List)
		{
			std::shared_ptr<T> _buff = std::dynamic_pointer_cast<T>(com);
			if (_buff != nullptr)
			{
				if (typeid(shared_ptr<T>) == typeid(_buff))
				{
					already_attach = true;
				}
			}
		}
		if (!already_attach)
		{
			std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Initialize(std::static_pointer_cast<BeastEngine::GameObject>(shared_from_this()));
			Component_List.emplace_back(buff);
			return buff;
		}
	}
	else
	{
		std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Initialize(std::static_pointer_cast<BeastEngine::GameObject>(shared_from_this()));
		Component_List.emplace_back(buff);
		return buff;
	}
	return nullptr;
}

#include "Transform.h"
template<class Archive>
void BeastEngine::GameObject::serialize(Archive& archive)
{
	archive(cereal::base_class<BeastEngine::Object>(this), layer, tag, transform, Component_List, Active, Old_Active);
}
