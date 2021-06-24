#pragma once
#include <string>

namespace BeastEngine
{
	class System_Function
	{
	public:
		static std::string Get_Open_File_Name();
		static std::string Get_Save_File_Name();
		static std::string Create_ID();
	};
}