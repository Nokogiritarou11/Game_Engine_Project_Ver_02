#include "Animator_Manager.h"
#include "GameObject.h"
#include "Animator.h"
#include "Engine.h"
#include  "Thread_Pool.h"
using namespace std;
using namespace BeastEngine;

void Animator_Manager::Reset()
{
	animator_list.clear();
}

void Animator_Manager::Update()
{
	//コンポーネントの生存をチェックし、別スレッドに更新を行わせる
	bool expired = false;
	for (const auto& animator : animator_list)
	{
		if (const auto& anim = animator.lock())
		{
			if (anim->gameobject->Get_Active_In_Hierarchy())
			{
				if (anim->Get_Enabled())
				{
					Engine::thread_pool->Add_Job([anim] {anim->Update(); });
				}
			}
		}
		else
		{
			expired = true;
		}
	}
	//コンポーネントが削除されていた場合、リストから削除
	if (expired)
	{
		const auto& remove_it = remove_if(animator_list.begin(), animator_list.end(), [](weak_ptr<Animator> p) { return p.expired(); });
		animator_list.erase(remove_it, animator_list.end());
	}

	//アニメーション計算スレッド終了待ち
	Engine::thread_pool->Wait_Job_Complete();
}

void Animator_Manager::Add(shared_ptr<Animator> animator)
{
	animator_list.emplace_back(animator);
}