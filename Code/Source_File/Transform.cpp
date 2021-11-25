#include "Transform.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Asset_Manager.h"
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

Transform::Transform()
{
	if (const auto& p = parent.lock())
	{
		local_translation_matrix = Matrix::CreateTranslation(local_position);
		local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
		local_scale_matrix = Matrix::CreateScale(local_scale);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		world_matrix = local_matrix * p->Get_World_Matrix();
		world_matrix.Decompose(scale, rotation, position);

		scale_matrix = Matrix::CreateScale(scale);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		translation_matrix = Matrix::CreateTranslation(position);
	}
	else
	{
		translation_matrix = Matrix::CreateTranslation(position);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		local_position = position;
		local_rotation = rotation;
		local_scale = scale;

		local_translation_matrix = translation_matrix;
		local_rotation_matrix = rotation_matrix;
		local_scale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	left = Vector3::Transform(Vector3::Right, rotation_matrix);
	left.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	has_changed = false;
}

Transform::Transform(const Vector3 position, const Quaternion rotation)
{
	this->position = position;
	this->rotation = rotation;
	scale = Vector3(1, 1, 1);

	if (const auto& p = parent.lock())
	{
		local_translation_matrix = Matrix::CreateTranslation(local_position);
		local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
		local_scale_matrix = Matrix::CreateScale(local_scale);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		world_matrix = local_matrix * p->Get_World_Matrix();
		world_matrix.Decompose(scale, this->rotation, this->position);

		scale_matrix = Matrix::CreateScale(scale);
		rotation_matrix = Matrix::CreateFromQuaternion(this->rotation);
		translation_matrix = Matrix::CreateTranslation(this->position);
	}
	else
	{
		translation_matrix = Matrix::CreateTranslation(this->position);
		rotation_matrix = Matrix::CreateFromQuaternion(this->rotation);
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		local_position = this->position;
		local_rotation = this->rotation;
		local_scale = scale;

		local_translation_matrix = translation_matrix;
		local_rotation_matrix = rotation_matrix;
		local_scale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	left = Vector3::Transform(Vector3::Right, rotation_matrix);
	left.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();


	has_changed = false;
}

Transform::Transform(const Vector3 position, const Vector3 euler)
{
	this->position = position;
	rotation = Quaternion::Euler(euler);
	scale = Vector3(1, 1, 1);

	if (const auto& p = parent.lock())
	{
		local_translation_matrix = Matrix::CreateTranslation(local_position);
		local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
		local_scale_matrix = Matrix::CreateScale(local_scale);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		world_matrix = local_matrix * p->Get_World_Matrix();
		world_matrix.Decompose(scale, rotation, this->position);

		scale_matrix = Matrix::CreateScale(scale);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		translation_matrix = Matrix::CreateTranslation(this->position);
	}
	else
	{
		translation_matrix = Matrix::CreateTranslation(this->position);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		local_position = this->position;
		local_rotation = rotation;
		local_scale = scale;

		local_translation_matrix = translation_matrix;
		local_rotation_matrix = rotation_matrix;
		local_scale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	left = Vector3::Transform(Vector3::Right, rotation_matrix);
	left.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();


	has_changed = false;
}

void Transform::Initialize(const shared_ptr<GameObject>& obj)
{
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	obj->transform = static_pointer_cast<Transform>(shared_from_this());
	transform = static_pointer_cast<Transform>(shared_from_this());
}

bool Transform::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Transform"))
	{
		static weak_ptr<Transform> active_trans;

		static float pos[3] = { 0,0,0 };
		static float rot[3] = { 0,0,0 };
		static float scl[3] = { 1,1,1 };

		if (update_GUI || active_trans.lock() != gameobject->transform)
		{
			active_trans = gameobject->transform;

			const Vector3 trans_pos = Get_Local_Position();
			const Vector3 trans_rot = Get_Local_Euler_Angles();
			const Vector3 trans_scl = Get_Local_Scale();

			pos[0] = trans_pos.x;
			pos[1] = trans_pos.y;
			pos[2] = trans_pos.z;

			rot[0] = trans_rot.x;
			rot[1] = trans_rot.y;
			rot[2] = trans_rot.z;

			scl[0] = trans_scl.x;
			scl[1] = trans_scl.y;
			scl[2] = trans_scl.z;

			update_GUI = false;
		}

		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.25f;

		ImGui::Text("Position");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragFloat3("##pos", pos, 0.05f, -FLT_MAX, FLT_MAX))
		{
			Set_Local_Position(pos[0], pos[1], pos[2]);
			update_GUI = false;
		}
		ImGui::Text("Rotation");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragFloat3("##rot", rot, 0.05f, -FLT_MAX, FLT_MAX))
		{
			Set_Local_Euler_Angles(rot[0], rot[1], rot[2]);
			update_GUI = false;
		}
		ImGui::Text("Scale");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragFloat3("##sca", scl, 0.01f, -FLT_MAX, FLT_MAX))
		{
			Set_Local_Scale(scl[0], scl[1], scl[2]);
			update_GUI = false;
		}
	}
	return true;
}

