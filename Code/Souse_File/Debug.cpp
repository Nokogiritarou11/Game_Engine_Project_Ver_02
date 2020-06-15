#include "Debug.h"
using namespace std;
using namespace DirectX;

void Debug::Log()
{
	Debug_UI::Print_Log(u8"true");
}

void Debug::Log(string s)
{
	Debug_UI::Print_Log(s);
}

void Debug::Log(int i)
{
	Debug_UI::Print_Log(to_string(i));
}

void Debug::Log(float f)
{
	Debug_UI::Print_Log(to_string(f));
}

void Debug::Log(double d)
{
	Debug_UI::Print_Log(to_string(d));
}

void Debug::Log(Vector2 v)
{
	Debug_UI::Print_Log(to_string(v.x) + " " + to_string(v.y));
}

void Debug::Log(Vector3 v)
{
	Debug_UI::Print_Log(to_string(v.x) + " " + to_string(v.y) + " " + to_string(v.z));
}

void Debug::Log(Vector4 v)
{
	Debug_UI::Print_Log(to_string(v.x) + " " + to_string(v.y) + " " + to_string(v.z) + " " + to_string(v.w));
}

void Debug::Log(bool b)
{
	if (b)
	{
		Debug_UI::Print_Log(u8"true");
	}
	else
	{
		Debug_UI::Print_Log(u8"false");
	}
}
