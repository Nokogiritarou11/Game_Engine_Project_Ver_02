#pragma once
#include <stdio.h>
#include <string>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/memory.hpp"

class Transform;
class GameObject;
class Component;

class Object : public std::enable_shared_from_this<Object>
{
public:
	std::string name;

	Object(){};
	virtual ~Object(){};

	std::string ToString();
	static void Destroy(std::shared_ptr<GameObject> obj);
	static void Destroy(std::shared_ptr<Component> comp);

	static std::shared_ptr<GameObject> Instantiate(std::string name);
	static std::shared_ptr<GameObject> Instantiate(std::string name, std::shared_ptr<Transform> parent);
	static std::shared_ptr<GameObject> Instantiate(std::string name, std::shared_ptr<Transform> parent, bool instantiateInWorldSpace);
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector3 euler);
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector4 rotation);
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector3 euler, std::shared_ptr<Transform> parent);
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector4 rotation, std::shared_ptr<Transform> parent);

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(name);
	}
};

CEREAL_REGISTER_TYPE(Object)