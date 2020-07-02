#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
using namespace DirectX;
using namespace std;

class ObjectPool : public  MonoBehaviour
{
public:

	void Start();

	weak_ptr<GameObject> Instance_inPool(int Number, Vector3 pos, Quaternion rot);

private:

	vector<int> maxCount = {15};
	vector<list<weak_ptr<GameObject>>> _poolObj = {};

	void CreatePool();
	weak_ptr<GameObject> Get_Object(int Number);
};