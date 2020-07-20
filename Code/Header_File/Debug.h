#pragma once
#include <stdlib.h>
#include <string>
#include "Original_Math.h"

class Debug
{
public:
	static void Log();
	static void Log(std::string s);
	static void Log(char* c);
	static void Log(int i);
	static void Log(float f);
	static void Log(double d);
	static void Log(Vector2 v);
	static void Log(Vector3 v);
	static void Log(Vector4 v);
	static void Log(bool b);
private:

};