/*  ///////////////////////////////////////////////////////
	Getter & Setter
*/  ///////////////////////////////////////////////////////
Vector3 Transform::Get_Position() const
{
	return position;
}

void Transform::Set_Position(const Vector3 v)
{
	if (position != v)
	{
		position = v;
		translation_matrix = Matrix::CreateTranslation(position);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		if (const auto& p = parent.lock())
		{
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = world_matrix * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);
			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);
		}
		else
		{
			local_matrix = world_matrix;
			local_translation_matrix = translation_matrix;
			local_position = position;
		}

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}

void Transform::Set_Position(const float f1, const float f2, const float f3)
{
	const Vector3 V = { f1,f2,f3 };
	if (position != V)
	{
		position = V;
		translation_matrix = Matrix::CreateTranslation(position);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		if (const auto& p = parent.lock())
		{
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = world_matrix * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);
			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);
		}
		else
		{
			local_matrix = world_matrix;
			local_translation_matrix = translation_matrix;
			local_position = position;
		}

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}

Quaternion Transform::Get_Rotation() const
{
	return rotation;
}

void Transform::Set_Rotation(const Quaternion q)
{
	if (rotation != q)
	{
		rotation = q;
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		if (const auto& p = parent.lock())
		{
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = world_matrix * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);
			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);
		}
		else
		{
			local_rotation = rotation;
			local_rotation_matrix = rotation_matrix;
			local_matrix = world_matrix;
		}

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		left = Vector3::Transform(Vector3::Right, rotation_matrix);
		left.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}
void Transform::Set_Rotation(const float f1, const float f2, const float f3, const float f4)
{
	if (const Quaternion q = { f1,f2,f3,f4 }; rotation != q)
	{
		rotation = q;
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		if (const auto& p = parent.lock())
		{
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = world_matrix * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);
			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);
		}
		else
		{
			local_rotation = rotation;
			local_rotation_matrix = rotation_matrix;
			local_matrix = world_matrix;
		}

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		left = Vector3::Transform(Vector3::Right, rotation_matrix);
		left.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}

Vector3 Transform::Get_Scale() const
{
	return scale;
}

void Transform::Set_Scale(const Vector3 v)
{
	if (scale != v)
	{
		scale = v;
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		if (const auto& p = parent.lock())
		{
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = world_matrix * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);
			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);
		}
		else
		{
			local_scale = scale;
			local_scale_matrix = scale_matrix;
			local_matrix = world_matrix;
		}
		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}
void Transform::Set_Scale(const float f1, const float f2, const float f3)
{
	if (const Vector3 v = { f1,f2,f3 }; scale != v)
	{
		scale = v;
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		if (const auto& p = parent.lock())
		{
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = world_matrix * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);
			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);
		}
		else
		{
			local_scale = scale;
			local_scale_matrix = scale_matrix;
			local_matrix = world_matrix;
		}
		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}

Vector3 Transform::Get_Local_Position() const
{
	return local_position;
}

