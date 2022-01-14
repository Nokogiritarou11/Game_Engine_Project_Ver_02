#include "Enemy_Normal_01_Damageable.h"
#include "Character_Hit_Stop_Manager.h"
#include "Editor.h"
#include "Engine.h"
#include "Enemy_Parameter.h"
#include "Enemy_Manager.h"
#include "Damage_Collision.h"
#include "Object_Pool.h"
#include "Player_Camera_Controller.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Normal_01_Damageable::Awake()
{
	//メンバポインタの取得
	animator = Get_Component<Animator>();
	parameter = Get_Component<Enemy_Parameter>();
	hit_stop_manager = Get_Component<Character_Hit_Stop_Manager>();

	const auto& manager = GameObject::Find_With_Tag("Game_Manager").lock();
	pool = manager->Get_Component<Object_Pool>();
	enemy_manager = manager->Get_Component<Enemy_Manager>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
}

bool Enemy_Normal_01_Damageable::Take_Damage(const shared_ptr<Damage_Collision>& damage_collision)
{
	const auto& anim = animator.lock();
	const auto& param = parameter.lock();

	//無敵状態の場合はリターン
	if (param->is_invincible) return false;

	//スーパーアーマー状態でない場合
	if (!param->is_super_armor)
	{
		//プレイヤーの方へ向き直る
		const Vector3 now_pos = transform->Get_Position();
		Vector3 look_pos = now_pos - damage_collision->root_transform.lock()->Get_Forward();
		look_pos.y = now_pos.y;
		transform->Set_Local_Rotation(transform->Look_At(look_pos));
	}

	//ヒットストップ
	hit_stop_manager.lock()->Start_Hit_Stop(damage_collision->hit_stop_time);
	//プレイヤーの最終攻撃対象を自身に設定する
	enemy_manager.lock()->last_attack_target = parameter;
	//ヒット時カメラシェイク
	camera_controller.lock()->Shake_Camera(damage_collision->shake_count, damage_collision->shake_power);
	//ダメージを受けてからのタイマーをリセットする
	param->last_damaged_timer = 0;

	if (param->guarding)
	{
		//ガード状態だった場合
		//Animatorのパラメータを変更しリターン
		anim->Set_Trigger("Damage");
		return true;
	}

	//ヒットエフェクトを再生する
	pool.lock()->Instance_In_Pool(damage_collision->hit_particle_key, hit_pos_transform.lock()->Get_Position(), damage_collision->hit_transform.lock()->Get_Rotation());
	//生存状態時のみ行う
	if (param->hp > 0)
	{
		//HPへのダメージ処理
		param->hp -= damage_collision->damage_hp;

		//スタンダメージ処理
		if (param->is_ground && !param->stunning)
		{
			param->stun -= damage_collision->damage_stun;
		}

		//ダメージによる致死時
		if (param->hp <= 0)
		{
			//以後のダメージ判定を無効化
			param->is_invincible = true;
			//Animatorに通知
			anim->Set_Trigger("Dead");
			anim->Set_Trigger("Damage");
			anim->Set_Int("Damage_State", static_cast<int>(damage_collision->damage_type));
			//致死エフェクトを再生する
			pool.lock()->Instance_In_Pool(critical_particle_key, hit_pos_transform.lock()->Get_Position(), damage_collision->hit_transform.lock()->Get_Rotation());
			//死亡時処理
			enemy_manager.lock()->Enemy_Dead(true, parameter);
			return true;
		}

		if (param->stun <= 0)
		{
			anim->Set_Trigger("Stun");
			enemy_manager.lock()->Enemy_Stunned();
			return true;
		}
	}

	//Animatorに通知
	anim->Set_Trigger("Damage");
	anim->Set_Int("Damage_State", static_cast<int>(damage_collision->damage_type));
	return true;
}

bool Enemy_Normal_01_Damageable::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Normal_01_Damageable", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

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

		ImGui::LeftText_InputText(u8"致死エフェクトキー", "##critical_particle_key", &critical_particle_key, window_center);
	}
	return true;
}