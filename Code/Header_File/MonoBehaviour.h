#pragma once
#include <stdio.h>
#include "Behaviour.h"
#include "Mathf.h"
#include "Original_Math.h"
#include "Time.h"

class MonoBehaviour : public Behaviour
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
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Behaviour>(this));
	}
};
CEREAL_REGISTER_TYPE(MonoBehaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Behaviour, MonoBehaviour)