void Transform::Set_Local_Position(const Vector3 v)
{
	if (local_position != v)
	{
		local_position = v;
		local_translation_matrix = Matrix::CreateTranslation(local_position);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		if (const auto& p = parent.lock())
		{
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);
			translation_matrix = Matrix::CreateTranslation(position);
		}
		else
		{
			world_matrix = local_matrix;
			position = local_position;
			translation_matrix = local_translation_matrix;
		}

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}
void Transform::Set_Local_Position(const float f1, const float f2, const float f3)
{
	if (const Vector3 v = { f1,f2,f3 }; local_position != v)
	{
		local_position = v;
		local_translation_matrix = Matrix::CreateTranslation(local_position);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		if (const auto& p = parent.lock())
		{
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);
			translation_matrix = Matrix::CreateTranslation(position);
		}
		else
		{
			world_matrix = local_matrix;
			position = local_position;
			translation_matrix = local_translation_matrix;
		}

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}

Quaternion Transform::Get_Local_Rotation() const
{
	return local_rotation;
}

void Transform::Set_Local_Rotation(const Quaternion q)
{
	if (local_rotation != q)
	{
		local_rotation = q;
		local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		if (const auto& p = parent.lock())
		{
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);
			rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		}
		else
		{
			world_matrix = local_matrix;
			rotation = local_rotation;
			rotation_matrix = local_rotation_matrix;
		}

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();
		left = Vector3::Transform(Vector3::Right, rotation_matrix);
		left.Normalize();
		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}
void Transform::Set_Local_Rotation(const float f1, const float f2, const float f3, const float f4)
{
	if (const Quaternion q = { f1,f2,f3,f4 }; local_rotation != q)
	{
		local_rotation = q;
		local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		if (const auto& p = parent.lock())
		{
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);
			rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		}
		else
		{
			world_matrix = local_matrix;
			rotation = local_rotation;
			rotation_matrix = local_rotation_matrix;
		}

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		left = Vector3::Transform(Vector3::Right, rotation_matrix);
		left.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}

Vector3 Transform::Get_Local_Scale() const
{
	return local_scale;
}

void Transform::Set_Local_Scale(const Vector3 v)
{
	if (local_scale != v)
	{
		local_scale = v;
		local_scale_matrix = Matrix::CreateScale(local_scale);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		if (const auto& p = parent.lock())
		{
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);
			scale_matrix = Matrix::CreateScale(scale);
		}
		else
		{
			world_matrix = local_matrix;
			scale = local_scale;
			scale_matrix = local_scale_matrix;
		}

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}
void Transform::Set_Local_Scale(const float f1, const float f2, const float f3)
{
	if (const Vector3 v = { f1,f2,f3 }; local_scale != v)
	{
		local_scale = v;
		local_scale_matrix = Matrix::CreateScale(local_scale);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
		if (const auto& p = parent.lock())
		{
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);
			scale_matrix = Matrix::CreateScale(scale);
		}
		else
		{
			world_matrix = local_matrix;
			scale = local_scale;
			scale_matrix = local_scale_matrix;
		}

		Change_Children();
		has_changed = true;
		update_GUI = true;
	}
}

Vector3 Transform::Get_Forward() const
{
	return forward;
}

Vector3 Transform::Get_Left() const
{
	return left;
}

Vector3 Transform::Get_Up() const
{
	return up;
}

Vector3 Transform::Get_Euler_Angles() const
{
	return rotation.To_Euler();
}

void Transform::Set_Euler_Angles(const Vector3 v)
{
	Set_Rotation(Quaternion::Euler(v));
}
auto Transform::Set_Euler_Angles(const float f1, const float f2, const float f3) -> void
{
	Set_Rotation(Quaternion::Euler(f1, f2, f3));
}

Vector3 Transform::Get_Local_Euler_Angles() const
{
	return local_rotation.To_Euler();
}

void Transform::Set_Local_Euler_Angles(const Vector3 v)
{
	Set_Local_Rotation(Quaternion::Euler(v));
}
void Transform::Set_Local_Euler_Angles(const float f1, const float f2, const float f3)
{
	Set_Local_Rotation(Quaternion::Euler(f1, f2, f3));
}

