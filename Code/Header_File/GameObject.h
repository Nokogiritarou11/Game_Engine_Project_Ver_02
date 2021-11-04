#pragma once
#include <typeinfo>
#include <string>
#include <list>
#include <memory>
#include "Component.h"

namespace BeastEngine
{
	//�Q�[���I�u�W�F�N�g
	class GameObject final : public Object
	{
	public:
		[[nodiscard]] bool Compare_Tag(const std::string& tag) const;	//�w�肵���^�O�Ɉ�v���邩�Ԃ�
		[[nodiscard]] bool Get_Active() const;						//�Q�[���I�u�W�F�N�g���A�N�e�B�u��(�e�̏�Ԃ��l�����Ȃ�)
		void Set_Active(bool value);					//�A�N�e�B�u��Ԃ��w�肷��
		[[nodiscard]] bool Get_Active_In_Hierarchy() const;			//�q�G�����L�[��ŃA�N�e�B�u��(�e�̏�Ԃ��l������)

		int layer = 0;                                          //�R���W������`�攻��p���C���[
		std::string tag = "Default";                            //�^�O
		std::shared_ptr<Transform> transform;                   //�A�^�b�`����Ă���Transform
		std::vector<std::shared_ptr<Component>> component_list; //�A�^�b�`����Ă���R���|�[�l���g�̃��X�g

		template<class T>
		std::shared_ptr<T> Get_Component();	//�A�^�b�`����Ă���R���|�[�l���g���������Ԃ�(���݂��Ȃ��ꍇnull_ptr)
		template<class T>
		std::shared_ptr<T> Add_Component();	//�R���|�[�l���g���A�^�b�`����
		std::shared_ptr<Component> Add_Component(const std::string& class_name);

		static std::weak_ptr<GameObject> Find(const std::string& name);			//�V�[�����̃Q�[���I�u�W�F�N�g�𖼑O�Ō�������
		static std::weak_ptr<GameObject> Find_With_Tag(const std::string& tag);	//�V�[�����̃Q�[���I�u�W�F�N�g���^�O�Ō�������

	private:
		void Initialize(); //������
		void Release();    //��n��

		void Set_Child_Active(bool value) const; //�q�̃A�N�e�B�u��Ԃ�؂�ւ���

		bool active = true;     //�A�N�e�B�u���
		bool active_old = true; //�ύX���g���K�[�p�L���b�V��

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
	for (std::shared_ptr<Component> com : component_list)
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

	//�����A�^�b�`�ł��邩�m�F
	if (std::dynamic_pointer_cast<Component>(buff)->Can_Multiple())
	{
		std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		component_list.emplace_back(buff);
		return buff;
	}

	//���ɃA�^�b�`����Ă��邩�m�F
	bool already_attach = false;
	for (std::shared_ptr<Component> com : component_list)
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
		std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		component_list.emplace_back(buff);
		return buff;
	}

	return nullptr;
}

#include "Transform.h"
template<class Archive>
void BeastEngine::GameObject::serialize(Archive& archive, std::size_t version)
{
	archive(cereal::base_class<Object>(this), layer, tag, transform, component_list, active, active_old);
}