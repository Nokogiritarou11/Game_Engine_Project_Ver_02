#pragma once
#include <string>
#include "Original_Math.h"

namespace BeastEngine
{
	//デバッグ表示
	class Debug
	{
	public:
		static void Log();                     //デバッグログに追加する
		static void Log(const std::string& s); //デバッグログに追加する
		static void Log(const char* c);        //デバッグログに追加する
		static void Log(int i);                //デバッグログに追加する
		static void Log(float f);              //デバッグログに追加する
		static void Log(double d);             //デバッグログに追加する
		static void Log(Vector2 v);            //デバッグログに追加する
		static void Log(Vector3 v);            //デバッグログに追加する
		static void Log(Vector4 v);            //デバッグログに追加する
		static void Log(bool b);               //デバッグログに追加する
	};
}