#pragma once
#include <stdio.h>
#include "Behaviour.h"
#include "Mathf.h"
#include "Original_Math.h"
#include "Time.h"

class MonoBehaviour : public Behavior
{
public:

	MonoBehaviour();
	~MonoBehaviour();

	virtual void Awake() {};
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};

	virtual void OnEnable() {};
	virtual void OnDisable() {};
	virtual void OnDestroy() {};

	void Initialize();
	void Initialize(std::shared_ptr<GameObject> obj);

private:
};