#pragma once
#include <memory>
#include <string>

namespace BeastEngine
{
	class GameObject;

	class Resources
	{
	public:
		static void Create_Prefab(std::shared_ptr<GameObject> gameobject); //�v���n�u��ۑ�����B��������p�X�͌Œ�(Assets\\Prefab)
		static std::shared_ptr<GameObject> Load_Prefab(const std::string& file_path);

	private:
		static void Instance_child(std::shared_ptr<GameObject> gameobject);
	};
}