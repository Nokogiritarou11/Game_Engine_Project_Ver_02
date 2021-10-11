#pragma once
#include <string>

namespace BeastEngine
{
	class System_Function
	{
	public:
		static std::string Get_Open_File_Name(const std::string& extension = "", const std::string& initial_dir = "");
		static std::string Get_Save_File_Name(std::string extension = "", std::string initial_dir = "");
		static std::string Create_Id();
	};
}