#pragma once
#include "Debug_UI.h"

class Debug
{
public:
	static void Log();
	static void Log(std::string s);
	static void Log(int i);
	static void Log(float f);
	static void Log(double d);
	static void Log(Vector2 v);
	static void Log(Vector3 v);
	static void Log(Vector4 v);
	static void Log(bool b);
private:

};