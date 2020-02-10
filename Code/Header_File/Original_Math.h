#pragma once
#include <random>
#include <stdlib.h>

float lerp(float start, float end, float t)
{
	return (1 - t) * start + t * end;
}

bool Probability(float fPercent)
{
	fPercent /= 100;
	if ((float)rand() / RAND_MAX < fPercent)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float Random_Range(float min, float max)
{
	float f = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	return f;
}
