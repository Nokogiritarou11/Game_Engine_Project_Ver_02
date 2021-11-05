#pragma once
#include <string>

namespace BeastEngine
{
	//�G���W�������Ŏg�p�����w���p�[�֐�(Windows�Ȃ�)
	class System_Function
	{
	public:
		static std::string Get_Open_File_Name(const std::string& extension = "", const std::string& initial_dir = ""); //�_�C�A���O����t�@�C���p�X���擾����
		static std::string Get_Save_File_Name(std::string extension = "", std::string initial_dir = ""); //�_�C�A���O����t�@�C���p�X���擾����
		static std::string Create_Id(); //�ŗL��UUID�𐶐�����
	};
}