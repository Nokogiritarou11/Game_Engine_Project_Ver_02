#include "Bounds.h"
#include "Transform.h"
using namespace std;
using namespace BeastEngine;

void Bounds::Set_Center(const Vector3& value)
{
	center = value;
	min = center - extents;
	max = center + extents;
}
void Bounds::Set_Center(const float& x, const float& y, const float& z)
{
	center = Vector3(x, y, z);
	min = center - extents;
	max = center + extents;
}

void Bounds::Set_Size(const Vector3& value)
{
	size = value;
	extents = size / 2;
	min = center - extents;
	max = center + extents;
}
void Bounds::Set_Size(const float& x, const float& y, const float& z)
{
	size = Vector3(x, y, z);
	extents = size / 2;
	min = center - extents;
	max = center + extents;
}

bool Bounds::Get_Is_Culling_Frustum(const shared_ptr<Transform>& trans, const array<Vector4, 6>& planes) const
{
	bool cull = false;

	const Vector3& position = trans->Get_Position();
	const Vector3& scale = trans->Get_Scale();
	const Vector3 min_scaled = (center + min) * scale;
	const Vector3 max_scaled = (center + max) * scale;

	//Plane‚Ì“à‘¤‚É‚ ‚é‚©”»’è
	for (int plane_id = 0; plane_id < 6; ++plane_id)
	{
		const Vector3 plane_normal = { planes[plane_id].x, planes[plane_id].y, planes[plane_id].z };
		const float plane_constant = planes[plane_id].w;
		Vector3 axis;

		// x-axis
		if (planes[plane_id].x < 0.0f) axis.x = min_scaled.x + position.x;
		else						  axis.x = max_scaled.x + position.x;

		// y-axis
		if (planes[plane_id].y < 0.0f) axis.y = min_scaled.y + position.y;
		else						  axis.y = max_scaled.y + position.y;

		// z-axis
		if (planes[plane_id].z < 0.0f) axis.z = min_scaled.z + position.z;
		else						  axis.z = max_scaled.z + position.z;

		if (plane_normal.Dot(axis) + plane_constant < 0.0f)
		{
			cull = true;
			break;
		}
	}
	return cull;
}
