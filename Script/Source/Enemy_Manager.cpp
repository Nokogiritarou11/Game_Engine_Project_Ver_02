#include "Enemy_Manager.h"
#include "Player_Parameter.h"
#include "Time_Manager.h"
#include "Player_Camera_Controller.h"
#include "Object_Pool.h"

using namespace std;
using namespace BeastEngine;

void Enemy_Manager::Awake()
{
	//メンバポインタの取得
	time_manager = Get_Component<Time_Manager>();
	camera_controller = GameObject::Find_With_Tag("main_camera").lock()->transform->Get_Parent().lock()->Get_Component<Player_Camera_Controller>();
	pool = GameObject::Find_With_Tag("Game_Manager").lock()->Get_Component<Object_Pool>();
	player_parameter = GameObject::Find_With_Tag("player").lock()->Get_Component<Player_Parameter>();
}

void Enemy_Manager::Update()
{
	//前回攻撃指示を出してからの経過時間を更新
	last_attacked_timer += Time::delta_time;

	//攻撃指示を行う場合
	if (last_attacked_timer >= attack_check_time && !enemy_list.empty())
	{
		//攻撃を受けていないエネミーの中でプレイヤーに対して一番近いものを検索する
		weak_ptr<Enemy_Parameter> nearest_enemy;
		const Vector3 player_pos = player_parameter.lock()->transform->Get_Position();
		float min_dis = FLT_MAX;
		for (const auto& enemy_weak : enemy_list)
		{
			if (const auto& enemy = enemy_weak.lock(); enemy->last_damaged_timer >= damaged_check_time && !enemy->stunning && !enemy->is_attack_preliminary)
			{
				const Vector3 pos = enemy->transform->Get_Position();
				if (const float dis = Vector3::DistanceSquared(pos, player_pos); dis < min_dis)
				{
					min_dis = dis;
					nearest_enemy = enemy_weak;
				}
			}
		}

		//対象の存在チェック
		if (const auto& enemy = nearest_enemy.lock())
		{
			//攻撃指示を出す
			enemy->is_attack_mode = true;
		}
		//タイマーのリセット
		last_attacked_timer = 0;
	}
}

void Enemy_Manager::Instance_Enemy(const Enemy_Type& type, const Vector3& position, const Quaternion& rotation)
{
	string key;
	//引数によって出現させるエネミーを変える
	switch (type)
	{
		case Enemy_Type::Enemy_Normal_01:
			key = "Enemy_Normal_01";
			break;
		case Enemy_Type::Enemy_Big_01:
			key = "Enemy_Big_01";
			break;
		default:
			return;
	}

	//オブジェクトプールからエネミーを呼び出し、リストに加える
	const auto& parameter = pool.lock()->Instance_In_Pool(key, position, rotation)->Get_Component<Enemy_Parameter>();
	enemy_list.emplace_back(parameter);

	//追加前にアクティブなエネミーが居なかった場合はロックオン対象に設定する
	if (last_attack_target.expired())
	{
		last_attack_target = parameter;
	}
}

void Enemy_Manager::Add_Attacking_List(const std::weak_ptr<Enemy_Parameter>& parameter)
{
	//攻撃中リストに追加する
	attacking_list.emplace_back(parameter);
}

void Enemy_Manager::Remove_Attacking_List(const std::weak_ptr<Enemy_Parameter>& parameter)
{
	//リストから削除対象を探して削除する
	const auto& param = parameter.lock();
	const size_t size = attacking_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == attacking_list[i].lock())
		{
			swap(attacking_list[i], attacking_list.back());
			attacking_list.pop_back();
			break;
		}
	}
}

void Enemy_Manager::Remove_Stunning_List(const std::weak_ptr<Enemy_Parameter>& parameter)
{
	//リストから削除対象を探して削除する
	const auto& param = parameter.lock();
	const size_t size = stunning_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == stunning_list[i].lock())
		{
			swap(stunning_list[i], stunning_list.back());
			stunning_list.pop_back();
			break;
		}
	}
}

