#include "Bounds.h"
#include "Transform.h"
using namespace std;
using namespace BeastEngine;

void Bounds::Set_center(const Vector3& value)
{
	center = value;
	min = center - extents;
	max = center + extents;
}
void Bounds::Set_center(const float& x, const float& y, const float& z)
{
	center = { x,y,z };
	min = center - extents;
	max = center + extents;
}

void Bounds::Set_size(const Vector3& value)
{
	size = value;
	extents = size / 2;
	min = center - extents;
	max = center + extents;
}
void Bounds::Set_size(const float& x, const float& y, const float& z)
{
	size = { x,y,z };
	extents = size / 2;
	min = center - extents;
	max = center + extents;
}

bool Bounds::Get_Is_Culling_Frustum(const shared_ptr<Transform>& trans, const array<Vector4, 6>& planes)
{
	bool cull = false;

	const Vector3& position = trans->Get_Position();
	const Vector3 min_scaled = min * trans->Get_Scale();
	const Vector3 max_scaled = max * trans->Get_Scale();

	for (int planeID = 0; planeID < 6; ++planeID)
	{
		const Vector3 planeNormal = { planes[planeID].x, planes[planeID].y, planes[planeID].z };
		const float planeConstant = planes[planeID].w;
		Vector3 axisVert;

		// x-axis
		if (planes[planeID].x < 0.0f) axisVert.x = min_scaled.x + position.x;
		else						  axisVert.x = max_scaled.x + position.x;

		// y-axis
		if (planes[planeID].y < 0.0f) axisVert.y = min_scaled.y + position.y;
		else						  axisVert.y = max_scaled.y + position.y;

		// z-axis
		if (planes[planeID].z < 0.0f) axisVert.z = min_scaled.z + position.z;
		else						  axisVert.z = max_scaled.z + position.z;

		if (planeNormal.Dot(axisVert) + planeConstant < 0.0f)
		{
			cull = true;
			break;
		}
	}
	return cull;
}