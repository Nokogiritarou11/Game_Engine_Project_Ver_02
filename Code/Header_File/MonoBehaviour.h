#pragma once
#include <stdio.h>
#include "Behaviour.h"
#include "Input_Manager.h"
#include "Audio_Manager.h"
#include <SimpleMath.h>
#include "Time.h"

class MonoBehaviour : public Behavior
{
public:
	bool Start_Flag = false;

	MonoBehaviour();
	~MonoBehaviour();

	virtual void Awake() {};
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};

	virtual void OnEnable() {};
	virtual void OnDisable() {};
	virtual void OnDestroy() {};

	void Initialize(std::shared_ptr<GameObject> obj);
private:

};