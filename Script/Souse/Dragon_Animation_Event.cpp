#include "Dragon_Animation_Event.h"
#include "Object_Pool.h"
using namespace std;

void Dragon_Animation_Event::Start()
{
	obj_pool = GameObject::Find("Object_Pool").lock()->GetComponent<Object_Pool>();
	animator = GetComponent<Animator>();
	player = GameObject::Find("Player").lock()->GetComponent<Transform>();
	breath_round = GameObject::Find("Breath_Round").lock()->GetComponent<Particle>();
}

void Dragon_Animation_Event::Update()
{
	shared_ptr<Animator> anim = animator.lock();
	static int playing_anim = -1;
	static int playing_anim_old = -1;
	static float playing_time = 0;
	static bool trigger = false;
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
				obj->transform->Set_rotation(obj->transform->LookAt(player.lock()->Get_position()));
				trigger = true;
			}
		}

		//放射ブレス
		if (playing_anim == 4)
		{
		}

		//叩きつけ
		if (playing_anim == 5)
		{
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