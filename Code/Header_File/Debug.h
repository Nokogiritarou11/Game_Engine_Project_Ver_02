#pragma once
#include <stdlib.h>
#include <string>
#include "Original_Math.h"

class Debug
{
public:
	static void Log();              //�f�o�b�O���O�ɒǉ�����
	static void Log(std::string s); //�f�o�b�O���O�ɒǉ�����
	static void Log(char* c);       //�f�o�b�O���O�ɒǉ�����
	static void Log(int i);         //�f�o�b�O���O�ɒǉ�����
	static void Log(float f);       //�f�o�b�O���O�ɒǉ�����
	static void Log(double d);      //�f�o�b�O���O�ɒǉ�����
	static void Log(Vector2 v);     //�f�o�b�O���O�ɒǉ�����
	static void Log(Vector3 v);     //�f�o�b�O���O�ɒǉ�����
	static void Log(Vector4 v);     //�f�o�b�O���O�ɒǉ�����
	static void Log(bool b);        //�f�o�b�O���O�ɒǉ�����
private:

};