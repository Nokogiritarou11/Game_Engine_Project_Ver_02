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
	std::string name; //名前

	Object(){};
	virtual ~Object(){};

	std::string ToString(); //名前を返す
	static void Destroy(std::shared_ptr<GameObject> obj); //オブジェクトを削除する
	static void Destroy(std::shared_ptr<Component> comp); //オブジェクトを削除する

	static std::shared_ptr<GameObject> Instantiate(std::string name);                                                                        //オブジェクトを生成する
	static std::shared_ptr<GameObject> Instantiate(std::string name, std::shared_ptr<Transform> parent);                                     //オブジェクトを生成する
	static std::shared_ptr<GameObject> Instantiate(std::string name, std::shared_ptr<Transform> parent, bool instantiateInWorldSpace);       //オブジェクトを生成する
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector3 euler);                                       //オブジェクトを生成する
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector4 rotation);                                    //オブジェクトを生成する
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector3 euler, std::shared_ptr<Transform> parent);    //オブジェクトを生成する
	static std::shared_ptr<GameObject> Instantiate(std::string name, Vector3 position, Vector4 rotation, std::shared_ptr<Transform> parent); //オブジェクトを生成する

private:
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(name);
	}
};

CEREAL_REGISTER_TYPE(Object)