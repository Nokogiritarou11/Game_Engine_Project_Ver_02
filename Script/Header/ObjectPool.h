#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
using namespace DirectX;

class ObjectPool : public  MonoBehaviour
{
public:

	void Start();

	std::weak_ptr<GameObject> Instance_inPool(int Number, Vector3 pos, Quaternion rot);

private:

	std::vector<int> maxCount = {15};
	std::vector<std::list<std::weak_ptr<GameObject>>> _poolObj = {};

	void CreatePool();
	std::weak_ptr<GameObject> Get_Object(int Number);
};