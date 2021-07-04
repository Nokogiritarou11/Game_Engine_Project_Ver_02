#include "Transform.h"
#include "Scene_Manager.h"
#include "Scene.h"
#include "Include_ImGui.h"
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

Transform::Transform()
{
	if (shared_ptr<Transform> p = parent.lock())
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

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	has_changed = false;
}

Transform::Transform(Vector3 _position, Quaternion _rotation)
{
	position = _position;
	rotation = _rotation;
	scale = { 1,1,1 };

	if (shared_ptr<Transform> p = parent.lock())
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

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();


	has_changed = false;
}

Transform::Transform(Vector3 _position, Vector3 _euler)
{
	position = _position;
	rotation = Quaternion::Euler(_euler);
	scale = { 1,1,1 };

	if (shared_ptr<Transform> p = parent.lock())
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

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();


	has_changed = false;
}

Transform::~Transform()
{
}

void Transform::Initialize(shared_ptr<GameObject> obj)
{
	gameobject = obj;
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

			Vector3 trans_pos = { 0,0,0 };
			Vector3 trans_rot = { 0,0,0 };
			Vector3 trans_scl = { 0,0,0 };

			trans_pos = Get_Local_Position();
			trans_rot = Get_Local_Euler_Angles();
			trans_scl = Get_Local_Scale();

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

		if (ImGui::DragFloat3(u8"Position", pos, 0.05f, -FLT_MAX, FLT_MAX))
		{
			Set_Local_Position(pos[0], pos[1], pos[2]);
			update_GUI = false;
		}
		if (ImGui::DragFloat3(u8"Rotation", rot, 0.05f, -FLT_MAX, FLT_MAX))
		{
			Set_Local_Euler_Angles(rot[0], rot[1], rot[2]);
			update_GUI = false;
		}
		if (ImGui::DragFloat3(u8"Scale", scl, 0.01f, -FLT_MAX, FLT_MAX))
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
	return this->position;
}

