#include "Transform.h"
#include "Scene_Manager.h"
#include "Include_ImGui.h"
using namespace DirectX;
using namespace std;

Transform::Transform()
{
	if (shared_ptr<Transform> p = parent.lock())
	{
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		localScale_matrix = Matrix::CreateScale(localScale);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
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

		localPosition = position;
		localRotation = rotation;
		localScale = scale;

		localTranslation_matrix = translation_matrix;
		localRotation_matrix = rotation_matrix;
		localScale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	hasChanged = false;
}

Transform::Transform(Vector3 _position, Quaternion _rotation)
{
	position = _position;
	rotation = _rotation;
	scale = { 1,1,1 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		localScale_matrix = Matrix::CreateScale(localScale);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
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

		localPosition = position;
		localRotation = rotation;
		localScale = scale;

		localTranslation_matrix = translation_matrix;
		localRotation_matrix = rotation_matrix;
		localScale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();


	hasChanged = false;
}

Transform::Transform(Vector3 _position, Vector3 _euler)
{
	position = _position;
	rotation = Quaternion::Euler(_euler);
	scale = { 1,1,1 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		localScale_matrix = Matrix::CreateScale(localScale);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
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

		localPosition = position;
		localRotation = rotation;
		localScale = scale;

		localTranslation_matrix = translation_matrix;
		localRotation_matrix = rotation_matrix;
		localScale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();


	hasChanged = false;
}

Transform::~Transform()
{
}

void Transform::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
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

		if (Update_GUI || active_trans.lock() != gameObject->transform)
		{
			active_trans = gameObject->transform;

			Vector3 trans_pos = { 0,0,0 };
			Vector3 trans_rot = { 0,0,0 };
			Vector3 trans_scl = { 0,0,0 };

			trans_pos = Get_localPosition();
			trans_rot = Get_localEulerAngles();
			trans_scl = Get_localScale();

			pos[0] = trans_pos.x;
			pos[1] = trans_pos.y;
			pos[2] = trans_pos.z;

			rot[0] = trans_rot.x;
			rot[1] = trans_rot.y;
			rot[2] = trans_rot.z;

			scl[0] = trans_scl.x;
			scl[1] = trans_scl.y;
			scl[2] = trans_scl.z;

			Update_GUI = false;
		}

		if (ImGui::DragFloat3(u8"Position", pos, 0.05f, -FLT_MAX, FLT_MAX))
		{
			Set_localPosition(pos[0], pos[1], pos[2]);
		}
		if (ImGui::DragFloat3(u8"Rotation", rot, 0.05f, -FLT_MAX, FLT_MAX))
		{
			Set_localEulerAngles(rot[0], rot[1], rot[2]);
		}
		if (ImGui::DragFloat3(u8"Scale", scl, 0.01f, -FLT_MAX, FLT_MAX))
		{
			Set_localScale(scl[0], scl[1], scl[2]);
		}
	}
	return true;
}

/*  ///////////////////////////////////////////////////////
	Getter & Setter
*/  ///////////////////////////////////////////////////////
Vector3 Transform::Get_position() const
{
	return this->position;
}

void Transform::Set_position(Vector3 V)
{
	position = V;

	if (shared_ptr<Transform> p = parent.lock())
	{
		localPosition = { (position.x - p->position.x) * p->localScale.x, (position.y - p->position.y) * p->localScale.y, (position.z - p->position.z) * p->localScale.z };
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		translation_matrix = Matrix::CreateTranslation(position);
	}
	else
	{
		translation_matrix = Matrix::CreateTranslation(position);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		local_matrix = world_matrix;
		localTranslation_matrix = translation_matrix;
		localPosition = position;
	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

void Transform::Set_position(float f1, float f2, float f3)
{
	position = { f1,f2,f3 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		localPosition = { (position.x - p->position.x) * p->localScale.x, (position.y - p->position.y) * p->localScale.y, (position.z - p->position.z) * p->localScale.z };
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		translation_matrix = Matrix::CreateTranslation(position);
	}
	else
	{
		translation_matrix = Matrix::CreateTranslation(position);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		local_matrix = world_matrix;
		localTranslation_matrix = translation_matrix;
		localPosition = position;
	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

Quaternion Transform::Get_rotation() const
{
	return this->rotation;
}

void Transform::Set_rotation(Quaternion Q)
{
	rotation = Q;

	if (shared_ptr<Transform> p = parent.lock())
	{
		Quaternion q;
		p->rotation.Inverse(q);
		localRotation = rotation * q;
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	}
	else
	{
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		localRotation = rotation;
		localRotation_matrix = rotation_matrix;
		local_matrix = world_matrix;

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}
void Transform::Set_rotation(float f1, float f2, float f3, float f4)
{
	rotation = { f1,f2,f3,f4 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		Quaternion q;
		p->rotation.Inverse(q);
		localRotation = rotation * q;
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	}
	else
	{
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		localRotation = rotation;
		localRotation_matrix = rotation_matrix;
		local_matrix = world_matrix;

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

Vector3 Transform::Get_scale() const
{
	return this->scale;
}

void Transform::Set_scale(Vector3 V)
{
	scale = V;
	if (shared_ptr<Transform> p = parent.lock())
	{
		localScale = { scale.x / p->scale.x, scale.y / p->scale.y, scale.z / p->scale.z };
		localScale_matrix = Matrix::CreateScale(localScale);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		scale_matrix = Matrix::CreateScale(scale);
	}
	else
	{
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		localScale = scale;
		localScale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}
	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}
void Transform::Set_scale(float f1, float f2, float f3)
{
	scale = { f1,f2,f3 };
	if (shared_ptr<Transform> p = parent.lock())
	{
		localScale = { scale.x / p->scale.x, scale.y / p->scale.y, scale.z / p->scale.z };
		localScale_matrix = Matrix::CreateScale(localScale);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		scale_matrix = Matrix::CreateScale(scale);
	}
	else
	{
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;

		localScale = scale;
		localScale_matrix = scale_matrix;
		local_matrix = world_matrix;
	}
	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

Vector3 Transform::Get_localPosition() const
{
	return this->localPosition;
}

void Transform::Set_localPosition(Vector3 V)
{
	localPosition = V;
	localTranslation_matrix = Matrix::CreateTranslation(localPosition);
	local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
	{
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		translation_matrix = Matrix::CreateTranslation(position);
	}
	else
	{
		world_matrix = local_matrix;
		position = localPosition;
		translation_matrix = localTranslation_matrix;
	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}
void Transform::Set_localPosition(float f1, float f2, float f3)
{
	localPosition = { f1,f2,f3 };
	localTranslation_matrix = Matrix::CreateTranslation(localPosition);
	local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
	{
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		translation_matrix = Matrix::CreateTranslation(position);
	}
	else
	{
		world_matrix = local_matrix;
		position = localPosition;
		translation_matrix = localTranslation_matrix;
	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

Quaternion Transform::Get_localRotation() const
{
	return this->localRotation;
}

void Transform::Set_localRotation(Quaternion Q)
{
	localRotation = Q;
	localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
	local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
	{
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	}
	else
	{
		world_matrix = local_matrix;
		rotation = localRotation;
		rotation_matrix = localRotation_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();
	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();
	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}
void Transform::Set_localRotation(float f1, float f2, float f3, float f4)
{
	localRotation = { f1,f2,f3,f4 };
	localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
	local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
	{
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	}
	else
	{
		world_matrix = local_matrix;
		rotation = localRotation;
		rotation_matrix = localRotation_matrix;
	}

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

Vector3 Transform::Get_localScale() const
{
	return this->localScale;
}

void Transform::Set_localScale(Vector3 V)
{
	localScale = V;
	localScale_matrix = Matrix::CreateScale(localScale);
	local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
	{
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		scale_matrix = Matrix::CreateScale(scale);
	}
	else
	{
		world_matrix = local_matrix;
		scale = localScale;
		scale_matrix = localScale_matrix;
	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}
void Transform::Set_localScale(float f1, float f2, float f3)
{
	localScale = { f1,f2,f3 };
	localScale_matrix = Matrix::CreateScale(localScale);
	local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
	if (shared_ptr<Transform> p = parent.lock())
	{
		world_matrix = local_matrix * p->Get_world_matrix();
		world_matrix.Decompose(scale, rotation, position);
		scale_matrix = Matrix::CreateScale(scale);
	}
	else
	{
		world_matrix = local_matrix;
		scale = localScale;
		scale_matrix = localScale_matrix;
	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

Vector3 Transform::Get_forward() const
{
	return this->forward;
}

Vector3 Transform::Get_right() const
{
	return this->right;
}

Vector3 Transform::Get_up() const
{
	return this->up;
}

Vector3 Transform::Get_eulerAngles() const
{
	return this->rotation.To_Euler();
}

void Transform::Set_eulerAngles(Vector3 V)
{
	Set_rotation(Quaternion::Euler(V));
}
void Transform::Set_eulerAngles(float f1, float f2, float f3)
{
	Set_rotation(Quaternion::Euler(f1, f2, f3));
}

Vector3 Transform::Get_localEulerAngles() const
{
	return this->localRotation.To_Euler();
}

void Transform::Set_localEulerAngles(Vector3 V)
{
	Set_localRotation(Quaternion::Euler(V));
}
void Transform::Set_localEulerAngles(float f1, float f2, float f3)
{
	Set_localRotation(Quaternion::Euler(f1, f2, f3));
}

weak_ptr<Transform> Transform::Get_parent() const
{
	return this->parent;
}

void Transform::Set_parent(shared_ptr<Transform> P)
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

			localScale = { scale.x / P->scale.x, scale.y / P->scale.y, scale.z / P->scale.z };
			localPosition = { (position.x - P->position.x) * P->localScale.x, (position.y - P->position.y) * P->localScale.y, (position.z - P->position.z) * P->localScale.z };
			Quaternion q;
			P->rotation.Inverse(q);
			localRotation = rotation * q;

			localTranslation_matrix = Matrix::CreateTranslation(localPosition);
			localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
			localScale_matrix = Matrix::CreateScale(localScale);

			local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
			world_matrix = local_matrix * P->Get_world_matrix();
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
	hasChanged = true;
	Update_GUI = true;
}

void Transform::Set_parent(shared_ptr<Transform> P, int index_insert)
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
			localScale = { scale.x / P->scale.x, scale.y / P->scale.y, scale.z / P->scale.z };
			localPosition = { (position.x - P->position.x) * P->localScale.x, (position.y - P->position.y) * P->localScale.y, (position.z - P->position.z) * P->localScale.z };
			Quaternion q;
			P->rotation.Inverse(q);
			localRotation = rotation * q;

			localTranslation_matrix = Matrix::CreateTranslation(localPosition);
			localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
			localScale_matrix = Matrix::CreateScale(localScale);

			local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
			world_matrix = local_matrix * P->Get_world_matrix();
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
	hasChanged = true;
	Update_GUI = true;
}

void Transform::OnParentChanged()
{
	shared_ptr<Transform> P = parent.lock();
	world_matrix = local_matrix * P->Get_world_matrix();
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
	Update_GUI = true;
}

void Transform::Change_Children()
{
	shared_ptr<Transform> C;
	for (weak_ptr<Transform> child : children)
	{
		if (!child.expired())
		{
			C = child.lock();
			C->OnParentChanged();
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

		localPosition = position;
		localRotation = rotation;
		localScale = localScale;

		localTranslation_matrix = translation_matrix;
		localRotation_matrix = rotation_matrix;
		localScale_matrix = scale_matrix;

		local_matrix = world_matrix;
	}
}

std::weak_ptr<Transform> Transform::GetChild(int index)
{
	return children[index];
}

int Transform::childCount()
{
	return (int)children.size();
}

int Transform::GetSiblingIndex() const
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

		for (size_t i = 0; i < scene->gameObject_List.size(); ++i)
		{
			if (scene->gameObject_List[i]->transform->Get_parent().expired())
			{
				++index_this;
				if (scene->gameObject_List[i] == gameObject)
				{
					return index_this;
				}
			}
		}
	}
	return -1;
}

void Transform::SetSiblingIndex(int index)
{
	if (GetSiblingIndex() != index)
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
			for (size_t i = 0; i < scene->gameObject_List.size(); ++i)
			{
				if (scene->gameObject_List[i]->transform->Get_parent().expired())
				{
					++index_insert;
					if (index_insert == index)
					{
						insert_at = scene->gameObject_List[i]->transform;
						break;
					}
				}
			}

			for (size_t i = 0; i < scene->gameObject_List.size(); ++i)
			{
				if (scene->gameObject_List[i]->transform->Get_parent().expired())
				{
					if (scene->gameObject_List[i] == gameObject)
					{
						scene->gameObject_List.erase(scene->gameObject_List.begin() + i);
						break;
					}
				}
			}

			if (index <= (int)scene->gameObject_List.size())
			{
				bool is_last = true;
				for (size_t i = 0; i < scene->gameObject_List.size(); ++i)
				{
					if (scene->gameObject_List[i]->transform->Get_parent().expired())
					{
						if (scene->gameObject_List[i]->transform == insert_at)
						{
							is_last = false;
							scene->gameObject_List.insert(scene->gameObject_List.begin() + i, gameObject);
							break;
						}
					}
				}
				if (is_last)
				{
					scene->gameObject_List.emplace_back(gameObject);
				}
			}
			else
			{
				scene->gameObject_List.emplace_back(gameObject);
			}
		}
	}
}

Matrix Transform::Get_world_matrix() const
{
	return this->world_matrix;
}

void Transform::Set_world_matrix(Matrix matrix)
{
	world_matrix = matrix;
	world_matrix.Decompose(scale, rotation, position);

	scale_matrix = Matrix::CreateScale(scale);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	translation_matrix = Matrix::CreateTranslation(position);

	if (shared_ptr<Transform> p = parent.lock())
	{
		localPosition = { (position.x - p->position.x) * p->localScale.x, (position.y - p->position.y) * p->localScale.y, (position.z - p->position.z) * p->localScale.z };
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		Quaternion q;
		p->rotation.Inverse(q);
		localRotation = rotation * q;
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		localScale = { scale.x / p->scale.x, scale.y / p->scale.y, scale.z / p->scale.z };
		localScale_matrix = Matrix::CreateScale(localScale);

		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
	}
	else
	{
		local_matrix = world_matrix;
		localScale = scale;
		localScale_matrix = scale_matrix;
		localRotation = rotation;
		localRotation_matrix = rotation_matrix;
		localPosition = position;
		localTranslation_matrix = translation_matrix;
	}

	Change_Children();
	hasChanged = true;
	Update_GUI = true;
}

/*  ///////////////////////////////////////////////////////
	Function
*/  ///////////////////////////////////////////////////////

Quaternion Transform::LookAt(Vector3 pos)
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