weak_ptr<Transform> Transform::Get_Parent() const
{
	return parent;
}

void Transform::Set_Parent(const shared_ptr<Transform>& p)
{
	if (p != nullptr)
	{
		if (p != parent.lock())
		{
			if (!parent.expired())
			{
				Remove_Parent();
			}

			parent = p;
			p->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
			/*
			localScale = { scale.x / P->scale.x, scale.y / P->scale.y, scale.z / P->scale.z };
			localPosition = { (position.x - P->position.x) * P->localScale.x, (position.y - P->position.y) * P->localScale.y, (position.z - P->position.z) * P->localScale.z };
			Quaternion q;
			P->rotation.Inverse(q);
			localRotation = rotation * q;
			*/
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = Get_World_Matrix() * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);

			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);

			local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);

			scale_matrix = Matrix::CreateScale(scale);
			rotation_matrix = Matrix::CreateFromQuaternion(rotation);
			translation_matrix = Matrix::CreateTranslation(position);

			forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
			forward.Normalize();
			left = Vector3::Transform(Vector3::Right, rotation_matrix);
			left.Normalize();
			up = Vector3::Transform(Vector3::Up, rotation_matrix);
			up.Normalize();

			Change_Children();
		}
	}
	else
	{
		Remove_Parent();
	}
	has_changed = true;
	update_GUI = true;
}

void Transform::Set_Parent(const shared_ptr<Transform>& p, const int index_insert)
{
	if (p != nullptr)
	{
		if (p != parent.lock())
		{
			if (!parent.expired())
			{
				Remove_Parent();
			}

			parent = p;
			if (index_insert < 0)
			{
				p->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
			}
			else
			{
				p->children.insert(p->children.begin() + index_insert, static_pointer_cast<Transform>(shared_from_this()));
			}
			/*
			localScale = { scale.x / P->scale.x, scale.y / P->scale.y, scale.z / P->scale.z };
			localPosition = { (position.x - P->position.x) * P->localScale.x, (position.y - P->position.y) * P->localScale.y, (position.z - P->position.z) * P->localScale.z };
			Quaternion q;
			P->rotation.Inverse(q);
			localRotation = rotation * q;
			*/
			Matrix m;
			p->Get_World_Matrix().Invert(m);
			local_matrix = Get_World_Matrix() * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);

			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);

			local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
			world_matrix = local_matrix * p->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);

			scale_matrix = Matrix::CreateScale(scale);
			rotation_matrix = Matrix::CreateFromQuaternion(rotation);
			translation_matrix = Matrix::CreateTranslation(position);

			forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
			forward.Normalize();
			left = Vector3::Transform(Vector3::Right, rotation_matrix);
			left.Normalize();
			up = Vector3::Transform(Vector3::Up, rotation_matrix);
			up.Normalize();

			Change_Children();
		}
	}
	else
	{
		Remove_Parent();
	}
	has_changed = true;
	update_GUI = true;
}

void Transform::On_Parent_Changed()
{
	const auto& p = parent.lock();
	world_matrix = local_matrix * p->Get_World_Matrix();
	world_matrix.Decompose(scale, rotation, position);
	translation_matrix = Matrix::CreateTranslation(position);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	scale_matrix = Matrix::CreateScale(scale);
	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();
	left = Vector3::Transform(Vector3::Right, rotation_matrix);
	left.Normalize();
	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	update_GUI = true;
}

void Transform::Change_Children()
{
	for (weak_ptr<Transform>& child : children)
	{
		if (!child.expired())
		{
			const auto& c = child.lock();
			c->On_Parent_Changed();
		}
	}
}

void Transform::Remove_Parent()
{
	if (const auto& p = parent.lock())
	{
		auto it = p->children.begin();
		while (it != p->children.end())
		{
			if ((*it).lock() == static_pointer_cast<Transform>(shared_from_this()))
			{
				p->children.erase(it);
				break;
			}
			else ++it;
		}

		parent.reset();

		local_position = position;
		local_rotation = rotation;
		local_scale = scale;

		local_translation_matrix = translation_matrix;
		local_rotation_matrix = rotation_matrix;
		local_scale_matrix = scale_matrix;

		local_matrix = world_matrix;
	}
}

