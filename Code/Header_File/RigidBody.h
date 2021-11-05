#pragma once
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btTransform.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include <memory>
#include "Original_Math.h"

namespace BeastEngine
{
	class Transform;
	class Collider;

	//加速度加算時の力の計算方法
	enum class Force_Mode
	{
		Force,
		Impulse
	};

	//リジッドボディコンポーネント (Unityとは違い、コライダーに内蔵)
	class RigidBody
	{
	public:
		~RigidBody();

		void Set_Mass(float set_mass);        //質量を設定する
		[[nodiscard]] float Get_Mass() const; //質量を取得する

		void Set_Kinematic(bool value); //キネマティックコライダーかどうかを設定する
		void Set_Dynamic(bool value);   //ダイナミックコライダーかどうかを設定する
		void Use_Gravity(bool value);   //重力に影響されるか設定する

		void Set_Freeze_Position(bool x_axis, bool y_axis, bool z_axis); //軸ごとに座標を固定する
		void Set_Freeze_Rotation(bool x_axis, bool y_axis, bool z_axis); //軸ごとに回転を固定する

		void Set_Angular_Velocity(Vector3 velocity) const;  //角速度を設定する
		[[nodiscard]] Vector3 Get_Angular_Velocity() const; //角速度を取得する
		void Set_Velocity(Vector3 velocity) const;  //加速度を設定する
		[[nodiscard]] Vector3 Get_Velocity() const; //加速度を取得する

		void Add_Force(Vector3 force, Force_Mode mode = Force_Mode::Force) const;
		void Add_Force_AtPosition(Vector3 force, Vector3 position, Force_Mode mode = Force_Mode::Force) const;

	private:
		void Initialize(const std::shared_ptr<Collider>& col); //初期化
		void Create();       //リジッドボディを作成しマネージャーに登録する
		void Resize() const; //リサイズする
		void Remove();       //マネージャーから削除する

		void Get_BtTransform(btTransform& t) const;       //Bullet内での姿勢を取得する
		void Set_BtTransform(const btTransform& t) const; //Bullet内での姿勢を設定する
		void Set_Debug_Draw(bool value) const;            //デバッグ描画に登録する
		bool Get_Debug_Drawed() const;                    //デバッグ描画に登録するか取得する

		std::unique_ptr<btRigidBody> rigidbody;
		std::unique_ptr<btDefaultMotionState> motion_state;
		std::weak_ptr<Collider> collider;

		float mass = 1.0f;         //質量
		float send_mass = 1.0f;    //Bullet内部で使用している質量
		bool is_dynamic = true;    //ダイナミックコライダーか
		bool is_kinematic = false; //キネマティックコライダーか
		Vector3 gravity = { 0.0f, -9.8f, 0.0f };       //重力
		Vector3 linear_factor = { 1.0f, 1.0f, 1.0f };  //加速度
		Vector3 angular_factor = { 1.0f, 1.0f, 1.0f }; //角加速度

		friend class Collider;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(mass, is_dynamic, is_kinematic, gravity, linear_factor, angular_factor);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::RigidBody, 1)