#pragma once
#include <typeinfo>
#include <string>
#include <list>
#include <memory>
#include "Component.h"

namespace BeastEngine
{
	class GameObject : public BeastEngine::Object
	{
	public:
		bool Compare_Tag(std::string _tag);								//�w�肵���^�O�Ɉ�v���邩�Ԃ�
		bool Get_Active();												//�Q�[���I�u�W�F�N�g���A�N�e�B�u��(�e�̏�Ԃ��l�����Ȃ�)
		void Set_Active(bool value);									//�A�N�e�B�u��Ԃ��w�肷��
		bool Get_Active_In_Hierarchy();									//�q�G�����L�[��ŃA�N�e�B�u��(�e�̏�Ԃ��l������)

		int layer = 0;
		std::string tag = "Default";
		std::shared_ptr<BeastEngine::Transform> transform;
		std::vector<std::shared_ptr<BeastEngine::Component>> component_list;

		template<class T>
		std::shared_ptr<T> Get_Component();								//�A�^�b�`�T��Ă���R���|�[�l���g���������Ԃ�(���݂��Ȃ��ꍇnull_ptr)
		template<class T>
		std::shared_ptr<T> Add_Component();								//�R���|�[�l���g���A�^�b�`����
		std::shared_ptr<Component> Add_Component(std::string class_name);

		static std::weak_ptr<GameObject> Find(std::string Name);			//�V�[�����̃Q�[���I�u�W�F�N�g�𖼑O�Ō�������
		static std::weak_ptr<GameObject> Find_With_Tag(std::string Tag);	//�V�[�����̃Q�[���I�u�W�F�N�g���^�O�Ō�������

	private:

		void Initialize();
		void Release();
		void Set_Child_Active(bool value);
		bool active = true;
		bool active_old = true;

		friend class Resources;
		friend class Scene;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version);
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::GameObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::GameObject)
CEREAL_CLASS_VERSION(BeastEngine::GameObject, 1)

template<class T>
std::shared_ptr<T> BeastEngine::GameObject::Get_Component()
{
	for (std::shared_ptr<BeastEngine::Component> com : component_list)
	{
		std::shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			return buff;
		}
	}
	return nullptr;
}

template<class T>
std::shared_ptr<T> BeastEngine::GameObject::Add_Component()
{
	std::shared_ptr<T> buff = std::make_shared<T>();
	bool can_multiple = std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Can_Multiple();

	if (!can_multiple)
	{
		bool already_attach = false;
		for (std::shared_ptr<BeastEngine::Component> com : component_list)
		{
			std::shared_ptr<T> _buff = std::dynamic_pointer_cast<T>(com);
			if (_buff != nullptr)
			{
				already_attach = true;
				break;
			}
		}
		if (!already_attach)
		{
			std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Initialize(std::static_pointer_cast<BeastEngine::GameObject>(shared_from_this()));
			component_list.emplace_back(buff);
			return buff;
		}
	}
	else
	{
		std::dynamic_pointer_cast<BeastEngine::Component>(buff)->Initialize(std::static_pointer_cast<BeastEngine::GameObject>(shared_from_this()));
		component_list.emplace_back(buff);
		return buff;
	}
	return nullptr;
}

#include "Transform.h"
template<class Archive>
void BeastEngine::GameObject::serialize(Archive& archive, std::size_t version)
{
	archive(cereal::base_class<BeastEngine::Object>(this), layer, tag, transform, component_list, active, active_old);
}