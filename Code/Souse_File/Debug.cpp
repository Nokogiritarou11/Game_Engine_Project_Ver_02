#include "Debug.h"
#include "Engine.h"
using namespace std;
using namespace DirectX;

void Debug::Log()
{
	Engine::debug_ui->Print_Log(u8"true");
}

void Debug::Log(string s)
{
	Engine::debug_ui->Print_Log(s);
}

void Debug::Log(char* c)
{
	string s = c;
	Engine::debug_ui->Print_Log(s);
}

void Debug::Log(int i)
{
	Engine::debug_ui->Print_Log(to_string(i));
}

void Debug::Log(float f)
{
	Engine::debug_ui->Print_Log(to_string(f));
}

void Debug::Log(double d)
{
	Engine::debug_ui->Print_Log(to_string(d));
}

void Debug::Log(Vector2 v)
{
	Engine::debug_ui->Print_Log(to_string(v.x) + " " + to_string(v.y));
}

void Debug::Log(Vector3 v)
{
	Engine::debug_ui->Print_Log(to_string(v.x) + " " + to_string(v.y) + " " + to_string(v.z));
}

void Debug::Log(Vector4 v)
{
	Engine::debug_ui->Print_Log(to_string(v.x) + " " + to_string(v.y) + " " + to_string(v.z) + " " + to_string(v.w));
}

void Debug::Log(bool b)
{
	if (b)
	{
		Engine::debug_ui->Print_Log(u8"true");
	}
	else
	{
		Engine::debug_ui->Print_Log(u8"false");
	}
}
