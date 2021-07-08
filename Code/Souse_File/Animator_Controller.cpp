#include "Animator_Controller.h"
#include "Animator_State_Machine.h"
#include "Transform.h"
using namespace std;
using namespace BeastEngine;

void Animator_Controller::Initialize(shared_ptr<Transform> root)
{
	for (size_t i = 0; i < state_machines.size(); ++i)
	{
		state_machines[i]->Initialize(root);
	}
}
void Animator_Controller::Update()
{
	playing_state_machine->Update(parameters);

	for (size_t i = 0; i < playing_state_machine->clip_data.targets.size(); ++i)
	{
		const Animator_State_Machine::Animation_Target target = playing_state_machine->clip_data.targets[i];
		const vector<Animation_Clip::Keyframe>& keyframes = target.animation.keys;

		int keyCount = static_cast<int>(keyframes.size());
		for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
			if (playing_state_machine->currentSeconds >= keyframe0.time && playing_state_machine->currentSeconds < keyframe1.time)
			{
				float rate = (playing_state_machine->currentSeconds - keyframe0.time / keyframe1.time - keyframe0.time);

				// ２つのキーフレーム間の補完計算
				shared_ptr<Transform> bone = target.target_transform.lock();

				bone->Set_Local_Scale(DirectX::XMVectorLerp(keyframe0.scale, keyframe1.scale, rate));
				bone->Set_Local_Rotation(DirectX::XMQuaternionSlerp(keyframe0.rotation, keyframe1.rotation, rate));
				bone->Set_Local_Position(DirectX::XMVectorLerp(keyframe0.position, keyframe1.position, rate));
				break;
			}
		}
	}
}