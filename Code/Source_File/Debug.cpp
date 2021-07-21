#include "Debug.h"
#include "Engine.h"
#include "Editor.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

void Debug::Log()
{
#if  _DEBUG
	Engine::editor->Print_Log(u8"true");
#endif
}

void Debug::Log(string s)
{
#if  _DEBUG
	Engine::editor->Print_Log(s);
#endif
}

void Debug::Log(char* c)
{
#if  _DEBUG
	string s = c;
	Engine::editor->Print_Log(s);
#endif
}

void Debug::Log(int i)
{
#if  _DEBUG
	Engine::editor->Print_Log(to_string(i));
#endif
}

void Debug::Log(float f)
{
#if  _DEBUG
	Engine::editor->Print_Log(to_string(f));
#endif
}

void Debug::Log(double d)
{
#if  _DEBUG
	Engine::editor->Print_Log(to_string(d));
#endif
}

void Debug::Log(Vector2 v)
{
#if  _DEBUG
	Engine::editor->Print_Log(to_string(v.x) + " " + to_string(v.y));
#endif
}

void Debug::Log(Vector3 v)
{
#if  _DEBUG
	Engine::editor->Print_Log(to_string(v.x) + " " + to_string(v.y) + " " + to_string(v.z));
#endif
}

void Debug::Log(Vector4 v)
{
#if  _DEBUG
	Engine::editor->Print_Log(to_string(v.x) + " " + to_string(v.y) + " " + to_string(v.z) + " " + to_string(v.w));
#endif
}

void Debug::Log(bool b)
{
#if  _DEBUG
	if (b)
	{
		Engine::editor->Print_Log(u8"true");
	}
	else
	{
		Engine::editor->Print_Log(u8"false");
	}
#endif
}
