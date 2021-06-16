#pragma once
#include <memory>
#include <string>

namespace BeastEngine
{
	class GameObject;

	class Resources
	{
	public:
		static void Create_Prefab(std::shared_ptr<BeastEngine::GameObject> gameobject); //�v���n�u��ۑ�����B��������p�X�͌Œ�(Resouces\\Prefab)
		static std::shared_ptr<BeastEngine::GameObject> Load_Prefab(std::string file_path);

	private:
		static void Instance_child(std::shared_ptr<BeastEngine::GameObject> gameobject);
	};
}