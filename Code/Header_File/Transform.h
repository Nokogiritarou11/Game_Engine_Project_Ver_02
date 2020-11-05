#pragma once
#include <stdio.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>
#include "GameObject.h"

class Transform : public Component
{
public:

	Transform();
	Transform(Vector3 _position, Quaternion _rotation);
	Transform(Vector3 _position, Vector3 _euler);
	~Transform();

	//falseに設定されてから変更があったかどうか
	bool hasChanged = false;

	//GetterSetter

	Vector3					 Get_position        () const;											//TransformのPosition(座標)を返す
	void					 Set_position        (Vector3 V);										//TransformのPosition(座標)を設定する
	void					 Set_position        (float f1, float f2, float f3);					//TransformのPosition(座標)を設定する
	Quaternion				 Get_rotation        () const;											//TransformのRotation(クォータニオン)を返す
	void					 Set_rotation        (Quaternion Q);									//TransformのRotation(クォータニオン)を設定する
	void					 Set_rotation        (float f1, float f2, float f3, float f4);			//TransformのRotation(クォータニオン)を設定する
	Vector3					 Get_scale           () const;											//TransformのScale(サイズ)を返す
	void					 Set_scale           (Vector3 V);										//TransformのScale(サイズ)を設定する
	void					 Set_scale           (float f1, float f2, float f3);					//TransformのScale(サイズ)を設定する

	Vector3					 Get_localPosition   () const;											//TransformのLocalPosition(ローカル座標)を返す
	void					 Set_localPosition   (Vector3 V);										//TransformのLocalPosition(ローカル座標)を設定する
	void					 Set_localPosition   (float f1, float f2, float f3);					//TransformのLocalPosition(ローカル座標)を設定する
	Quaternion				 Get_localRotation   () const;											//TransformのLocalRotation(ローカルクォータニオン)を返す
	void					 Set_localRotation   (Quaternion Q);									//TransformのLocalRotation(ローカルクォータニオン)を設定する
	void					 Set_localRotation   (float f1, float f2, float f3, float f4);			//TransformのLocalRotation(ローカルクォータニオン)を設定する
	Vector3					 Get_localScale      () const;											//TransformのLocalScale(ローカルサイズ)を返す
	void					 Set_localScale      (Vector3 V);										//TransformのLocalScale(ローカルサイズ)を設定する
	void					 Set_localScale      (float f1, float f2, float f3);					//TransformのLocalScale(ローカルサイズ)を設定する

	Vector3					 Get_forward         () const;											//Transformのワールド空間でのZ軸(前方向)を返す
	Vector3					 Get_right           () const;											//Transformのワールド空間でのX軸(右方向)を返す
	Vector3					 Get_up              () const;											//Transformのワールド空間でのY軸(上方向)を返す

	Vector3					 Get_eulerAngles     () const;											//TransformのRotation(クォータニオン)をオイラー角に変換して返す
	void					 Set_eulerAngles     (Vector3 V);										//TransformのRotation(クォータニオン)にオイラー角を変換して設定する
	void					 Set_eulerAngles     (float f1, float f2, float f3);					//TransformのRotation(クォータニオン)にオイラー角を変換して設定する
	Vector3					 Get_localEulerAngles() const;											//TransformのLocalRotation(ローカルクォータニオン)をオイラー角に変換して返す
	void					 Set_localEulerAngles(Vector3 V);										//TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する
	void					 Set_localEulerAngles(float f1, float f2, float f3);					//TransformのLocalRotation(ローカルクォータニオン)にオイラー角を変換して設定する

	std::weak_ptr<Transform> Get_parent          () const;											//Transformの親を返す
	void					 Set_parent          (std::shared_ptr<Transform> P);					//Transformの親を設定する
	void					 Set_parent          (std::shared_ptr<Transform> P, int index_insert);	//Transformの親を設定する(自分が何番目の子になるか指定する)

	std::weak_ptr<Transform> GetChild            (int index);										//Transformの指定したインデックスの子を返す
	int						 childCount          ();												//Transformが子をいくつ保持しているかを返す(子が居ない場合、０)

	int						 GetSiblingIndex     () const;											//ヒエラルキー上でその階層(兄弟関係)の中で何番目かを返す
	void					 SetSiblingIndex     (int index);										//ヒエラルキー上でその階層(兄弟関係)の中での順番をインデックスで指定する

	Matrix					 Get_world_matrix    () const;											//Transformのワールド変換行列を返す

	//Function

	Quaternion				 LookAt              (Vector3 pos);										//TransformのForwardを指定した座標へ向ける

private:

	Vector3    position            = { 0, 0, 0 };
	Quaternion rotation            = { 0, 0, 0 ,1 };
	Vector3    scale               = { 1, 1, 1 };

	Vector3    localPosition       = { 0, 0, 0 };
	Quaternion localRotation       = { 0, 0, 0 ,1 };
	Vector3    localScale          = { 1, 1, 1 };

	Vector3 forward                = { 0.0f, 0.0f, 1.0f };
	Vector3 right                  = { 1.0f, 0.0f, 0.0f };
	Vector3 up                     = { 0.0f, 1.0f, 0.0f };

	Matrix world_matrix            = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Matrix scale_matrix            = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix rotation_matrix         = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix translation_matrix      = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	Matrix local_matrix            = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	Matrix localScale_matrix       = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix localRotation_matrix    = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	Matrix localTranslation_matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	std::weak_ptr<Transform> parent;
	std::vector<std::weak_ptr<Transform>> children;

	void OnParentChanged();
	void Change_Children();
	void Remove_Parent();

	void Initialize(std::shared_ptr<GameObject> obj) override;
	bool Draw_ImGui() override;

	friend class Debug_UI;
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive);
};
CEREAL_REGISTER_TYPE(Transform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component,Transform)

template<class Archive>
void Transform::serialize(Archive& archive)
{
	archive(cereal::base_class<Component>(this),
		position, rotation, scale,
		localPosition, localRotation, localScale,
		forward, right, up,
		world_matrix, scale_matrix, rotation_matrix, translation_matrix,
		local_matrix, localScale_matrix, localRotation_matrix, localTranslation_matrix,
		parent, children);
}