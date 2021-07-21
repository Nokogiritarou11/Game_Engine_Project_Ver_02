#include "Bounds.h"

using namespace BeastEngine;

void Bounds::Set_center(Vector3 value)
{
	center = value;
	min = center - extents;
	max = center + extents;
}
void Bounds::Set_center(float x, float y, float z)
{
	center = { x,y,z };
	min = center - extents;
	max = center + extents;
}

void Bounds::Set_size(Vector3 value)
{
	size = value;
	extents = size / 2;
	min = center - extents;
	max = center + extents;
}
void Bounds::Set_size(float x, float y, float z)
{
	size = { x,y,z };
	extents = size / 2;
	min = center - extents;
	max = center + extents;
}