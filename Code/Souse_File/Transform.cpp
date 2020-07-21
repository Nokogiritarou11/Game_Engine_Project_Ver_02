#include "Transform.h"
#include "Include_ImGui.h"
using namespace DirectX;
using namespace std;

Transform::Transform()
{
	translation_matrix = Matrix::CreateTranslation(position);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	scale_matrix = Matrix::CreateScale(scale);

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	world_matrix = scale_matrix * rotation_matrix * translation_matrix;
	hasChanged = false;
}

Transform::Transform(Vector3 _position, Quaternion _rotation)
{
	position = _position;
	rotation = _rotation;
	scale = { 1,1,1 };

	translation_matrix = Matrix::CreateTranslation(position);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	scale_matrix = Matrix::CreateScale(scale);

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

	hasChanged = false;
}

Transform::Transform(Vector3 _position, Vector3 _euler)
{
	position = _position;
	rotation = Quaternion::Euler(_euler);
	scale = { 1,1,1 };

	translation_matrix = Matrix::CreateTranslation(position);
	rotation_matrix = Matrix::CreateFromQuaternion(rotation);
	scale_matrix = Matrix::CreateScale(scale);

	forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
	forward.Normalize();

	right = Vector3::Transform(Vector3::Right, rotation_matrix);
	right.Normalize();

	up = Vector3::Transform(Vector3::Up, rotation_matrix);
	up.Normalize();

	world_matrix = scale_matrix * rotation_matrix * translation_matrix;

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

/*
void Transform::Update()
{
	if (!isUpdated)
	{
		// ƒ[ƒ‹ƒhs—ñ‚ðì¬
		{
			Set_parameter();
			isUpdated = true;
		}
	}
}
*/

void Transform::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("Transform"))
	{
		static unsigned int has_ID = -1;
		static bool has_parent;

		static float pos[3] = { 0,0,0 };
		static float rot[3] = { 0,0,0 };
		static float scl[3] = { 1,1,1 };

		if (has_ID != gameObject->ID)
		{
			has_ID = gameObject->ID;
			if (shared_ptr<Transform> p = Get_parent().lock())
			{
				has_parent = true;
			}
			else
			{
				has_parent = false;
			}

			Vector3 trans_pos = { 0,0,0 };
			Vector3 trans_rot = { 0,0,0 };
			Vector3 trans_scl = { 0,0,0 };

			if (has_parent)
			{
				trans_pos = Get_localPosition();
				trans_rot = Get_localEulerAngles();
				trans_scl = Get_localScale();
			}
			else
			{
				trans_pos = Get_position();
				trans_rot = Get_eulerAngles();
				trans_scl = Get_scale();
			}
			pos[0] = trans_pos.x;
			pos[1] = trans_pos.y;
			pos[2] = trans_pos.z;

			rot[0] = trans_rot.x;
			rot[1] = trans_rot.y;
			rot[2] = trans_rot.z;
			
			scl[0] = trans_scl.x;
			scl[1] = trans_scl.y;
			scl[2] = trans_scl.z;
		}

		if (ImGui::DragFloat3(u8"Position", pos, 0.05f, -FLT_MAX, FLT_MAX))
		{
			if (has_parent)
			{
				Set_localPosition(pos[0], pos[1], pos[2]);
			}
			else
			{
				Set_position(pos[0], pos[1], pos[2]);
			}
		}
		if (ImGui::DragFloat3(u8"Rotation", rot, 0.05f, -FLT_MAX, FLT_MAX))
		{
			if (has_parent)
			{
				Set_localEulerAngles(rot[0], rot[1], rot[2]);
			}
			else
			{
				Set_eulerAngles(rot[0], rot[1], rot[2]);
			}
		}
		if (ImGui::DragFloat3(u8"Scale", scl, 0.01f, -FLT_MAX, FLT_MAX))
		{
			if (has_parent)
			{
				Set_localScale(scl[0], scl[1], scl[2]);
			}
			else
			{
				Set_scale(scl[0], scl[1], scl[2]);
			}
		}
	}
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
		Set_localPosition((position.x - p->position.x) * localScale.x, (position.y - p->position.y) * localScale.y, (position.z - p->position.z) * localScale.z);
	}
	else
	{
		translation_matrix = Matrix::CreateTranslation(position);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;
	}
	hasChanged = true;
}

