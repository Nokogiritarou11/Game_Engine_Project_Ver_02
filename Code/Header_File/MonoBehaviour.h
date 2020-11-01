#pragma once
#include <stdio.h>
#include "Behaviour.h"
#include "Mathf.h"
#include "Original_Math.h"
#include "Time.h"

class MonoBehaviour : public Behaviour
{
private:
	friend class Scene;
	friend class GameObject;

	virtual void Awake() {};
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};

	virtual void OnEnable() {};
	virtual void OnDisable() {};
	virtual void OnDestroy() {};

	void Initialize(std::shared_ptr<GameObject> obj) override;
	void SetActive(bool value) override;

	bool IsCalled_Awake = false;
	bool IsCalled_Start = false;
	bool IsCalled_Update = false;
	bool Disable_flg = false;
	void Add();

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Behaviour>(this));
	}
};
CEREAL_REGISTER_TYPE(MonoBehaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Behaviour, MonoBehaviour)