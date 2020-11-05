#pragma once
#include "Component.h"
#include <typeinfo>
#include <string>
#include <list>
#include <memory>

class Scene;

class GameObject : public Object
{
public:
	bool CompareTag(std::string _tag);								//�w�肵���^�O�Ɉ�v���邩�Ԃ�
	bool activeSelf();												//�Q�[���I�u�W�F�N�g���A�N�e�B�u��(�e�̏�Ԃ��l�����Ȃ�)
	bool activeInHierarchy();										//�q�G�����L�[��ŃA�N�e�B�u��(�e�̏�Ԃ��l������)
	void SetActive(bool value);										//�A�N�e�B�u��Ԃ��w�肷��

	int layer = 0;
	std::string tag = "Default";
	std::shared_ptr<Transform> transform;
	std::vector<std::shared_ptr<Component>> Component_List;

	template<class T>
	std::shared_ptr<T> GetComponent();								//�A�^�b�`�T��Ă���R���|�[�l���g���������Ԃ�(���݂��Ȃ��ꍇnull_ptr)
	template<class T>
	std::shared_ptr<T> AddComponent();								//�R���|�[�l���g���A�^�b�`����

	static std::weak_ptr<GameObject> Find(std::string Name);		//�V�[�����̃Q�[���I�u�W�F�N�g�𖼑O�Ō�������
	static std::weak_ptr<GameObject> FindWithTag(std::string Tag);	//�V�[�����̃Q�[���I�u�W�F�N�g���^�O�Ō�������

private:

	void Initialize();
	void Set_Child_Active(bool value);
	bool Active = true;
	bool Old_Active = true;

	friend class Scene;
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);
};

CEREAL_REGISTER_TYPE(GameObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Object, GameObject)

template<class T>
std::shared_ptr<T> GameObject::GetComponent()
{
	for (std::shared_ptr<Component> com : Component_List)
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
std::shared_ptr<T> GameObject::AddComponent()
{
	bool already_attach = false;
	for (std::shared_ptr<Component> com : Component_List)
	{
		std::shared_ptr<T> buff = std::dynamic_pointer_cast<T>(com);
		if (buff != nullptr)
		{
			if (typeid(shared_ptr<T>) == typeid(buff))
			{
				already_attach = true;
			}
		}
	}
	if (!already_attach)
	{
		std::shared_ptr<T> buff = std::make_shared<T>();
		std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		Component_List.emplace_back(buff);
		return buff;
	}
	return nullptr;
}

#include "Transform.h"
template<class Archive>
void GameObject::serialize(Archive& archive)
{
	archive(cereal::base_class<Object>(this), layer, tag, transform, Component_List, Active, Old_Active);
}