void Transform::Set_position(float f1, float f2, float f3)
{
	position = { f1,f2,f3 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		Set_localPosition((position.x - p->position.x) * localScale.x, (position.y - p->position.y) * localScale.y, (position.z - p->position.z) * localScale.z);
	}
	else
	{
		translation_matrix = Matrix::CreateTranslation(position);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;
	}
	hasChanged = true;
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
		Set_localRotation(rotation * q);
	}
	else
	{
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		world_matrix = scale_matrix * rotation_matrix * translation_matrix;
	}
	hasChanged = true;
}
void Transform::Set_rotation(float f1, float f2, float f3, float f4)
{
	rotation = { f1,f2,f3,f4 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		Quaternion q;
		p->rotation.Inverse(q);
		Set_localRotation(rotation * q);
	}
	else
	{
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		world_matrix = scale_matrix * rotation_matrix * translation_matrix;
	}
	hasChanged = true;
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
		Set_localScale(scale.x / p->scale.x, scale.y / p->scale.y, scale.z / p->scale.z);
	}
	else
	{
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;
		hasChanged = true;
	}
}
void Transform::Set_scale(float f1, float f2, float f3)
{
	scale = { f1,f2,f3 };
	if (shared_ptr<Transform> p = parent.lock())
	{
		Set_localScale(scale.x / p->scale.x, scale.y / p->scale.y, scale.z / p->scale.z);
	}
	else
	{
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = scale_matrix * rotation_matrix * translation_matrix;
		hasChanged = true;
	}
}

Vector3 Transform::Get_localPosition() const
{
	return this->localPosition;
}

void Transform::Set_localPosition(Vector3 V)
{
	localPosition = V;

	if (shared_ptr<Transform> p = parent.lock())
	{
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		position = { world_matrix._41 * localScale.x,world_matrix._42 * localScale.y, world_matrix._43 * localScale.z };
		translation_matrix = Matrix::CreateTranslation(position);
	}
	else
	{
		Set_position(V);
	}
	hasChanged = true;
}
void Transform::Set_localPosition(float f1, float f2, float f3)
{
	localPosition = { f1,f2,f3 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();
		position = { world_matrix._41 * localScale.x,world_matrix._42 * localScale.y, world_matrix._43 * localScale.z };
		translation_matrix = Matrix::CreateTranslation(position);
	}
	else
	{
		Set_position(f1, f2, f3);
	}
	hasChanged = true;
}

Quaternion Transform::Get_localRotation() const
{
	return this->localRotation;
}

void Transform::Set_localRotation(Quaternion Q)
{
	localRotation = Q;

	if (shared_ptr<Transform> p = parent.lock())
	{
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();

		Matrix w = world_matrix;
		w._41 = w._42 = w._43 = 0;

		rotation = Quaternion::CreateFromRotationMatrix(w);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();
	}
	else
	{
		Set_rotation(Q);
	}
	hasChanged = true;
}
void Transform::Set_localRotation(float f1, float f2, float f3, float f4)
{
	localRotation = { f1,f2,f3,f4 };

	if (shared_ptr<Transform> p = parent.lock())
	{
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();

		Matrix w = world_matrix;
		w._41 = w._42 = w._43 = 0;

		rotation = Quaternion::CreateFromRotationMatrix(w);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);

		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();

		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();

		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();
	}
	else
	{
		Set_rotation(f1, f2, f3, f4);
	}
	hasChanged = true;
}

Vector3 Transform::Get_localScale() const
{
	return this->localScale;
}

void Transform::Set_localScale(Vector3 V)
{
	localScale = V;
	if (shared_ptr<Transform> p = parent.lock())
	{
		localScale_matrix = Matrix::CreateScale(localScale);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		scale = localScale * p->scale;
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = local_matrix * p->Get_world_matrix();
	}
	else
	{
		Set_scale(V);
	}
	hasChanged = true;
}
void Transform::Set_localScale(float f1, float f2, float f3)
{
	if (shared_ptr<Transform> p = parent.lock())
	{
		localScale_matrix = Matrix::CreateScale(localScale);
		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		scale = localScale * p->scale;
		scale_matrix = Matrix::CreateScale(scale);
		world_matrix = local_matrix * p->Get_world_matrix();
	}
	else
	{
		Set_scale(f1, f2, f3);
	}
	hasChanged = true;
}

