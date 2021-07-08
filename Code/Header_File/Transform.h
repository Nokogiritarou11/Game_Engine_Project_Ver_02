#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include <unordered_map>
#include "GameObject.h"

namespace BeastEngine
{
	class Transform : public Component
	{
	public:

		Transform();
		Transform(BeastEngine::Vector3 _position, BeastEngine::Quaternion _rotation);
		Transform(BeastEngine::Vector3 _position, BeastEngine::Vector3 _euler);
		~Transform();

		//falseに設定されてから変更があったかどうか
		bool has_changed = false;

		//GetterSetter

		BeastEngine::Vector3	 Get_Position() const;											//TransformのPosition(座標)を返す
		void					 Set_Position(BeastEngine::Vector3 V);							//TransformのPosition(座標)を設定する
		void					 Set_Position(float f1, float f2, float f3);					//TransformのPosition(座標)を設定する
		BeastEngine::Quaternion	 Get_Rotation() const;											//TransformのRotation(クォータニオン)を返す
		void					 Set_Rotation(BeastEngine::Quaternion Q);						//TransformのRotation(クォータニオン)を設定する
		void					 Set_Rotation(float f1, float f2, float f3, float f4);			//TransformのRotation(クォータニオン)を設定する
		BeastEngine::Vector3	 Get_Scale() const;												//TransformのScale(サイズ)を返す
		void					 Set_Scale(BeastEngine::Vector3 V);								//TransformのScale(サイズ)を設定する
		void					 Set_Scale(float f1, float f2, float f3);						//TransformのScale(サイズ)を設定する

		BeastEngine::Vector3	 Get_Local_Position() const;									//TransformのLocalPosition(ローカル座標)を返す
		void					 Set_Local_Position(BeastEngine::Vector3 V);					//TransformのLocalPosition(ローカル座標)を設定する
		void					 Set_Local_Position(float f1, float f2, float f3);				//TransformのLocalPosition(ローカル座標)を設定する
		BeastEngine::Quaternion	 Get_Local_Rotation() const;									//TransformのLocalRotation(ローカルクォータニオン)を返す
		void					 Set_Local_Rotation(BeastEngine::Quaternion Q);					//TransformのLocalRotation(ローカルクォータニオン)を設定する
		void					 Set_Local_Rotation(float f1, float f2, float f3, float f4);	//TransformのLocalRotation(ローカルクォータニオン)を設定する
		BeastEngine::Vector3	 Get_Local_Scale() const;										//TransformのLocalScale(ローカルサイズ)を返す
		void					 Set_Local_Scale(BeastEngine::Vector3 V);						//TransformのLocalScale(ローカルサイズ)を設定する
		void					 Set_Local_Scale(float f1, float f2, float f3);					//TransformのLocalScale(ローカルサイズ)を設定する

		BeastEngine::Vector3	 Get_Forward() const;											//Transformのワールド空間でのZ軸(前方向)を返す
		BeastEngine::Vector3	 Get_Right() const;												//Transformのワールド空間でのX軸(右方向)を返す
		BeastEngine::Vector3	 Get_Up() const;												//Transformのワールド空間でのY軸(上方向)を返す

		BeastEngine::Vector3	 Get_Euler_Angles() const;										//TransformのRotation(クォータニオン)をオイラー角に変換して返す
		void					 Set_Euler_Angles(BeastEngine::Vector3 V);						//TransformのRotation(クォータニオン)にオイラー角を変換して設定する
		void					 Set_Euler_Angles(float f1, float f2, float f3);				//TransformのRotation(クォータニオン)にオイラー角を変換して設定する
		BeastEngine::Vector3	 Get_Local_Euler_Angles() const;								//TransformのLocalRotation(ローカルクォータニオン)をオイラー角に変換して返す
		void					 Set_Local_Euler_Angles(BeastEngine::Vector3 V);				//TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する
		void					 Set_Local_Euler_Angles(float f1, float f2, float f3);			//TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する

		std::weak_ptr<Transform> Get_Parent() const;											//Transformの親を返す
		void					 Set_Parent(std::shared_ptr<Transform> P);						//Transformの親を設定する
		void					 Set_Parent(std::shared_ptr<Transform> P, int index_insert);	//Transformの親を設定する(自分が何番目の子になるか指定する)

		std::weak_ptr<Transform> Get_Child(int index);											//Transformの指定したインデックスの子を返す
		int						 Get_Child_Count();												//Transformが子をいくつ保持しているかを返す(子が居ない場合、０)
		std::weak_ptr<Transform> Find(std::string n);
		int						 Get_Sibling_Index() const;										//ヒエラルキー上でその階層(兄弟関係)の中で何番目かを返す
		void					 Set_Sibling_Index(int index);									//ヒエラルキー上でその階層(兄弟関係)の中での順番をインデックスで指定する

		BeastEngine::Matrix		 Get_World_Matrix() const;										//Transformのワールド変換行列を返す
		void					 Set_World_Matrix(BeastEngine::Matrix matrix);					//Transformのワールド変換行列を直接設定する

		//Function

		BeastEngine::Quaternion	 Look_At(BeastEngine::Vector3 pos);								//TransformのForwardを指定した座標へ向ける

	private:

		BeastEngine::Vector3    position = { 0, 0, 0 };
		BeastEngine::Quaternion rotation = { 0, 0, 0 ,1 };
		BeastEngine::Vector3    scale = { 1, 1, 1 };

		BeastEngine::Vector3    local_position = { 0, 0, 0 };
		BeastEngine::Quaternion local_rotation = { 0, 0, 0 ,1 };
		BeastEngine::Vector3    local_scale = { 1, 1, 1 };

		BeastEngine::Vector3 forward = { 0.0f, 0.0f, 1.0f };
		BeastEngine::Vector3 right = { 1.0f, 0.0f, 0.0f };
		BeastEngine::Vector3 up = { 0.0f, 1.0f, 0.0f };

		BeastEngine::Matrix world_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		BeastEngine::Matrix scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		BeastEngine::Matrix local_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		BeastEngine::Matrix local_scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix local_rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix local_translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		std::weak_ptr<Transform> parent;
		std::vector<std::weak_ptr<Transform>> children;

		bool update_GUI = false;

		void On_Parent_Changed();
		void Change_Children();
		void Remove_Parent();

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool Can_Multiple() override { return false; };

		friend class Editor;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive);
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Transform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Transform)

template<class Archive>
void BeastEngine::Transform::serialize(Archive& archive)
{
	archive(cereal::base_class<BeastEngine::Component>(this),
		gameobject,
		position, rotation, scale,
		local_position, local_rotation, local_scale,
		forward, right, up,
		world_matrix, scale_matrix, rotation_matrix, translation_matrix,
		local_matrix, local_scale_matrix, local_rotation_matrix, local_translation_matrix,
		parent, children);
}