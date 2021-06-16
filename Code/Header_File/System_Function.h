#pragma once
#include <stdio.h>
#include <string>
#include <Windows.h>

namespace BeastEngine
{
	class System_Function
	{
	public:
		static std::string Get_Open_File_Name();
		static std::string Get_Save_File_Name();
	};
}