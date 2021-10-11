#pragma once
#include <typeinfo>
#include <string>
#include <list>
#include <memory>
#include "Component.h"

namespace BeastEngine
{
	class GameObject : public Object
	{
	public:
		bool Compare_Tag(const std::string& tag) const;								//指定したタグに一致するか返す
		bool Get_Active() const;												//ゲームオブジェクトがアクティブか(親の状態を考慮しない)
		void Set_Active(bool value);									//アクティブ状態を指定する
		bool Get_Active_In_Hierarchy() const;									//ヒエラルキー上でアクティブか(親の状態を考慮する)

		int layer = 0;
		std::string tag = "Default";
		std::shared_ptr<Transform> transform;
		std::vector<std::shared_ptr<Component>> component_list;

		template<class T>
		std::shared_ptr<T> Get_Component();								//アタッチサれているコンポーネントを検索し返す(存在しない場合null_ptr)
		template<class T>
		std::shared_ptr<T> Add_Component();								//コンポーネントをアタッチする
		std::shared_ptr<Component> Add_Component(const std::string& class_name);

		static std::weak_ptr<GameObject> Find(const std::string& name);			//シーン内のゲームオブジェクトを名前で検索する
		static std::weak_ptr<GameObject> Find_With_Tag(const std::string& tag);	//シーン内のゲームオブジェクトをタグで検索する

	private:

		void Initialize();
		void Release();
		void Set_Child_Active(bool value) const;
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

	if (std::dynamic_pointer_cast<Component>(buff)->Can_Multiple())
	{
		std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		component_list.emplace_back(buff);
		return buff;
	}

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