void Enemy_Manager::Enemy_Dead(const bool& use_effect, const weak_ptr<Enemy_Parameter>& parameter)
{
	//死亡時演出を行う場合
	if (use_effect)
	{
		//時間停止とカメラシェイクを行う
		time_manager.lock()->Start_Time_Slow(effect_dead.time_parameter.delay, effect_dead.time_parameter.time, effect_dead.time_parameter.speed);
		camera_controller.lock()->Shake_Camera(effect_dead.shake_parameter.count, effect_dead.shake_parameter.power);
	}

	const auto& param = parameter.lock();
	//即死技の場合もあるのでHPを強制的に0にする
	param->hp = 0;

	//生存リストから削除する
	const size_t size = enemy_list.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (param == enemy_list[i].lock())
		{
			swap(enemy_list[i], enemy_list.back());
			enemy_list.pop_back();
			break;
		}
	}

	const auto& p_param = player_parameter.lock();

	//現在プレイヤーの攻撃対象だった場合
	if (last_attack_target.lock() == param)
	{
		if (enemy_list.empty())
		{
			//残りの生存エネミーが存在しなければ攻撃対象ポインタをリセットする
			last_attack_target.reset();

		}
		else
		{
			//まだエネミーが残っているのでプレイヤーに一番近いものを代わりに選択する
			float nearest_distance = FLT_MAX;
			for (const auto& enemy : enemy_list)
			{
				const Vector3 pos = enemy.lock()->transform->Get_Position();
				if (const float dis = Vector3::DistanceSquared(pos, p_param->transform->Get_Position()); dis < nearest_distance)
				{
					nearest_distance = dis;
					last_attack_target = enemy;
				}
			}
		}
	}

	//攻撃中だった場合はリストから削除する
	if (param->is_attack_preliminary) Remove_Attacking_List(parameter);
	//スタン中だった場合はリストから削除する
	if (param->stunning) Remove_Stunning_List(parameter);

	//現在プレイヤーのロックオン対象だった場合
	if (p_param->target.lock() == param)
	{
		//ロックオンを解除する
		p_param->target.reset();
	}
}

void Enemy_Manager::Enemy_Stunned(const bool& use_effect, const weak_ptr<Enemy_Parameter>& parameter)
{
	//スタン状態リストに追加
	stunning_list.emplace_back(parameter);

	//スタン時演出を行う場合
	if (use_effect)
	{
		//時間停止とカメラシェイクを行う
		time_manager.lock()->Start_Time_Slow(effect_stun.time_parameter.delay, effect_stun.time_parameter.time, effect_stun.time_parameter.speed);
		camera_controller.lock()->Shake_Camera(effect_stun.shake_parameter.count, effect_stun.shake_parameter.power);
	}
}

Vector3 Enemy_Manager::Get_Nearest_Enemy_Position(const Vector3& position)
{
	//引数の座標に一番近いエネミーを検索する
	Vector3 nearest_position;
	float min_dis = FLT_MAX;
	for (const auto& enemy : enemy_list)
	{
		const Vector3 pos = enemy.lock()->transform->Get_Position();
		if (const float dis = Vector3::DistanceSquared(pos, position); dis < min_dis)
		{
			min_dis = dis;
			nearest_position = pos;
		}
	}
	nearest_position.y = position.y;
	return nearest_position;
}

bool Enemy_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Enemy_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::LeftText_DragFloat(u8"攻撃判定時間", "##attack_check_time", &attack_check_time, window_center);
		ImGui::LeftText_DragFloat(u8"ダメージ復帰時間", "##damaged_check_time", &damaged_check_time, window_center);

		if (ImGui::TreeNode(u8"敵死亡時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"スロー効果"))
			{
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##effect_dead_time_parameter_delay", &effect_dead.time_parameter.delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##effect_dead_time_parameter_time", &effect_dead.time_parameter.time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##effect_dead_time_parameter_speed", &effect_dead.time_parameter.speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##effect_dead_shake_parameter_count", &effect_dead.shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##effect_dead_shake_parameter_power", &effect_dead.shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode(u8"敵スタン時演出パラメータ"))
		{
			if (ImGui::TreeNode(u8"スロー効果"))
			{
				ImGui::LeftText_DragFloat(u8"発生ディレイ", "##effect_stun_time_parameter_delay", &effect_stun.time_parameter.delay, window_center);
				ImGui::LeftText_DragFloat(u8"効果時間", "##effect_stun_time_parameter_time", &effect_stun.time_parameter.time, window_center);
				ImGui::LeftText_DragFloat(u8"速度", "##effect_stun_time_parameter_speed", &effect_stun.time_parameter.speed, window_center);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode(u8"カメラ揺れ"))
			{
				ImGui::LeftText_DragInt(u8"回数", "##effect_stun_shake_parameter_count", &effect_stun.shake_parameter.count, window_center);
				ImGui::LeftText_DragFloat(u8"強度", "##effect_stun_shake_parameter_power", &effect_stun.shake_parameter.power, window_center);
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::Button(u8"敵_通常_01 生成"))
		{
			Instance_Enemy(Enemy_Type::Enemy_Normal_01, Vector3::Zero, Quaternion::Identity);
		}
	}
	return true;
}