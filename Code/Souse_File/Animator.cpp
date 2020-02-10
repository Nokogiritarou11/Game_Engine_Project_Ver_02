#include "Animator.h"
#include "Animator_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Time.h"
using namespace std;

Animator::Animator()
{
}

Animator::~Animator()
{
}

void Animator::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	Animator_Manager::Add(static_pointer_cast<Animator>(shared_from_this()));
}

void Animator::Set_Skin_Renderer(std::shared_ptr<SkinMesh_Renderer> render)
{
	skin_renderer = render;
}

void Animator::Update()
{
	shared_ptr<SkinMesh_Renderer> skin = skin_renderer.lock();
	Animation_Time = skin->Animation_Time + Time::deltaTime * Animation_Speed;
	Animation_End = skin->Animation_End;
	if (Animation_Loop) Animation_End = false;
	Animation_Rate = skin->Animation_Rate;

	if (Animation_Index_Old != Animation_Index)
	{
		Animation_Time = 0;
		Animation_End = false;
		Animation_Index_Old = Animation_Index;
	}
	skin->Animation_Index = Animation_Index;
	skin->Animation_Time = Animation_Time;
	skin->Animation_Loop = Animation_Loop;
	skin->Animation_End = Animation_End;
}