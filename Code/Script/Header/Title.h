#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
using namespace DirectX;
using namespace std;

class Title : public MonoBehaviour
{
public:
	void Start();
	void Update();
private:
	weak_ptr<GameObject> title;
	weak_ptr<GameObject> text;
	weak_ptr<GameObject> explain;
	float timer = 0;
	int state = 0;
};