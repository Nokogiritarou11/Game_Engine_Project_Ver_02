#pragma once
#include "GameObject.h"

class Resources
{
public:
	static void Create_Prefab(std::shared_ptr<GameObject> gameobject); //�v���n�u��ۑ�����B��������p�X�͌Œ�(Resouces\\Prefab)
	static std::shared_ptr<GameObject> Load_Prefab(std::string file_path);

private:
	static void Instance_child(std::shared_ptr<GameObject> gameobject);
};