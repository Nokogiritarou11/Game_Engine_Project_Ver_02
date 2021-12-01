#include "Character_Hit_Stop_Manager.h"
#include "Character_Parameter.h"

using namespace std;
using namespace BeastEngine;

void Character_Hit_Stop_Manager::Awake()
{
	//メンバポインタの取得
	parameter = Get_Component<Character_Parameter>();
	animator = Get_Component<Animator>();
}

void Character_Hit_Stop_Manager::Update()
{
	//ヒットストップが有効か
	if (is_stop)
	{
		//タイマーの更新
		stop_timer -= Time::delta_time;
		//タイマーの終了時
		if (stop_timer < 0.0f)
		{
			//パーティクルを再開
			for (auto& particle : stop_particles)
			{
				particle.lock()->Play();
			}

			//アニメーションの速度を戻す
			animator.lock()->Set_Float("Hit_Stop_Speed", 1);
			parameter.lock()->pausing = false;

			//メンバをリセット
			stop_particles.clear();
			stop_timer = 0;
			is_stop = false;
		}
	}
}

void Character_Hit_Stop_Manager::Start_Hit_Stop(const float stop_time, const vector<weak_ptr<Particle>>& particles)
{
	//メンバへの代入
	stop_timer = stop_time;
	stop_particles = particles;
	//ヒットストップ対象のパーティクルを一時停止する
	for (auto& particle : stop_particles)
	{
		particle.lock()->Pause();
	}

	//アニメーションの速度を0にすることでヒットストップを行う
	animator.lock()->Set_Float("Hit_Stop_Speed", 0);
	parameter.lock()->pausing = true;

	is_stop = true;
}

void Character_Hit_Stop_Manager::Start_Hit_Stop(const float stop_time)
{
	//メンバへの代入
	stop_timer = stop_time;
	//ヒットストップ対象のパーティクルを一時停止する
	for (auto& particle : stop_particles)
	{
		particle.lock()->Pause();
	}

	//アニメーションの速度を0にすることでヒットストップを行う
	animator.lock()->Set_Float("Hit_Stop_Speed", 0);
	parameter.lock()->pausing = true;

	is_stop = true;
}

bool Character_Hit_Stop_Manager::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Character_Hit_Stop_Manager", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;
		ImGui::Text(u8"設定できるパラメータはありません");
	}
	return true;
}