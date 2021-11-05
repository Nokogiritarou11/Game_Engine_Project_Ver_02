#pragma once
#include <string>

namespace BeastEngine
{
	//エンジン内部で使用されるヘルパー関数(Windowsなど)
	class System_Function
	{
	public:
		static std::string Get_Open_File_Name(const std::string& extension = "", const std::string& initial_dir = ""); //ダイアログからファイルパスを取得する
		static std::string Get_Save_File_Name(std::string extension = "", std::string initial_dir = ""); //ダイアログからファイルパスを取得する
		static std::string Create_Id(); //固有なUUIDを生成する
	};
}