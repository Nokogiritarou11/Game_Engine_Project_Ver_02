#pragma once
#include <stdio.h>
#include <string>
#include <list>
#include <memory>
#include "GameObject.h"

class Scene
{
public:

	virtual void Initialize() {};
	std::shared_ptr<GameObject> Instance_GameObject(std::string name);
	void Destroy_GameObject(std::shared_ptr<GameObject> gameObject);
	void Update();
	void Reset();

	std::weak_ptr<GameObject> Find(std::string Name);
	std::weak_ptr<GameObject> FindWithTag(std::string Tag);

private:

	std::list<std::shared_ptr<GameObject>> gameObject_List;
};

/////////////////////////////////////////////////////////////////////////

class Game_01_Scene : public Scene
{
public:
	void Initialize();

private:
};
class Game_02_Scene : public Scene
{
public:
	void Initialize();

private:
};
class Game_03_Scene : public Scene
{
public:
	void Initialize();

private:
};

/////////////////////////////////////////////////////////////////////////

class Title_Scene : public Scene
{
public:
	void Initialize();

private:
};