void Transform::Set_Position(Vector3 V)
{
	position = V;
	translation_matrix = Matrix::CreateTranslation(position);
	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

	if (shared_ptr<Transform> p = parent.lock())
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

void Transform::Set_Position(float f1, float f2, float f3)
{
	position = { f1,f2,f3 };
	translation_matrix = Matrix::CreateTranslation(position);
	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

	if (shared_ptr<Transform> p = parent.lock())
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

Quaternion Transform::Get_Rotation() const
{
	return this->rotation;
}

void Transform::Set_Rotation(Quaternion Q)
{
	rotation = Q;
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

	if (shared_ptr<Transform> p = parent.lock())
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

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	has_changed = true;
	update_GUI = true;
}
void Transform::Set_Rotation(float f1, float f2, float f3, float f4)
{
	rotation = { f1,f2,f3,f4 };
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

	if (shared_ptr<Transform> p = parent.lock())
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

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	has_changed = true;
	update_GUI = true;
}

Vector3 Transform::Get_Scale() const
{
	return this->scale;
}

void Transform::Set_Scale(Vector3 V)
{
	scale = V;
	scale_matrix = Matrix::CreateScale(scale);
	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

	if (shared_ptr<Transform> p = parent.lock())
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
void Transform::Set_Scale(float f1, float f2, float f3)
{
	scale = { f1,f2,f3 };
	scale_matrix = Matrix::CreateScale(scale);
	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

	if (shared_ptr<Transform> p = parent.lock())
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

Vector3 Transform::Get_Local_Position() const
{
	return this->local_position;
}

void Transform::Set_Local_Position(Vector3 V)
{
	local_position = V;
	local_translation_matrix = Matrix::CreateTranslation(local_position);
	local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
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
void Transform::Set_Local_Position(float f1, float f2, float f3)
{
	local_position = { f1,f2,f3 };
	local_translation_matrix = Matrix::CreateTranslation(local_position);
	local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
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

Quaternion Transform::Get_Local_Rotation() const
{
	return this->local_rotation;
}

void Transform::Set_Local_Rotation(Quaternion Q)
{
	local_rotation = Q;
	local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
	local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
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
	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();
	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	has_changed = true;
	update_GUI = true;
}
void Transform::Set_Local_Rotation(float f1, float f2, float f3, float f4)
{
	local_rotation = { f1,f2,f3,f4 };
	local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
	local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
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

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	has_changed = true;
	update_GUI = true;
}

Vector3 Transform::Get_Local_Scale() const
{
	return this->local_scale;
}

void Transform::Set_Local_Scale(Vector3 V)
{
	local_scale = V;
	local_scale_matrix = Matrix::CreateScale(local_scale);
	local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
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
void Transform::Set_Local_Scale(float f1, float f2, float f3)
{
	local_scale = { f1,f2,f3 };
	local_scale_matrix = Matrix::CreateScale(local_scale);
	local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
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

Vector3 Transform::Get_Forward() const
{
	return this->forward;
}

Vector3 Transform::Get_Right() const
{
	return this->right;
}

Vector3 Transform::Get_Up() const
{
	return this->up;
}

Vector3 Transform::Get_Euler_Angles() const
{
	return this->rotation.To_Euler();
}

void Transform::Set_Euler_Angles(Vector3 V)
{
	Set_Rotation(Quaternion::Euler(V));
}
void Transform::Set_Euler_Angles(float f1, float f2, float f3)
{
	Set_Rotation(Quaternion::Euler(f1, f2, f3));
}

Vector3 Transform::Get_Local_Euler_Angles() const
{
	return this->local_rotation.To_Euler();
}

void Transform::Set_Local_Euler_Angles(Vector3 V)
{
	Set_Local_Rotation(Quaternion::Euler(V));
}
void Transform::Set_Local_Euler_Angles(float f1, float f2, float f3)
{
	Set_Local_Rotation(Quaternion::Euler(f1, f2, f3));
}

weak_ptr<Transform> Transform::Get_Parent() const
{
	return this->parent;
}

void Transform::Set_Parent(shared_ptr<Transform> P)
{
	if (P != nullptr)
	{
		if (P != parent.lock())
		{
			if (!parent.expired())
			{
				Remove_Parent();
			}

			parent = P;
			P->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
			/*
			localScale = { scale.x / P->scale.x, scale.y / P->scale.y, scale.z / P->scale.z };
			localPosition = { (position.x - P->position.x) * P->localScale.x, (position.y - P->position.y) * P->localScale.y, (position.z - P->position.z) * P->localScale.z };
			Quaternion q;
			P->rotation.Inverse(q);
			localRotation = rotation * q;
			*/
			Matrix m;
			P->Get_World_Matrix().Invert(m);
			local_matrix = Get_World_Matrix() * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);

			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);

			local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
			world_matrix = local_matrix * P->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);

			scale_matrix = Matrix::CreateScale(scale);
			rotation_matrix = Matrix::CreateFromQuaternion(rotation);
			translation_matrix = Matrix::CreateTranslation(position);

			forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
			forward.Normalize();
			right = Vector3::Transform(Vector3::Right, rotation_matrix);
			right.Normalize();
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

void Transform::Set_Parent(shared_ptr<Transform> P, int index_insert)
{
	if (P != nullptr)
	{
		if (P != parent.lock())
		{
			if (!parent.expired())
			{
				Remove_Parent();
			}

			parent = P;
			if (index_insert < 0)
			{
				P->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
			}
			else
			{
				P->children.insert(P->children.begin() + index_insert, static_pointer_cast<Transform>(shared_from_this()));
			}
			/*
			localScale = { scale.x / P->scale.x, scale.y / P->scale.y, scale.z / P->scale.z };
			localPosition = { (position.x - P->position.x) * P->localScale.x, (position.y - P->position.y) * P->localScale.y, (position.z - P->position.z) * P->localScale.z };
			Quaternion q;
			P->rotation.Inverse(q);
			localRotation = rotation * q;
			*/
			Matrix m;
			P->Get_World_Matrix().Invert(m);
			local_matrix = Get_World_Matrix() * m;

			local_matrix.Decompose(local_scale, local_rotation, local_position);

			local_translation_matrix = Matrix::CreateTranslation(local_position);
			local_rotation_matrix = Matrix::CreateFromQuaternion(local_rotation);
			local_scale_matrix = Matrix::CreateScale(local_scale);

			local_matrix = local_scale_matrix * local_rotation_matrix * local_translation_matrix;
			world_matrix = local_matrix * P->Get_World_Matrix();
			world_matrix.Decompose(scale, rotation, position);

			scale_matrix = Matrix::CreateScale(scale);
			rotation_matrix = Matrix::CreateFromQuaternion(rotation);
			translation_matrix = Matrix::CreateTranslation(position);

			forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
			forward.Normalize();
			right = Vector3::Transform(Vector3::Right, rotation_matrix);
			right.Normalize();
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
	shared_ptr<Transform> P = parent.lock();
	world_matrix = local_matrix * P->Get_World_Matrix();
	world_matrix.Decompose(scale, rotation, position);
	translation_matrix = Matrix::CreateTranslation(position);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	scale_matrix = Matrix::CreateScale(scale);
	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();
	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();
	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	update_GUI = true;
}

void Transform::Change_Children()
{
	shared_ptr<Transform> C;
	for (weak_ptr<Transform> child : children)
	{
		if (!child.expired())
		{
			C = child.lock();
			C->On_Parent_Changed();
		}
	}
}

void Transform::Remove_Parent()
{
	if (shared_ptr<Transform> P = parent.lock())
	{
		shared_ptr<Transform> trans;
		auto it = P->children.begin();
		while (it != P->children.end())
		{
			trans = (*it).lock();
			if (trans == static_pointer_cast<Transform>(shared_from_this()))
			{
				P->children.erase(it);
				break;
			}
			else ++it;
		}

		parent.reset();

		local_position = position;
		local_rotation = rotation;
		local_scale = local_scale;

		local_translation_matrix = translation_matrix;
		local_rotation_matrix = rotation_matrix;
		local_scale_matrix = scale_matrix;

		local_matrix = world_matrix;
	}
}

std::weak_ptr<Transform> Transform::Get_Child(int index)
{
	return children[index];
}

int Transform::Get_Child_Count()
{
	return (int)children.size();
}

int Transform::Get_Sibling_Index() const
{
	if (shared_ptr<Transform> P = parent.lock())
	{
		shared_ptr<Transform> child;
		for (size_t i = 0; i < P->children.size(); ++i)
		{
			child = P->children[i].lock();
			if (child == transform)
			{
				return i;
			}
		}
	}
	else
	{
		const unique_ptr<Scene>& scene = Scene_Manager::Get_Active_Scene();
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

weak_ptr<Transform> Transform::Find(std::string n)
{
	shared_ptr<Transform> child;
	for (int i = 0; i < (int)children.size(); ++i)
	{
		child = children[i].lock();
		if (child->gameobject->name == n)
		{
			return child;
		}
	}

	return child;
}

void Transform::Set_Sibling_Index(int index)
{
	if (Get_Sibling_Index() != index)
	{
		if (index < 0) index = 0;

		if (shared_ptr<Transform> P = parent.lock())
		{
			shared_ptr<Transform> insert_at;
			if (index >= (int)P->children.size())
			{
				insert_at = P->children[(int)P->children.size() - 1].lock();
			}
			else
			{
				insert_at = P->children[index].lock();
			}

			shared_ptr<Transform> child;
			for (int i = 0; i < (int)P->children.size(); ++i)
			{
				child = P->children[i].lock();
				if (child == transform)
				{
					P->children.erase(P->children.begin() + i);
					break;
				}
			}

			if (index <= (int)P->children.size())
			{
				bool is_last = true;
				shared_ptr<Transform> child;
				for (int i = 0; i < (int)P->children.size(); ++i)
				{
					child = P->children[i].lock();
					if (child == insert_at)
					{
						is_last = false;
						P->children.insert(P->children.begin() + i, static_pointer_cast<Transform>(shared_from_this()));
						break;
					}
				}
				if (is_last)
				{
					P->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
				}
			}
			else
			{
				P->children.emplace_back(static_pointer_cast<Transform>(shared_from_this()));
			}
		}
		else
		{
			const unique_ptr<Scene>& scene = Scene_Manager::Get_Active_Scene();

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

			if (index <= (int)scene->gameobject_list.size())
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
	return this->world_matrix;
}

void Transform::Set_World_Matrix(Matrix matrix)
{
	world_matrix = matrix;
	world_matrix.Decompose(scale, rotation, position);

	scale_matrix = Matrix::CreateScale(scale);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	translation_matrix = Matrix::CreateTranslation(position);

	if (shared_ptr<Transform> p = parent.lock())
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

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	has_changed = true;
	update_GUI = true;
}

/*  ///////////////////////////////////////////////////////
	Function
*/  ///////////////////////////////////////////////////////

Quaternion Transform::Look_At(Vector3 pos)
{
	Vector3 up = { 0,1,0 };
	Vector3 z = XMVector3Normalize(pos - transform->position);
	Vector3 x = XMVector3Normalize(XMVector3Cross(up, z));
	Vector3 y = XMVector3Normalize(XMVector3Cross(z, x));

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
	float v = sqrt(elem[biggestIdx]) * 0.5f;
	q[biggestIdx] = v;
	float mult = 0.25f / v;

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
	}
	Quaternion rot = { q[0],q[1], q[2], q[3] };
	return rot;
}