std::weak_ptr<Transform> Transform::Get_Child(const int index)
{
	return children[index];
}

int Transform::Get_Child_Count() const
{
	return static_cast<int>(children.size());
}

int Transform::Get_Sibling_Index() const
{
	if (const auto& p = parent.lock())
	{
		for (size_t i = 0; i < p->children.size(); ++i)
		{
			if (p->children[i].lock() == transform)
			{
				return i;
			}
		}
	}
	else
	{
		const auto& scene = Engine::scene_manager->Get_Active_Scene();
		int index_this = -1;

		for (size_t i = 0; i < scene->gameobject_list.size(); ++i)
		{
			if (scene->gameobject_list[i]->transform->Get_Parent().expired())
			{
				++index_this;
				if (scene->gameobject_list[i] == gameobject)
				{
					return index_this;
				}
			}
		}
	}
	return -1;
}

//Žw’è•¶Žš‚Å‚Ì•ªŠ„
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

weak_ptr<Transform> Transform::Find(const std::string n) const
{
	weak_ptr<Transform> t_trans;
	if (!n.empty())
	{
		t_trans = transform;
		const vector<string> s = split(n, '/');
		const size_t split_size = s.size();
		for (size_t i = 0; i < split_size; ++i)
		{
			const auto& trans = t_trans.lock();
			bool found = false;
			const size_t child_size = trans->children.size();
			for (size_t t = 0; t < child_size; ++t)
			{
				if (const auto& child = trans->children[t].lock(); child->gameobject->name == s[i])
				{
					found = true;
					t_trans = child;
					break;
				}
			}
			if (!found)
			{
				weak_ptr<Transform> t;
				return t;
			}
		}
	}

	return t_trans;
}

void Transform::Set_Sibling_Index(int index)
{
	if (Get_Sibling_Index() != index)
	{
		if (index < 0) index = 0;

		if (const auto& p = parent.lock())
		{
			shared_ptr<Transform> insert_at;
			if (index >= static_cast<int>(p->children.size()))
			{
				insert_at = p->children[static_cast<int>(p->children.size()) - 1].lock();
			}
			else
			{
				insert_at = p->children[index].lock();
			}

			for (int i = 0; i < static_cast<int>(p->children.size()); ++i)
			{
				if (const auto& child = p->children[i].lock(); child == transform)
				{
					p->children.erase(p->children.begin() + i);
					break;
				}
			}

			if (index <= static_cast<int>(p->children.size()))
			{
				bool is_last = true;
				for (int i = 0; i < static_cast<int>(p->children.size()); ++i)
				{
					if (p->children[i].lock() == insert_at)
					{
						is_last = false;
						p->children.insert(p->children.begin() + i, static_pointer_cast<Transform>(shared_from_this()));
						break;
					}
				}
				if (is_last)
				{
					p->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
				}
			}
			else
			{
				p->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
			}
		}
		else
		{
			const unique_ptr<Scene>& scene = Engine::scene_manager->Get_Active_Scene();

			shared_ptr<Transform> insert_at;
			int index_insert = -1;
			for (size_t i = 0; i < scene->gameobject_list.size(); ++i)
			{
				if (scene->gameobject_list[i]->transform->Get_Parent().expired())
				{
					++index_insert;
					if (index_insert == index)
					{
						insert_at = scene->gameobject_list[i]->transform;
						break;
					}
				}
			}

			for (size_t i = 0; i < scene->gameobject_list.size(); ++i)
			{
				if (scene->gameobject_list[i]->transform->Get_Parent().expired())
				{
					if (scene->gameobject_list[i] == gameobject)
					{
						scene->gameobject_list.erase(scene->gameobject_list.begin() + i);
						break;
					}
				}
			}

			if (index <= static_cast<int>(scene->gameobject_list.size()))
			{
				bool is_last = true;
				for (size_t i = 0; i < scene->gameobject_list.size(); ++i)
				{
					if (scene->gameobject_list[i]->transform->Get_Parent().expired())
					{
						if (scene->gameobject_list[i]->transform == insert_at)
						{
							is_last = false;
							scene->gameobject_list.insert(scene->gameobject_list.begin() + i, gameobject);
							break;
						}
					}
				}
				if (is_last)
				{
					scene->gameobject_list.emplace_back(gameobject);
				}
			}
			else
			{
				scene->gameobject_list.emplace_back(gameobject);
			}
		}
	}
}

