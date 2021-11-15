#pragma once
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include <unordered_map>
#include "GameObject.h"

namespace BeastEngine
{
	class Transform final : public Component
	{
	public:
		Transform();
		Transform(Vector3 position, Quaternion rotation);
		Transform(Vector3 position, Vector3 euler);

		//falseに設定されてから変更があったかどうか
		bool has_changed = false;

		//GetterSetter

		[[nodiscard]] Vector3 Get_Position() const;							//TransformのPosition(座標)を返す
		void Set_Position(Vector3 v);							            //TransformのPosition(座標)を設定する
		void Set_Position(float f1, float f2, float f3);					//TransformのPosition(座標)を設定する
		[[nodiscard]] Quaternion Get_Rotation() const;						//TransformのRotation(クォータニオン)を返す
		void Set_Rotation(Quaternion q);						            //TransformのRotation(クォータニオン)を設定する
		void Set_Rotation(float f1, float f2, float f3, float f4);			//TransformのRotation(クォータニオン)を設定する
		[[nodiscard]] Vector3 Get_Scale() const;							//TransformのScale(サイズ)を返す
		void Set_Scale(Vector3 v);								            //TransformのScale(サイズ)を設定する
		void Set_Scale(float f1, float f2, float f3);						//TransformのScale(サイズ)を設定する

		[[nodiscard]] Vector3 Get_Local_Position() const;					//TransformのLocalPosition(ローカル座標)を返す
		void Set_Local_Position(Vector3 v);					                //TransformのLocalPosition(ローカル座標)を設定する
		void Set_Local_Position(float f1, float f2, float f3);				//TransformのLocalPosition(ローカル座標)を設定する
		[[nodiscard]] Quaternion Get_Local_Rotation() const;				//TransformのLocalRotation(ローカルクォータニオン)を返す
		void Set_Local_Rotation(Quaternion q);					            //TransformのLocalRotation(ローカルクォータニオン)を設定する
		void Set_Local_Rotation(float f1, float f2, float f3, float f4);	//TransformのLocalRotation(ローカルクォータニオン)を設定する
		[[nodiscard]] Vector3 Get_Local_Scale() const;						//TransformのLocalScale(ローカルサイズ)を返す
		void Set_Local_Scale(Vector3 v);						            //TransformのLocalScale(ローカルサイズ)を設定する
		void Set_Local_Scale(float f1, float f2, float f3);					//TransformのLocalScale(ローカルサイズ)を設定する

		[[nodiscard]] Vector3 Get_Forward() const;							//Transformのワールド空間でのZ軸(前方向)を返す
		[[nodiscard]] Vector3 Get_Right() const;							//Transformのワールド空間でのX軸(右方向)を返す
		[[nodiscard]] Vector3 Get_Up() const;								//Transformのワールド空間でのY軸(上方向)を返す

		[[nodiscard]] Vector3 Get_Euler_Angles() const;						//TransformのRotation(クォータニオン)をオイラー角に変換して返す
		void Set_Euler_Angles(Vector3 v);						            //TransformのRotation(クォータニオン)にオイラー角を変換して設定する
		void Set_Euler_Angles(float f1, float f2, float f3);				//TransformのRotation(クォータニオン)にオイラー角を変換して設定する
		[[nodiscard]] Vector3 Get_Local_Euler_Angles() const;				//TransformのLocalRotation(ローカルクォータニオン)をオイラー角に変換して返す
		void Set_Local_Euler_Angles(Vector3 v);				                //TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する
		void Set_Local_Euler_Angles(float f1, float f2, float f3);			//TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する

		[[nodiscard]] std::weak_ptr<Transform> Get_Parent() const;			    //Transformの親を返す
		void Set_Parent(const std::shared_ptr<Transform>& p);					//Transformの親を設定する
		void Set_Parent(const std::shared_ptr<Transform>& p, int index_insert);	//Transformの親を設定する(自分が何番目の子になるか指定する)

		std::weak_ptr<Transform> Get_Child(int index);						    //Transformの指定したインデックスの子を返す
		int	Get_Child_Count() const;											//Transformが子をいくつ保持しているかを返す(子が居ない場合、０)

		[[nodiscard]] int Get_Sibling_Index() const;						    //ヒエラルキー上でその階層(兄弟関係)の中で何番目かを返す
		void Set_Sibling_Index(int index);									    //ヒエラルキー上でその階層(兄弟関係)の中での順番をインデックスで指定する

		[[nodiscard]] Matrix Get_World_Matrix() const;						    //Transformのワールド変換行列を返す
		void Set_World_Matrix(Matrix matrix);					                //Transformのワールド変換行列を直接設定する

																			    //Function
		[[nodiscard]] std::weak_ptr<Transform> Find(std::string n) const;		//Transform以下のオブジェクトを検索する
		[[nodiscard]] Quaternion Look_At(Vector3 pos) const;					//TransformのForwardを指定した座標へ向ける

	private:
		Vector3    position = { 0, 0, 0 };
		Quaternion rotation = { 0, 0, 0 ,1 };
		Vector3    scale = { 1, 1, 1 };

		Vector3    local_position = { 0, 0, 0 };
		Quaternion local_rotation = { 0, 0, 0 ,1 };
		Vector3    local_scale = { 1, 1, 1 };

		Vector3 forward = { 0.0f, 0.0f, 1.0f };
		Vector3 right = { 1.0f, 0.0f, 0.0f };
		Vector3 up = { 0.0f, 1.0f, 0.0f };

		Matrix world_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		Matrix scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		Matrix local_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		Matrix local_scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix local_rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		Matrix local_translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		std::weak_ptr<Transform> parent;
		std::vector<std::weak_ptr<Transform>> children{};

		bool update_GUI = false;

		void On_Parent_Changed(); //親Transformの姿勢が変更されたときに呼ばれる
		void Change_Children(); //自身の姿勢が変更されたときに子に対して処理を行う
		void Remove_Parent(); //親子関係を解除する

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //初期化
		bool Draw_ImGui() override; //ImGui描画
		bool Can_Multiple() override { return false; } //同コンポーネントを複数アタッチできるか

		friend class Editor;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version);
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Transform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Transform)
CEREAL_CLASS_VERSION(BeastEngine::Transform, 1)

template<class Archive>
void BeastEngine::Transform::serialize(Archive& archive, std::uint32_t const version)
{
	archive(cereal::base_class<Component>(this),
		gameobject,
		position, rotation, scale,
		local_position, local_rotation, local_scale,
		forward, right, up,
		world_matrix, scale_matrix, rotation_matrix, translation_matrix,
		local_matrix, local_scale_matrix, local_rotation_matrix, local_translation_matrix,
		parent, children);
}