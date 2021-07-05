#include "Animator.h"
#include "Engine.h"
#include "Animator_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animation_Clip.h"
#include "Animator_Controller.h"
#include "Time_Engine.h"
#include "Include_ImGui.h"
#include "System_Function.h"
using namespace std;
using namespace BeastEngine;

//指定文字での分割
vector<string> split(const string& s, char delim)
{
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim))
	{
		if (!item.empty())
		{
			elems.push_back(item);
		}
	}
	return elems;
}

void Animator::Initialize(shared_ptr<GameObject> obj)
{
	gameobject = obj;
	transform = obj->transform;
	Engine::animator_manager->Add(static_pointer_cast<Animator>(shared_from_this()));

	for each (string path in pathes)
	{
		Add_Clip(path);
	}
}

void Animator::Add_Clip(string path)
{
	shared_ptr<Animation_Clip> clip = Animation_Clip::Load_Clip(path);
	bool already = false;
	for (size_t i = 0; i < clips.size(); ++i)
	{
		if (clips[i].clip == clip) { already = true; break; }
	}
	if (!already)
	{
		Clip_Data data;
		data.clip = clip;

		for (size_t i = 0; i < clip->animations.size(); ++i)
		{
			Animation_Clip::Animation& anim = clip->animations[i];

			string t_path = anim.Target_Path;
			weak_ptr<Transform> t_trans = gameobject->transform;
			if (!t_path.empty())
			{
				vector<string> s = split(t_path, '/');
				for (size_t i = 0; i < s.size(); ++i)
				{
					t_trans = t_trans.lock()->Find(s[i]);
				}
			}

			Animation_Target target = { t_trans, anim };
			data.targets.push_back(target);
		}

		bool loaded = false;
		for (size_t i = 0; i < pathes.size(); ++i)
		{
			if (pathes[i] == path) { loaded = true; break; }
		}
		if (!loaded)
		{
			pathes.push_back(path);
		}
		clips.push_back(data);
	}
}

void Animator::Update()
{
	if (clips.empty())            return;
	if (!playing)                 return;
	if (currentAnimation < 0)     return;

	const shared_ptr<Animation_Clip> clip = clips[currentAnimation].clip;
	for (size_t i = 0; i < clips[currentAnimation].targets.size(); ++i)
	{
		const Animation_Target target = clips[currentAnimation].targets[i];
		const std::vector<Animation_Clip::Keyframe>& keyframes = target.animation.keys;

		int keyCount = static_cast<int>(keyframes.size());
		for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
		{
			// 現在の時間がどのキーフレームの間にいるか判定する
			const Animation_Clip::Keyframe& keyframe0 = keyframes.at(keyIndex);
			const Animation_Clip::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
			if (currentSeconds >= keyframe0.time && currentSeconds < keyframe1.time)
			{
				float rate = (currentSeconds - keyframe0.time / keyframe1.time - keyframe0.time);

				// ２つのキーフレーム間の補完計算
				shared_ptr<Transform> bone = target.target_transform.lock();

				bone->Set_Local_Scale(DirectX::XMVectorLerp(keyframe0.scale, keyframe1.scale, rate));
				bone->Set_Local_Rotation(DirectX::XMQuaternionSlerp(keyframe0.rotation, keyframe1.rotation, rate));
				bone->Set_Local_Position(DirectX::XMVectorLerp(keyframe0.position, keyframe1.position, rate));
				break;
			}
		}
	}

	// 最終フレーム処理
	if (endAnimation)
	{
		endAnimation = false;
		currentSeconds = 0;
		currentAnimation = -1;
		return;
	}

	// 時間経過
	currentSeconds += Time::delta_time * animation_speed;
	float length = clip->Get_Length();
	if (currentSeconds >= length)
	{
		if (loopAnimation)
		{
			currentSeconds -= length;
		}
		else
		{
			currentSeconds = length;
			endAnimation = true;
		}
	}
}

// アニメーション再生
void Animator::Play(int clipIndex)
{
	currentAnimation = clipIndex;
	endAnimation = false;
	playing = true;
}

void Animator::Stop()
{
	currentAnimation = -1;
	currentSeconds = 0;
	playing = false;
}

void Animator::Pause()
{
	playing = false;
}

bool Animator::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Animator_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Animator>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
		if (!removed)
		{
			return false;
		}
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	static bool enable;
	enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		if (!clips.empty())
		{
			static int item_current_idx = 0;                    // Here our selection data is an index.
			if (ImGui::BeginCombo("Select_Animation", clips[item_current_idx].clip->name.data()))
			{
				for (unsigned int n = 0; n < clips.size(); n++)
				{
					const bool is_selected = (item_current_idx == n);
					if (ImGui::Selectable(clips[n].clip->name.data(), is_selected))
						item_current_idx = n;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::DragFloat(u8"再生速度", &animation_speed, 0.01f, 0, FLT_MAX);
			ImGui::Checkbox(u8"ループ", &loopAnimation);

			if (ImGui::Button(ICON_FA_PLAY))
			{
				Play(item_current_idx);
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP))
			{
				Stop();
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_PAUSE))
			{
				Pause();
			}
		}
		else
		{
			ImGui::Text(u8"アニメーションデータが登録されていません");
		}

		if (ImGui::Button(u8"アニメーションの追加"))
		{
			string path = System_Function::Get_Open_File_Name("anim");
			if (path != "")
			{
				Add_Clip(path);
			}
		}
	}
	return true;
}