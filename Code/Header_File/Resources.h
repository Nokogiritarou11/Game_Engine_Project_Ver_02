#pragma once
#include <memory>
#include <string>

namespace BeastEngine
{
	class GameObject;

	class Resources
	{
	public:
		static void Create_Prefab(std::shared_ptr<GameObject> gameobject); //プレハブを保存する。生成するパスは固定(Assets\\Prefab)
		static std::shared_ptr<GameObject> Load_Prefab(const std::string& file_path);

	private:
		static void Instance_child(std::shared_ptr<GameObject> gameobject);
	};
}