#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
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
		bool hasChanged = false;

		//GetterSetter

		BeastEngine::Vector3	 Get_position() const;											//TransformのPosition(座標)を返す
		void					 Set_position(BeastEngine::Vector3 V);							//TransformのPosition(座標)を設定する
		void					 Set_position(float f1, float f2, float f3);					//TransformのPosition(座標)を設定する
		BeastEngine::Quaternion	 Get_rotation() const;											//TransformのRotation(クォータニオン)を返す
		void					 Set_rotation(BeastEngine::Quaternion Q);						//TransformのRotation(クォータニオン)を設定する
		void					 Set_rotation(float f1, float f2, float f3, float f4);			//TransformのRotation(クォータニオン)を設定する
		BeastEngine::Vector3	 Get_scale() const;												//TransformのScale(サイズ)を返す
		void					 Set_scale(BeastEngine::Vector3 V);								//TransformのScale(サイズ)を設定する
		void					 Set_scale(float f1, float f2, float f3);						//TransformのScale(サイズ)を設定する

		BeastEngine::Vector3	 Get_localPosition() const;										//TransformのLocalPosition(ローカル座標)を返す
		void					 Set_localPosition(BeastEngine::Vector3 V);						//TransformのLocalPosition(ローカル座標)を設定する
		void					 Set_localPosition(float f1, float f2, float f3);				//TransformのLocalPosition(ローカル座標)を設定する
		BeastEngine::Quaternion	 Get_localRotation() const;										//TransformのLocalRotation(ローカルクォータニオン)を返す
		void					 Set_localRotation(BeastEngine::Quaternion Q);					//TransformのLocalRotation(ローカルクォータニオン)を設定する
		void					 Set_localRotation(float f1, float f2, float f3, float f4);		//TransformのLocalRotation(ローカルクォータニオン)を設定する
		BeastEngine::Vector3	 Get_localScale() const;										//TransformのLocalScale(ローカルサイズ)を返す
		void					 Set_localScale(BeastEngine::Vector3 V);						//TransformのLocalScale(ローカルサイズ)を設定する
		void					 Set_localScale(float f1, float f2, float f3);					//TransformのLocalScale(ローカルサイズ)を設定する

		BeastEngine::Vector3	 Get_forward() const;											//Transformのワールド空間でのZ軸(前方向)を返す
		BeastEngine::Vector3	 Get_right() const;												//Transformのワールド空間でのX軸(右方向)を返す
		BeastEngine::Vector3	 Get_up() const;												//Transformのワールド空間でのY軸(上方向)を返す

		BeastEngine::Vector3	 Get_eulerAngles() const;										//TransformのRotation(クォータニオン)をオイラー角に変換して返す
		void					 Set_eulerAngles(BeastEngine::Vector3 V);						//TransformのRotation(クォータニオン)にオイラー角を変換して設定する
		void					 Set_eulerAngles(float f1, float f2, float f3);					//TransformのRotation(クォータニオン)にオイラー角を変換して設定する
		BeastEngine::Vector3	 Get_localEulerAngles() const;									//TransformのLocalRotation(ローカルクォータニオン)をオイラー角に変換して返す
		void					 Set_localEulerAngles(BeastEngine::Vector3 V);					//TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する
		void					 Set_localEulerAngles(float f1, float f2, float f3);			//TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する

		std::weak_ptr<Transform> Get_parent() const;											//Transformの親を返す
		void					 Set_parent(std::shared_ptr<Transform> P);						//Transformの親を設定する
		void					 Set_parent(std::shared_ptr<Transform> P, int index_insert);	//Transformの親を設定する(自分が何番目の子になるか指定する)

		std::weak_ptr<Transform> GetChild(int index);											//Transformの指定したインデックスの子を返す
		int						 childCount();													//Transformが子をいくつ保持しているかを返す(子が居ない場合、０)

		int						 GetSiblingIndex() const;										//ヒエラルキー上でその階層(兄弟関係)の中で何番目かを返す
		void					 SetSiblingIndex(int index);									//ヒエラルキー上でその階層(兄弟関係)の中での順番をインデックスで指定する

		BeastEngine::Matrix		 Get_world_matrix() const;										//Transformのワールド変換行列を返す
		void					 Set_world_matrix(BeastEngine::Matrix matrix);					//Transformのワールド変換行列を直接設定する

		//Function

		BeastEngine::Quaternion	 LookAt(BeastEngine::Vector3 pos);								//TransformのForwardを指定した座標へ向ける

	private:

		BeastEngine::Vector3    position = { 0, 0, 0 };
		BeastEngine::Quaternion rotation = { 0, 0, 0 ,1 };
		BeastEngine::Vector3    scale = { 1, 1, 1 };

		BeastEngine::Vector3    localPosition = { 0, 0, 0 };
		BeastEngine::Quaternion localRotation = { 0, 0, 0 ,1 };
		BeastEngine::Vector3    localScale = { 1, 1, 1 };

		BeastEngine::Vector3 forward = { 0.0f, 0.0f, 1.0f };
		BeastEngine::Vector3 right = { 1.0f, 0.0f, 0.0f };
		BeastEngine::Vector3 up = { 0.0f, 1.0f, 0.0f };

		BeastEngine::Matrix world_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		BeastEngine::Matrix scale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix rotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix translation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		BeastEngine::Matrix local_matrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
		BeastEngine::Matrix localScale_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix localRotation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		BeastEngine::Matrix localTranslation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

		std::weak_ptr<Transform> parent;
		std::vector<std::weak_ptr<Transform>> children;

		bool Update_GUI = false;

		void OnParentChanged();
		void Change_Children();
		void Remove_Parent();

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj) override;
		bool Draw_ImGui() override;
		bool CanMultiple() override { return false; };

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
		gameObject,
		position, rotation, scale,
		localPosition, localRotation, localScale,
		forward, right, up,
		world_matrix, scale_matrix, rotation_matrix, translation_matrix,
		local_matrix, localScale_matrix, localRotation_matrix, localTranslation_matrix,
		parent, children);
}