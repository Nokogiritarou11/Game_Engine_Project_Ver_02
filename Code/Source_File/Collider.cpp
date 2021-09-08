#include "Collider.h"
#include "GameObject.h"
#include "MonoBehaviour.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"

using namespace BeastEngine;
using namespace std;

void Collider::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Collider::Get_Enabled()
{
	return enabled;
}

void Collider::Set_Active(bool value)
{
	if (value)
	{
		if (gameobject->Get_Active_In_Hierarchy())
		{
			if (Get_Enabled())
			{
				if (!is_called)
				{
					Initialize_MonoBehaviour();
					is_called = true;
				}
				disabled = false;
			}
			else
			{
				disabled = true;
			}
		}
		else
		{
			disabled = true;
		}
	}
	else
	{
		disabled = true;
	}

	if (disabled != disabled_old)
	{
		disabled = disabled_old;
		if (disabled)
		{
			if (is_trigger) ghost->ghost.reset();
			else rigidbody->rigidbody.reset();
			shape.reset();
		}
		else
		{
			Create_Shape();
			Create_Collider();
			Rescale_Shape();
		}
	}
}

void Collider::Create_Collider()
{
	if (is_trigger)
	{
		rigidbody.reset();

		if (!ghost)
		{
			ghost = make_unique<GhostObject>();
		}
		ghost->Initialize(static_pointer_cast<Collider>(shared_from_this()));
	}
	else
	{
		ghost.reset();

		if (!rigidbody)
		{
			rigidbody = make_unique<RigidBody>();
		}
		rigidbody->Initialize(static_pointer_cast<Collider>(shared_from_this()));
	}

	//デバッグ表示可否
	Set_Debug_Draw(debug_drawed);
}

void Collider::Reset_Collider()
{
	if (is_trigger)
	{
		debug_drawed = ghost->Get_Debug_Drawed();
		ghost->Remove();
		ghost->ghost.reset();
	}
	else
	{
		debug_drawed = rigidbody->Get_Debug_Drawed();
		rigidbody->Remove();
		rigidbody->rigidbody.reset();
	}
}

void Collider::Set_IsTrigger(bool value)
{
	if (is_trigger != value)
	{
		is_trigger = value;
		Create_Collider();
	}
}

void Collider::Initialize(shared_ptr<GameObject> obj)
{
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;

	Create_Shape();
	Create_Collider();
	Rescale_Shape();

	Set_Active(gameobject->Get_Active_In_Hierarchy());
}

void Collider::Initialize_MonoBehaviour()
{
	for (auto& com : gameobject->component_list)
	{
		shared_ptr<MonoBehaviour>& buff = dynamic_pointer_cast<MonoBehaviour>(com);
		if (buff != nullptr)
		{
			send_list.emplace_back(buff);
		}
	}
}

void Collider::Rescale_Shape()
{
	Vector3 scale = transform->Get_Scale();
	btVector3 bt_scale(scale.x, scale.y, scale.z);
	if (shape->getLocalScaling() != bt_scale)
	{
		shape->setLocalScaling(bt_scale);
		if (is_trigger) ghost->Resize();
		else rigidbody->Resize();
	}
}

void Collider::Call_Hit(Collision& collision)
{
	string& id = collision.collider->Get_Instance_ID();
	auto it = hit_list_old.find(id);
	if (it == hit_list_old.end())
	{
		hit_list[id] = collision.collider;
		if (is_trigger)Call_OnTrigger_Enter(collision);
		else Call_OnCollision_Enter(collision);
	}
	else
	{
		hit_list[id] = collision.collider;
		if (is_trigger) Call_OnTrigger_Stay(collision);
		else Call_OnCollision_Stay(collision);
	}
}

void Collider::Update_Transform()
{
	Vector3 pos = transform->Get_Position() + (transform->Get_Right() * center.x) + (transform->Get_Up() * center.y) + (transform->Get_Forward() * center.z);
	Quaternion rot = transform->Get_Rotation();

	Rescale_Shape();

	if (position_old != pos || rotation_old != rot)
	{
		position_old = pos;
		rotation_old = rot;

		btTransform t(btQuaternion(rot.x, rot.y, rot.z, rot.w), btVector3(pos.x, pos.y, pos.z));
		if (is_trigger)	ghost->Set_btTransform(t);
		else rigidbody->Set_btTransform(t);
	}
}

void Collider::Update_Simulation()
{
	btTransform t;
	if (is_trigger) ghost->Get_btTransform(t);
	else rigidbody->Get_btTransform(t);

	btVector3 v = t.getOrigin();
	btQuaternion q = t.getRotation();

	Vector3 pos = { v.x(), v.y(), v.z() };
	Quaternion rot = { q.x(), q.y(), q.z(), q.w() };
	pos -= ((transform->Get_Right() * center.x) + (transform->Get_Up() * center.y) + (transform->Get_Forward() * center.z));

	if (position_old != pos || rotation_old != rot)
	{
		position_old = pos;
		rotation_old = rot;
		transform->Set_Position(pos);
		transform->Set_Rotation(rot);
	}

	shared_ptr<Collider> col;
	for (auto& hit : hit_list_old)
	{
		auto it = hit_list.find(hit.first);
		if (it == hit_list.end())
		{
			col = hit.second.lock();
			Collision collision = { col, col->gameobject, col->transform };
			if (is_trigger) Call_OnTrigger_Exit(collision);
			else Call_OnCollision_Exit(collision);
		}
	}
	hit_list_old = hit_list;
	hit_list.clear();
}

