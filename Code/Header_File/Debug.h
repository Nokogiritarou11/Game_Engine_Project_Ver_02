#pragma once
#include <stdlib.h>
#include <string>
#include "Original_Math.h"

class Debug
{
public:
	static void Log();              //デバッグログに追加する
	static void Log(std::string s); //デバッグログに追加する
	static void Log(char* c);       //デバッグログに追加する
	static void Log(int i);         //デバッグログに追加する
	static void Log(float f);       //デバッグログに追加する
	static void Log(double d);      //デバッグログに追加する
	static void Log(Vector2 v);     //デバッグログに追加する
	static void Log(Vector3 v);     //デバッグログに追加する
	static void Log(Vector4 v);     //デバッグログに追加する
	static void Log(bool b);        //デバッグログに追加する
private:

};