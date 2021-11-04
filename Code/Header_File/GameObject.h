#pragma once
#include <typeinfo>
#include <string>
#include <list>
#include <memory>
#include "Component.h"

namespace BeastEngine
{
	//ゲームオブジェクト
	class GameObject final : public Object
	{
	public:
		[[nodiscard]] bool Compare_Tag(const std::string& tag) const;	//指定したタグに一致するか返す
		[[nodiscard]] bool Get_Active() const;						//ゲームオブジェクトがアクティブか(親の状態を考慮しない)
		void Set_Active(bool value);					//アクティブ状態を指定する
		[[nodiscard]] bool Get_Active_In_Hierarchy() const;			//ヒエラルキー上でアクティブか(親の状態を考慮する)

		int layer = 0;                                          //コリジョンや描画判定用レイヤー
		std::string tag = "Default";                            //タグ
		std::shared_ptr<Transform> transform;                   //アタッチされているTransform
		std::vector<std::shared_ptr<Component>> component_list; //アタッチされているコンポーネントのリスト

		template<class T>
		std::shared_ptr<T> Get_Component();	//アタッチされているコンポーネントを検索し返す(存在しない場合null_ptr)
		template<class T>
		std::shared_ptr<T> Add_Component();	//コンポーネントをアタッチする
		std::shared_ptr<Component> Add_Component(const std::string& class_name);

		static std::weak_ptr<GameObject> Find(const std::string& name);			//シーン内のゲームオブジェクトを名前で検索する
		static std::weak_ptr<GameObject> Find_With_Tag(const std::string& tag);	//シーン内のゲームオブジェクトをタグで検索する

	private:
		void Initialize(); //初期化
		void Release();    //後始末

		void Set_Child_Active(bool value) const; //子のアクティブ状態を切り替える

		bool active = true;     //アクティブ状態
		bool active_old = true; //変更時トリガー用キャッシュ

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

	//複数アタッチできるか確認
	if (std::dynamic_pointer_cast<Component>(buff)->Can_Multiple())
	{
		std::dynamic_pointer_cast<Component>(buff)->Initialize(std::static_pointer_cast<GameObject>(shared_from_this()));
		component_list.emplace_back(buff);
		return buff;
	}

	//既にアタッチされているか確認
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