void Collider::Call_OnTrigger_Enter(Collision& collision)
{
	for (auto& m : send_list)
	{
		if (shared_ptr<MonoBehaviour>& mono = m.lock())
		{
			mono->OnTrigger_Enter(collision);
		}
	}
}
void Collider::Call_OnTrigger_Stay(Collision& collision)
{
	for (auto& m : send_list)
	{
		if (shared_ptr<MonoBehaviour>& mono = m.lock())
		{
			mono->OnTrigger_Stay(collision);
		}
	}
}
void Collider::Call_OnTrigger_Exit(Collision& collision)
{
	for (auto& m : send_list)
	{
		if (shared_ptr<MonoBehaviour>& mono = m.lock())
		{
			mono->OnTrigger_Exit(collision);
		}
	}
}

void Collider::Call_OnCollision_Enter(Collision& collision)
{
	for (auto& m : send_list)
	{
		if (shared_ptr<MonoBehaviour>& mono = m.lock())
		{
			mono->OnCollision_Enter(collision);
		}
	}
}
void Collider::Call_OnCollision_Stay(Collision& collision)
{
	for (auto& m : send_list)
	{
		if (shared_ptr<MonoBehaviour>& mono = m.lock())
		{
			mono->OnCollision_Stay(collision);
		}
	}
}
void Collider::Call_OnCollision_Exit(Collision& collision)
{
	for (auto& m : send_list)
	{
		if (shared_ptr<MonoBehaviour>& mono = m.lock())
		{
			mono->OnCollision_Exit(collision);
		}
	}
}

void Collider::Set_Debug_Draw(bool value)
{
	if (is_trigger) ghost->Set_Debug_Draw(value);
	else rigidbody->Set_Debug_Draw(value);
}

bool Collider::Draw_ImGui_Header(string component_name, bool& open)
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	open = ImGui::CollapsingHeader(component_name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	string sub = component_name + "_sub";
	if (ImGui::BeginPopupContextItem(component_name.c_str()))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Component>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}

	if (removed) return false;

	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	return true;
}

void Collider::Draw_ImGui_Common()
{
	float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

	ImGui::Text(u8"中心オフセット");
	ImGui::SameLine(window_center);
	ImGui::SetNextItemWidth(window_center);
	float center_im[3] = { center.x, center.y, center.z };
	if (ImGui::DragFloat3("##Center", center_im, 0.1f))
	{
		center = { center_im[0], center_im[1], center_im[2] };
	}

	ImGui::Separator();

	ImGui::Text(u8"トリガーコライダー");
	ImGui::SameLine(window_center);
	bool trigger = is_trigger;
	if (ImGui::Checkbox("##Trigger", &trigger))
	{
		Set_IsTrigger(trigger);
	}

	if (!is_trigger)
	{
		ImGui::Text(u8"ダイナミックコライダー");
		ImGui::SameLine(window_center);
		bool dynamic = rigidbody->is_dynamic;
		if (ImGui::Checkbox("##Dynamic", &dynamic))
		{
			rigidbody->Set_Dynamic(dynamic);
		}

		if (rigidbody->is_dynamic)
		{
			ImGui::Text(u8"質量");
			ImGui::SameLine(window_center);
			ImGui::SetNextItemWidth(window_center);
			float mass = rigidbody->mass;
			if (ImGui::InputFloat("##Mass", &mass, 0, 0, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				rigidbody->Set_Mass(mass);
			}

			ImGui::Text(u8"重力");
			ImGui::SameLine(window_center);
			bool gravity = (rigidbody->gravity == Vector3(0.0f, -9.8f, 0.0f));
			if (ImGui::Checkbox("##Gravity", &gravity))
			{
				rigidbody->Use_Gravity(gravity);
			}
		}
		else
		{
			ImGui::Text(u8"キネマティック");
			ImGui::SameLine(window_center);
			bool kinematic = rigidbody->is_kinematic;
			if (ImGui::Checkbox("##Kinematic", &kinematic))
			{
				rigidbody->Set_Kinematic(kinematic);
			}
		}

		ImGui::Text(u8"移動制限");
		ImGui::SameLine(window_center);
		bool pos_x = !static_cast<bool>(rigidbody->linear_factor.x);
		bool pos_y = !static_cast<bool>(rigidbody->linear_factor.y);
		bool pos_z = !static_cast<bool>(rigidbody->linear_factor.z);
		bool changed_pos = false;
		if (ImGui::Checkbox("X##Pos", &pos_x)) changed_pos = true;
		ImGui::SameLine();
		if (ImGui::Checkbox("Y##Pos", &pos_y)) changed_pos = true;
		ImGui::SameLine();
		if (ImGui::Checkbox("Z##Pos", &pos_z)) changed_pos = true;

		if (changed_pos) rigidbody->Set_Freeze_Position(pos_x, pos_y, pos_z);

		ImGui::Text(u8"回転制限");
		ImGui::SameLine(window_center);
		bool rot_x = !static_cast<bool>(rigidbody->angular_factor.x);
		bool rot_y = !static_cast<bool>(rigidbody->angular_factor.y);
		bool rot_z = !static_cast<bool>(rigidbody->angular_factor.z);
		bool changed_rot = false;
		if (ImGui::Checkbox("X##Rot", &rot_x)) changed_rot = true;
		ImGui::SameLine();
		if (ImGui::Checkbox("Y##Rot", &rot_y)) changed_rot = true;
		ImGui::SameLine();
		if (ImGui::Checkbox("Z##Rot", &rot_z)) changed_rot = true;

		if (changed_rot) rigidbody->Set_Freeze_Rotation(rot_x, rot_y, rot_z);
	}
}