#include "Animator.h"
#include "Engine.h"
#include "Animator_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Time_Engine.h"
#include "Include_ImGui.h"
using namespace std;

void Animator::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	Engine::animator_manager->Add(static_pointer_cast<Animator>(shared_from_this()));

	shared_ptr<SkinMesh_Renderer> skin = GetComponent<SkinMesh_Renderer>();
	if (skin)
	{
		Set_Skin_Renderer(skin);
	}
}

void Animator::Set_Skin_Renderer(std::shared_ptr<SkinMesh_Renderer> render)
{
	skin_renderer = render;
	if (render->mesh_data)
	{
		Set_Mesh(render->mesh_data);
	}
}
void Animator::Set_Mesh(std::shared_ptr<Mesh> mesh)
{
	mesh_data = mesh;
}

void Animator::Update()
{
	if (!mesh_data)	                   return;
	if (!Playing)                      return;
	if (currentAnimation < 0)          return;
	if (mesh_data->animations.empty()) return;

	const Mesh::Animation& animation = mesh_data->animations.at(currentAnimation);
	const std::vector<Mesh::Keyframe>& keyframes = animation.keyframes;

	shared_ptr<SkinMesh_Renderer> skin = skin_renderer.lock();
	vector<weak_ptr<Transform>>& nodes = skin->bones;

	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// 現在の時間がどのキーフレームの間にいるか判定する
		const Mesh::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const Mesh::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentSeconds >= keyframe0.seconds && currentSeconds < keyframe1.seconds)
		{
			float rate = (currentSeconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

			assert(nodes.size() == keyframe0.nodeKeys.size());
			assert(nodes.size() == keyframe1.nodeKeys.size());
			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// ２つのキーフレーム間の補完計算
				const Mesh::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const Mesh::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				shared_ptr<Transform> bone = nodes[nodeIndex].lock();

				bone->Set_localScale(DirectX::XMVectorLerp(key0.scale, key1.scale, rate));
				bone->Set_localRotation(DirectX::XMQuaternionSlerp(key0.rotation, key1.rotation, rate));
				bone->Set_localPosition(DirectX::XMVectorLerp(key0.position, key1.position, rate));
			}
			break;
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
	currentSeconds += Time::deltaTime * animation_speed;
	if (currentSeconds >= animation.secondsLength)
	{
		if (loopAnimation)
		{
			currentSeconds -= animation.secondsLength;
		}
		else
		{
			currentSeconds = animation.secondsLength;
			endAnimation = true;
		}
	}
}

// アニメーション再生
void Animator::Play(int animationIndex)
{
	currentAnimation = animationIndex;
	endAnimation = false;
	Playing = true;
}

void Animator::Stop()
{
	currentAnimation = -1;
	currentSeconds = 0;
	Playing = false;
}

void Animator::Pause()
{
	Playing = false;
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
	enable = enableSelf();
	if (ImGui::Checkbox("##enable", &enable))
	{
		SetEnabled(enable);
	}

	if (open)
	{
		if (skin_renderer.expired())
		{
			mesh_data = nullptr;
		}

		if (!mesh_data)
		{
			if (ImGui::Button(u8"Meshを検索"))
			{
				shared_ptr<SkinMesh_Renderer> skin = GetComponent<SkinMesh_Renderer>();
				if (skin)
				{
					Set_Skin_Renderer(skin);
					shared_ptr<SkinMesh_Renderer> skin = skin_renderer.lock();
					if (skin->mesh_data)
					{
						mesh_data = skin->mesh_data;
					}
				}
			}
		}
		else
		{
			if (!mesh_data->animations.empty())
			{
				static int item_current_idx = 0;                    // Here our selection data is an index.
				if (ImGui::BeginCombo("Select_Animation", mesh_data->animations[item_current_idx].name.data()))
				{
					for (unsigned int n = 0; n < mesh_data->animations.size(); n++)
					{
						const bool is_selected = (item_current_idx == n);
						if (ImGui::Selectable(mesh_data->animations[n].name.data(), is_selected))
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
				ImGui::Text(u8"メッシュにアニメーションデータがありません");
			}
		}
	}
	return true;
}