Matrix Transform::Get_World_Matrix() const
{
	return world_matrix;
}

void Transform::Set_World_Matrix(const Matrix matrix)
{
	world_matrix = matrix;
	world_matrix.Decompose(scale, rotation, position);

	scale_matrix = Matrix::CreateScale(scale);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	translation_matrix = Matrix::CreateTranslation(position);

	if (const auto& p = parent.lock())
	{
		Matrix m;
		p->Get_World_Matrix().Invert(m);
		local_matrix = world_matrix * m;

		local_matrix.Decompose(local_scale, local_rotation, local_position);
		local_translation_matrix = Matrix::CreateTranslation(local_position);
		local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
		local_scale_matrix = Matrix::CreateScale(local_scale);
		local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
	}
	else
	{
		local_matrix = world_matrix;
		local_scale = scale;
		local_scale_matrix = scale_matrix;
		local_rotation = rotation;
		local_rotation_matrix = rotation_matrix;
		local_position = position;
		local_translation_matrix = translation_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	left = Vector3::Transform(Vector3::Right, rotation_matrix);
	left.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	has_changed = true;
	update_GUI = true;
}

/*  ///////////////////////////////////////////////////////
	Function
*/  ///////////////////////////////////////////////////////

Quaternion Transform::Look_At(const Vector3 pos) const
{
	constexpr Vector3 up = { 0,1,0 };
	const Vector3 z = XMVector3Normalize(pos - position);
	const Vector3 x = XMVector3Normalize(XMVector3Cross(up, z));
	const Vector3 y = XMVector3Normalize(XMVector3Cross(z, x));

	Matrix m = Matrix::Identity;
	m._11 = x.x; m._12 = y.x; m._13 = z.x;
	m._21 = x.y; m._22 = y.y; m._23 = z.y;
	m._31 = x.z; m._32 = y.z; m._33 = z.z;

	float elem[4] = { 0,0,0,0 };
	elem[0] = m._11 - m._22 - m._33 + 1.0f;
	elem[1] = -m._11 + m._22 - m._33 + 1.0f;
	elem[2] = -m._11 - m._22 + m._33 + 1.0f;
	elem[3] = m._11 + m._22 + m._33 + 1.0f;

	int biggestIdx = 0;
	for (int i = 0; i < 4; i++)
	{
		if (elem[i] > elem[biggestIdx])
		{
			biggestIdx = i;
		}
	}

	float q[4] = { 0,0,0,0 };
	const float v = sqrt(elem[biggestIdx]) * 0.5f;
	q[biggestIdx] = v;
	const float mult = 0.25f / v;

	switch (biggestIdx)
	{
		case 0:
			q[1] = (m._21 + m._12) * mult;
			q[2] = (m._13 + m._31) * mult;
			q[3] = (m._32 - m._23) * mult;
			break;
		case 1:
			q[0] = (m._21 + m._12) * mult;
			q[2] = (m._32 + m._23) * mult;
			q[3] = (m._13 - m._31) * mult;
			break;
		case 2:
			q[0] = (m._13 + m._31) * mult;
			q[1] = (m._32 + m._23) * mult;
			q[3] = (m._21 - m._12) * mult;
			break;
		case 3:
			q[0] = (m._32 - m._23) * mult;
			q[1] = (m._13 - m._31) * mult;
			q[2] = (m._21 - m._12) * mult;
			break;
		default:
			break;
	}
	const Quaternion rot = { q[0],q[1], q[2], q[3] };
	return rot;
}
