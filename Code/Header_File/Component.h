#pragma once
#include "Object.h"
#include <typeinfo>
#include <memory>

namespace BeastEngine
{
	class Editor;

	//ゲームオブジェクトにアタッチできるコンポーネントの基底クラス
	class Component : public Object
	{
	public:
		std::shared_ptr<GameObject> gameobject; //アタッチしているGameObject
		std::shared_ptr<Transform> transform;   //アタッチしているGameObjectのTransform

		[[nodiscard]] bool Compare_Tag(const std::string& tag) const; //GameObjectのタグが引数と一致しているかを取得する

		template<class T>
		std::shared_ptr<T> Get_Component(); //GameObjectにアタッチされたコンポーネントを取得する(存在しない場合null_ptr)
		template<class T>
		std::shared_ptr<T> Add_Component();                                      //GameObjectにコンポーネントをアタッチする(テンプレート版)
		std::shared_ptr<Component> Add_Component(const std::string& class_name); //GameObjectにコンポーネントをアタッチする(クラス名版)

	protected:
		virtual void Set_Active(bool value) {}; //アクティブ状態を切り替える
		Component() = default;

	private:

		virtual bool Can_Multiple() { return true; };                       //同じコンポーネントを複数アタッチ可能か
		virtual void Initialize(const std::shared_ptr<GameObject>& obj) {}; //初期化
		virtual bool Draw_ImGui() { return true; };                         //ImGuiで表示するための共通関数

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

	//コンポーネントを名前から作成するためのファクトリークラス
	struct Component_Factory
	{
		typedef std::map<std::string, std::shared_ptr<Component>(*)()> map_type;

		//stringからコンポーネントを作成する
		static std::shared_ptr<Component> Create_Instance(std::string const& s)
		{
			const auto it = Get_Map()->find(s);
			if (it == Get_Map()->end())
				return nullptr;
			return it->second();
		}

		//名前のリストに登録されているか確認する
		static std::shared_ptr<map_type> Get_Map()
		{
			if (!map) { map = std::make_shared<map_type>(); }
			return map;
		}

	private:
		static std::shared_ptr<map_type> map;
	};

	//マクロから登録するためのレジスタークラス
	template<class T>
	struct Component_Register : Component_Factory
	{
		explicit Component_Register(std::string const& s)
		{
			Get_Map()->insert(std::make_pair(s, &Create_Component<T>));
		}
	};
}

//名前からコンポーネントを作成するためにファクトリーに登録するマクロ
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

	//複数アタッチできるか確認
	if (std::dynamic_pointer_cast<Component>(buff)->Can_Multiple())
	{
		std::dynamic_pointer_cast<Component>(buff)->Initialize(gameobject);
		gameobject->component_list.emplace_back(buff);
		return buff;
	}

	//既にアタッチされているか確認
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