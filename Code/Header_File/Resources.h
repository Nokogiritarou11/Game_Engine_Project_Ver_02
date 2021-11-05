#pragma once
#include <memory>
#include <string>

namespace BeastEngine
{
	class GameObject;

	//�A�Z�b�g�f�[�^�w���p�[�֐�
	class Resources
	{
	public:
		static void Create_Prefab(std::shared_ptr<GameObject> gameobject); //�v���n�u��ۑ����� ������̃p�X�͌Œ�(Assets\\Prefab)
		static std::shared_ptr<GameObject> Load_Prefab(const std::string& file_path); //�t�@�C���p�X����v���n�u��ǂݍ��݃V�[���ɔz�u����

	private:
		static void Instance_Child(std::shared_ptr<GameObject> gameobject);
	};
}