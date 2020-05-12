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
	position    = _position;
	eulerAngles = _euler;
}

Transform::~Transform()
{
}

void Transform::Initialize(shared_ptr<GameObject> obj)
{
	gameObject     = obj;
	obj->transform = static_pointer_cast<Transform>(shared_from_this());
	transform      = static_pointer_cast<Transform>(shared_from_this());
}

void Transform::Update()
{
	// ÉèÅ[ÉãÉhçsóÒÇçÏê¨
	{
		XMMATRIX S, R, T;
		S = XMMatrixScaling(scale.x, scale.y, scale.z);
		//R = XMMatrixRotationRollPitchYaw(XMConvertToRadians(eulerAngles.x), XMConvertToRadians(eulerAngles.y), XMConvertToRadians(eulerAngles.z));	// ZXYâÒì]
		R = XMMatrixRotationQuaternion(_rotation);
		T = XMMatrixTranslation(position.x, position.y, position.z);

		XMStoreFloat4x4(&scale_matrix, S);
		XMStoreFloat4x4(&rotation_matrix, R);
		XMStoreFloat4x4(&translation_matrix, T);
		XMStoreFloat4x4(&world, S * R * T);

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

void Transform::Change_rot(Quaternion q)
{
	_rotation = q;
	_eulerAngles = _rotation.eulerAngles;
}

void Transform::Change_rot(Vector3 v)
{
	_eulerAngles = v;
	_rotation.eulerAngles = _eulerAngles;
}