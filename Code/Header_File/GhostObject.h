#pragma once
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "LinearMath/btTransform.h"
#include <memory>

namespace BeastEngine
{
	class Transform;
	class Collider;

	//ゴースト(トリガー)コライダー
	class GhostObject
	{
	public:
		~GhostObject();

	private:
		void Initialize(std::shared_ptr<Collider> col); //初期化
		void Create();       //作成
		void Resize() const; //リサイズ
		void Remove();       //削除
		void Get_BtTransform(btTransform& t) const;       //Bullet内での座標を取得する
		void Set_BtTransform(const btTransform& t) const; //Bullet内の座標を変更する
		void Set_Debug_Draw(bool value) const;            //デバッグ描画に登録する
		[[nodiscard]] bool Get_Debug_Drawed() const;      //デバッグ描画に登録されているか取得する

		std::unique_ptr<btGhostObject> ghost{};
		std::weak_ptr<Collider> collider;

		friend class Collider;
	};
}