#pragma once
#include <string>
#include "Original_Math.h"

namespace BeastEngine
{
	//�f�o�b�O�\��
	class Debug
	{
	public:
		static void Log();                     //�f�o�b�O���O�ɒǉ�����
		static void Log(const std::string& s); //�f�o�b�O���O�ɒǉ�����
		static void Log(const char* c);        //�f�o�b�O���O�ɒǉ�����
		static void Log(int i);                //�f�o�b�O���O�ɒǉ�����
		static void Log(float f);              //�f�o�b�O���O�ɒǉ�����
		static void Log(double d);             //�f�o�b�O���O�ɒǉ�����
		static void Log(Vector2 v);            //�f�o�b�O���O�ɒǉ�����
		static void Log(Vector3 v);            //�f�o�b�O���O�ɒǉ�����
		static void Log(Vector4 v);            //�f�o�b�O���O�ɒǉ�����
		static void Log(bool b);               //�f�o�b�O���O�ɒǉ�����
	};
}