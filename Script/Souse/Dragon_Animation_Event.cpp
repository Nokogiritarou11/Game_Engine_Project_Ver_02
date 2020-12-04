#include "Dragon_Animation_Event.h"
#include "Object_Pool.h"
#include "Player.h"
using namespace std;

void Dragon_Animation_Event::Start()
{
	obj_pool = GameObject::Find("Object_Pool").lock()->GetComponent<Object_Pool>();
	animator = GetComponent<Animator>();
	player = GameObject::Find("Player").lock()->GetComponent<Player>();
	breath_round = GameObject::Find("Breath_Round").lock()->GetComponent<Particle>();
	se_breath_round = GameObject::Find("Breath_Round").lock()->GetComponent<AudioSource>();
	wave = GameObject::Find("Wave").lock()->GetComponent<Particle>();

}

void Dragon_Animation_Event::Update()
{
	shared_ptr<Animator> anim = animator.lock();
	static int playing_anim = -1;
	static int playing_anim_old = -1;
	static float playing_time = 0;
	static bool trigger = false;
	static float breath_timer = 0;
	static int breath_count = 0;
	playing_anim = anim->GetPlayingAnimation();
	playing_time = anim->GetPlayingSeconds();

	if (playing_anim_old != playing_anim)
	{
		trigger = false;

		if (playing_anim == 4)
		{
			breath_round.lock()->Play();
		}

		playing_anim_old = playing_anim;
	}

	if (!trigger)
	{
		//ひっかき
		if (playing_anim == 2)
		{
		}

		//単発ブレス
		if (playing_anim == 3)
		{
			if (playing_time >= 1.0f)
			{
				shared_ptr<GameObject> obj = obj_pool.lock()->Instance_Breath();
				obj->transform->Set_position(breath_round.lock()->transform->Get_position());
				obj->transform->Set_rotation(obj->transform->LookAt(player.lock()->transform->Get_position()));
				trigger = true;
			}
		}

		//放射ブレス
		if (playing_anim == 4)
		{
			if (playing_time >= 2.1f)
			{
				breath_count = 0;
				breath_timer = 0;
				se_breath_round.lock()->Play();
				trigger = true;
			}
		}

		//叩きつけ
		if (playing_anim == 5)
		{
			if (playing_time >= 2.1f && !player.lock()->jump_flg)
			{
				player.lock()->Damage(10);
			}
			if (playing_time >= 2.1f)
			{
				wave.lock()->Play();
				trigger = true;
			}
		}
	}
	else
	{
		//放射ブレス
		if (playing_anim == 4)
		{
			breath_timer += Time::deltaTime;
			if (breath_count < 10 && breath_timer >= 0.2f)
			{
				shared_ptr<GameObject> obj = obj_pool.lock()->Instance_Breath_Round();
				obj->transform->Set_position(breath_round.lock()->transform->Get_position());
				Vector3 ang = breath_round.lock()->transform->Get_eulerAngles();
				obj->transform->Set_eulerAngles(0, ang.y, ang.z);
				breath_timer = 0;
				++breath_count;
			}
		}
	}
}

bool Dragon_Animation_Event::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Dragon_Animation_Event");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Dragon_Animation_Event_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Dragon_Animation_Event>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	if (open)
	{
	}

	return true;
}