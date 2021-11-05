#pragma once
#include <memory>
#include <string>

namespace BeastEngine
{
	class GameObject;

	//アセットデータヘルパー関数
	class Resources
	{
	public:
		static void Create_Prefab(std::shared_ptr<GameObject> gameobject); //プレハブを保存する 生成先のパスは固定(Assets\\Prefab)
		static std::shared_ptr<GameObject> Load_Prefab(const std::string& file_path); //ファイルパスからプレハブを読み込みシーンに配置する

	private:
		static void Instance_Child(std::shared_ptr<GameObject> gameobject);
	};
}