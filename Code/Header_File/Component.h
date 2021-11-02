#pragma once
#include "Object.h"
#include <typeinfo>
#include <memory>

namespace BeastEngine
{
	class Editor;

	//�Q�[���I�u�W�F�N�g�ɃA�^�b�`�ł���R���|�[�l���g�̊��N���X
	class Component : public Object
	{
	public:
		std::shared_ptr<GameObject> gameobject; //�A�^�b�`���Ă���GameObject
		std::shared_ptr<Transform> transform;   //�A�^�b�`���Ă���GameObject��Transform

		[[nodiscard]] bool Compare_Tag(const std::string& tag) const; //GameObject�̃^�O�������ƈ�v���Ă��邩���擾����

		template<class T>
		std::shared_ptr<T> Get_Component(); //GameObject�ɃA�^�b�`���ꂽ�R���|�[�l���g���擾����(���݂��Ȃ��ꍇnull_ptr)
		template<class T>
		std::shared_ptr<T> Add_Component();                                      //GameObject�ɃR���|�[�l���g���A�^�b�`����(�e���v���[�g��)
		std::shared_ptr<Component> Add_Component(const std::string& class_name); //GameObject�ɃR���|�[�l���g���A�^�b�`����(�N���X����)

	protected:
		virtual void Set_Active(bool value) {}; //�A�N�e�B�u��Ԃ�؂�ւ���
		Component() = default;

	private:

		virtual bool Can_Multiple() { return true; };                       //�����R���|�[�l���g�𕡐��A�^�b�`�\��
		virtual void Initialize(const std::shared_ptr<GameObject>& obj) {}; //������
		virtual bool Draw_ImGui() { return true; };                         //ImGui�ŕ\�����邽�߂̋��ʊ֐�

		friend class GameObject;
		friend class Editor;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this));
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Component)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Component)
CEREAL_CLASS_VERSION(BeastEngine::Component, 1)

namespace BeastEngine::Component_Data
{
	template<class T>
	std::shared_ptr<Component> Create_Component() { return std::make_shared<T>(); }

	//�R���|�[�l���g�𖼑O����쐬���邽�߂̃t�@�N�g���[�N���X
	struct Component_Factory
	{
		typedef std::map<std::string, std::shared_ptr<Component>(*)()> map_type;

		//string����R���|�[�l���g���쐬����
		static std::shared_ptr<Component> Create_Instance(std::string const& s)
		{
			const auto it = Get_Map()->find(s);
			if (it == Get_Map()->end())
				return nullptr;
			return it->second();
		}

		//���O�̃��X�g�ɓo�^����Ă��邩�m�F����
		static std::shared_ptr<map_type> Get_Map()
		{
			if (!map) { map = std::make_shared<map_type>(); }
			return map;
		}

	private:
		static std::shared_ptr<map_type> map;
	};

	//�}�N������o�^���邽�߂̃��W�X�^�[�N���X
	template<class T>
	struct Component_Register : Component_Factory
	{
		explicit Component_Register(std::string const& s)
		{
			Get_Map()->insert(std::make_pair(s, &Create_Component<T>));
		}
	};
}

//���O����R���|�[�l���g���쐬���邽�߂Ƀt�@�N�g���[�ɓo�^����}�N��
#define REGISTER_COMPONENT(NAME) \
namespace BeastEngine\
{\
	namespace Component_Data\
	{\
		static BeastEngine::Component_Data::Component_Register<NAME> reg_##NAME(#NAME);\
	}\
}

template<class T>
std::shared_ptr<T> BeastEngine::Component::Get_Component()
{
	for (std::shared_ptr<Component>& com : gameobject->component_list)
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
std::shared_ptr<T> BeastEngine::Component::Add_Component()
{
	std::shared_ptr<T> buff = std::make_shared<T>();

	//�����A�^�b�`�ł��邩�m�F
	if (std::dynamic_pointer_cast<Component>(buff)->Can_Multiple())
	{
		std::dynamic_pointer_cast<Component>(buff)->Initialize(gameobject);
		gameobject->component_list.emplace_back(buff);
		return buff;
	}

	//���ɃA�^�b�`����Ă��邩�m�F
	bool already_attach = false;
	for (std::shared_ptr<Component>& com : gameobject->component_list)
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
		std::dynamic_pointer_cast<Component>(buff)->Initialize(gameobject);
		gameobject->component_list.emplace_back(buff);
		return buff;
	}

	return nullptr;
}