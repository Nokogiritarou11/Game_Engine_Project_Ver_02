#include "Player_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Damage_Collision.h"
#include "Editor.h"
#include "Engine.h"
#include "Player_Parameter.h"
#include "Player_Camera_Controller.h"
#include "Object_Pool.h"
#include "Time_Manager.h"

using namespace std;
using namespace BeastEngine;

void Player_Damageable::Awake()
{
	//メンバポインタの取得
	animator = Get_Component<Animator>();
	parameter = Get_Component<Player_Parameter>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();
	time_manager = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Time_Manager>();
}

void Player_Damageable::Update()
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	//無敵状態の切り替え
	param->is_invincible = anim->Get_Bool("Invincible");
}

bool Player_Damageable::Take_Damage(const shared_ptr<Damage_Collision>& damage_collision)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	//無敵状態の場合はリターン
	if (param->is_invincible) return false;

	//攻撃してきた方へ向き直る
	const Vector3 from_pos = damage_collision->root_transform.lock()->Get_Position();
	transform->Set_Local_Rotation(transform->Look_At(Vector3(from_pos.x, transform->Get_Position().y, from_pos.z)));
	hit_stop_manager.lock()->Start_Hit_Stop(0.05f);

	if (param->guarding)
	{
		//ガード状態だった場合
		//ガードを開始してからの時間でジャストガードかどうかを判定する
		if (param->just_guard_timer > 0 && damage_collision->can_parry)
		{
			//ジャストガード成功時
			anim->Set_Trigger("Parry");
			//攻撃してきた相手を反撃が当たる位置へ移動させる
			damage_collision->root_transform.lock()->Set_Local_Position(transform->Get_Position() + transform->Get_Forward() * 2.0f);
			//時間停止とカメラシェイクを行う
			time_manager.lock()->Start_Time_Slow(parry_time_stop_parameter.delay, parry_time_stop_parameter.time, parry_time_stop_parameter.speed);
			camera_controller.lock()->Shake_Camera(parry_camera_shake_parameter.count, parry_camera_shake_parameter.power);
			//ジャストガードエフェクトを再生
			pool.lock()->Instance_In_Pool(parry_particle_key, transform->Get_Position() + transform->Get_Left() * parry_particle_position.x + transform->Get_Up() * parry_particle_position.y + transform->Get_Forward() * parry_particle_position.z, transform->Get_Local_Rotation());
		}
		else
		{
			//通常ガード時
			anim->Set_Trigger("Damage");
			//カメラシェイクとエフェクトの再生を行う
			camera_controller.lock()->Shake_Camera(guard_camera_shake_parameter.count, guard_camera_shake_parameter.power);
			pool.lock()->Instance_In_Pool(guard_particle_key, transform->Get_Position() + transform->Get_Left() * guard_particle_position.x + transform->Get_Up() * guard_particle_position.y + transform->Get_Forward() * guard_particle_position.z, transform->Get_Local_Rotation());
		}

		return true;
	}

	//ダメージ処理とヒットエフェクトの再生
	param->hp -= damage_collision->damage_hp;
	pool.lock()->Instance_In_Pool(damage_collision->hit_particle_key, hit_pos_transform.lock()->Get_Position(), damage_collision->hit_transform.lock()->Get_Rotation());
	anim->Set_Trigger("Damage");
	anim->Set_Int("Damage_State", static_cast<int>(damage_collision->damage_type));
	return true;
}

bool Player_Damageable::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Player_Damageable", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		if (ImGui::TreeNode(u8"パリィ時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"スロー効果"))
			{
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##parry_time_stop_parameter.delay", &parry_time_stop_parameter.delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##parry_time_stop_parameter.time", &parry_time_stop_parameter.time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##parry_time_stop_parameter.speed", &parry_time_stop_parameter.speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##parry_camera_shake_parameter.count", &parry_camera_shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##parry_camera_shake_parameter.power", &parry_camera_shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::LeftText_DragFloat3(u8"エフェクト座標", "##parry_particle_position", parry_particle_position, window_center);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"ガード時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##guard_camera_shake_parameter.count", &guard_camera_shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##guard_camera_shake_parameter.power", &guard_camera_shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::LeftText_DragFloat3(u8"エフェクト座標", "##guard_particle_position", guard_particle_position, window_center);

			ImGui::TreePop();
		}

		ImGui::Text(u8"ヒットエフェクト位置");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);

		string label_parent = u8"未設定 (ここにドラッグ)";
		if (const auto& p = hit_pos_transform.lock())
		{
			label_parent = p->gameobject->name;
		}
		ImGui::InputText("##Item", &label_parent, ImGuiInputTextFlags_ReadOnly);

		if (ImGui::BeginDragDropTarget())
		{
			if (const auto& drag = Engine::editor->Get_Drag_Object())
			{
				hit_pos_transform = drag->transform;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::LeftText_InputText(u8"ガードエフェクトキー", "##guard_particle_key", &guard_particle_key, window_center);
		ImGui::LeftText_InputText(u8"パリィエフェクトキー", "##parry_particle_key", &parry_particle_key, window_center);
	}
	return true;
}