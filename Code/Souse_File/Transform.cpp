#include "Transform.h"
#include "GameObject.h"
using namespace DirectX;
using namespace std;

Transform::Transform()
{
}

Transform::Transform(Vector3 _position, Quaternion _rotation)
{
	position = _position;
	rotation = _rotation;
}

Transform::Transform(Vector3 _position, Vector3 _euler)
{
	position = _position;
	rotation = Quaternion::Euler(_euler);
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

void Transform::Update()
{
	// ƒ[ƒ‹ƒhs—ñ‚ðì¬
	{
		XMMATRIX S, R, T;
		S = XMMatrixScaling(scale.x, scale.y, scale.z);
		//R = XMMatrixRotationRollPitchYaw(XMConvertToRadians(eulerAngles.x), XMConvertToRadians(eulerAngles.y), XMConvertToRadians(eulerAngles.z));	// ZXY‰ñ“]
		R = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
		T = XMMatrixTranslation(position.x, position.y, position.z);

		XMStoreFloat4x4(&scale_matrix, S);
		XMStoreFloat4x4(&rotation_matrix, R);
		XMStoreFloat4x4(&translation_matrix, T);
		XMStoreFloat4x4(&world_matrix, S * R * T);

		XMVECTOR right_v = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), up_v = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), forward_v = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		forward_v = XMVector3Transform(forward_v, R);
		forward_v = XMVector3Normalize(forward_v);

		right_v = XMVector3Transform(right_v, R);
		right_v = XMVector3Normalize(right_v);

		up_v = XMVector3Transform(up_v, R);
		up_v = XMVector3Normalize(up_v);

		XMStoreFloat3(&forward, forward_v);
		XMStoreFloat3(&right, right_v);
		XMStoreFloat3(&up, up_v);
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
}
void Transform::Set_position(float f1, float f2, float f3)
{
	position = { f1,f2,f3 };
}

Quaternion Transform::Get_rotation() const
{
	return this->rotation;
}

void Transform::Set_rotation(Quaternion Q)
{
	rotation = Q;
}
void Transform::Set_rotation(float f1, float f2, float f3, float f4)
{
	rotation = { f1,f2,f3,f4 };
}

Vector3 Transform::Get_scale() const
{
	return this->scale;
}

void Transform::Set_scale(Vector3 V)
{
	scale = V;
}
void Transform::Set_scale(float f1, float f2, float f3)
{
	scale = { f1,f2,f3 };
}

Vector3 Transform::Get_localPosition() const
{
	return this->localPosition;
}

void Transform::Set_localPosition(Vector3 V)
{
	localPosition = V;
}
void Transform::Set_localPosition(float f1, float f2, float f3)
{
	localPosition = { f1,f2,f3 };
}

Quaternion Transform::Get_localRotation() const
{
	return this->localRotation;
}

void Transform::Set_localRotation(Quaternion Q)
{
	localRotation = Q;
}
void Transform::Set_localRotation(float f1, float f2, float f3, float f4)
{
	localRotation = { f1,f2,f3,f4 };
}

Vector3 Transform::Get_localScale() const
{
	return this->localScale;
}

void Transform::Set_localScale(Vector3 V)
{
	localScale = V;
}
void Transform::Set_localScale(float f1, float f2, float f3)
{
	localScale = { f1,f2,f3 };
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
	rotation = Quaternion::Euler(V);
}
void Transform::Set_eulerAngles(float f1, float f2, float f3)
{
	rotation = Quaternion::Euler(f1, f2, f3);
}

Vector3 Transform::Get_localEulerAngles() const
{
	return this->localRotation.To_Euler();
}

void Transform::Set_localEulerAngles(Vector3 V)
{
	localRotation = Quaternion::Euler(V);
}
void Transform::Set_localEulerAngles(float f1, float f2, float f3)
{
	localRotation = Quaternion::Euler(f1, f2, f3);
}

weak_ptr<Transform> Transform::Get_parent() const
{
	return this->parent;
}

void Transform::Set_parent(weak_ptr<Transform>   P)
{
	parent = P;
}
void Transform::Set_parent(shared_ptr<Transform> P)
{
	parent = P;
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