Vector3 Transform::Get_forward() const
{
	return this->forward;
}

void Transform::Set_forward(Vector3 V)
{
	forward = V;
}
void Transform::Set_forward(float f1, float f2, float f3)
{
	forward = { f1,f2,f3 };
}

Vector3 Transform::Get_right() const
{
	return this->right;
}

void Transform::Set_right(Vector3 V)
{
	right = V;
}
void Transform::Set_right(float f1, float f2, float f3)
{
	right = { f1,f2,f3 };
}

Vector3 Transform::Get_up() const
{
	return this->up;
}

void Transform::Set_up(Vector3 V)
{
	up = V;
}
void Transform::Set_up(float f1, float f2, float f3)
{
	up = { f1,f2,f3 };
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

void Transform::Set_parent(weak_ptr<Transform>   P)
{
	if (shared_ptr<Transform> p = P.lock())
	{
		parent = P;

		localScale = { scale.x / p->scale.x, scale.y / p->scale.y, scale.z / p->scale.z };
		localPosition = { (position.x - p->position.x) * localScale.x, (position.y - p->position.y) * localScale.y, (position.z - p->position.z) * localScale.z };
		Quaternion q;
		p->rotation.Inverse(q);
		localRotation = rotation * q;

		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		localScale_matrix = Matrix::CreateScale(localScale);

		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * p->Get_world_matrix();

		position = { world_matrix._41 * localScale.x,world_matrix._42 * localScale.y, world_matrix._43 * localScale.z };
		translation_matrix = Matrix::CreateTranslation(position);

		Matrix w = world_matrix;
		w._41 = w._42 = w._43 = 0;
		rotation = Quaternion::CreateFromRotationMatrix(w);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();
		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();
		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		scale = localScale * p->scale;
		scale_matrix = Matrix::CreateScale(scale);
	}
	else
	{
		if (shared_ptr<Transform> pa = parent.lock())
		{
			parent.reset();
		}
	}
}
void Transform::Set_parent(shared_ptr<Transform> P)
{
	if (P == nullptr)
	{
		if (shared_ptr<Transform> pa = parent.lock())
		{
			parent.reset();
		}
	}
	else
	{
		parent = P;

		localScale = { scale.x / P->scale.x, scale.y / P->scale.y, scale.z / P->scale.z };
		localPosition = { (position.x - P->position.x) * localScale.x, (position.y - P->position.y) * localScale.y, (position.z - P->position.z) * localScale.z };
		Quaternion q;
		P->rotation.Inverse(q);
		localRotation = rotation * q;

		localTranslation_matrix = Matrix::CreateTranslation(localPosition);
		localRotation_matrix = Matrix::CreateFromQuaternion(localRotation);
		localScale_matrix = Matrix::CreateScale(localScale);

		local_matrix = localScale_matrix * localRotation_matrix * localTranslation_matrix;
		world_matrix = local_matrix * P->Get_world_matrix();

		position = { world_matrix._41 * localScale.x,world_matrix._42 * localScale.y, world_matrix._43 * localScale.z };
		translation_matrix = Matrix::CreateTranslation(position);

		Matrix w = world_matrix;
		w._41 = w._42 = w._43 = 0;
		rotation = Quaternion::CreateFromRotationMatrix(w);
		rotation_matrix = Matrix::CreateFromQuaternion(rotation);
		forward = Vector3::Transform(Vector3::Forward, rotation_matrix);
		forward.Normalize();
		right = Vector3::Transform(Vector3::Right, rotation_matrix);
		right.Normalize();
		up = Vector3::Transform(Vector3::Up, rotation_matrix);
		up.Normalize();

		scale = localScale * P->scale;
		scale_matrix = Matrix::CreateScale(scale);
	}
}

Matrix Transform::Get_world_matrix() const
{
	return this->world_matrix;
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