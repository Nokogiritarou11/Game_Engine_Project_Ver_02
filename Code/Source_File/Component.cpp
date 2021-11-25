#include "Component.h"
#include "GameObject.h"
using namespace std;
using namespace BeastEngine;
using namespace Component_Data;

shared_ptr<Component_Factory::map_type> Component_Factory::map;

bool Component::Compare_Tag(const string& tag) const
{
	return gameobject->tag == tag;
}

shared_ptr<Component> Component::Add_Component(const string& class_name)
{
	//�t�@�N�g���[�N���X���疼�O�ŃR���|�[�l���g�𐶐�����
	if (shared_ptr<Component> buff = Component_Factory::Create_Instance(class_name))
	{
		//�����A�^�b�`�ł��邩
		if (buff->Can_Multiple())
		{
			buff->Initialize(static_pointer_cast<GameObject>(shared_from_this()));
			gameobject->component_list.emplace_back(buff);
			return buff;
		}

		bool already_attach = false;
		for (shared_ptr<Component>& com : gameobject->component_list)
		{
			if (typeid(*buff) == typeid(*com))
			{
				already_attach = true;
				break;
			}
		}
		//���ɃA�^�b�`����Ă��邩
		if (!already_attach)
		{
			buff->Initialize(static_pointer_cast<GameObject>(shared_from_this()));
			gameobject->component_list.emplace_back(buff);
			return buff;
		}
	}
	return nullptr;
}