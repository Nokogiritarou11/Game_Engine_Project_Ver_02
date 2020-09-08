#include "Animator.h"
#include "Animator_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Time.h"
#include "Include_ImGui.h"
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
	vector<SkinMesh_Renderer::Node>& nodes = skin->GetNodes();

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

				SkinMesh_Renderer::Node& node = nodes[nodeIndex];

				DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotation);
				DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotation);
				DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.position);
				DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.position);

				DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
				DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
				DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

				DirectX::XMStoreFloat3(&node.scale, s);
				DirectX::XMStoreFloat4(&node.rotation, r);
				DirectX::XMStoreFloat3(&node.position, t);
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
	if (ImGui::CollapsingHeader("Animator"))
	{
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