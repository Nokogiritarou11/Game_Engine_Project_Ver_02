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


namespace BeastEngine
{
	class Transform;
	class GameObject;
	class Component;

	class Object : public std::enable_shared_from_this<Object>
	{
	public:
		std::string name; //���O

		Object() {};
		virtual ~Object() {};

		std::string ToString(); //���O��Ԃ�
		static void Destroy(std::shared_ptr<BeastEngine::GameObject> obj); //�I�u�W�F�N�g���폜����
		static void Destroy(std::shared_ptr<BeastEngine::Component> comp); //�I�u�W�F�N�g���폜����

		static std::shared_ptr<BeastEngine::GameObject> Instantiate(std::string name);                                                                        //�I�u�W�F�N�g�𐶐�����
		static std::shared_ptr<BeastEngine::GameObject> Instantiate(std::string name, std::shared_ptr<BeastEngine::Transform> parent);                                     //�I�u�W�F�N�g�𐶐�����
		static std::shared_ptr<BeastEngine::GameObject> Instantiate(std::string name, std::shared_ptr<BeastEngine::Transform> parent, bool instantiateInWorldSpace);       //�I�u�W�F�N�g�𐶐�����
		static std::shared_ptr<BeastEngine::GameObject> Instantiate(std::string name, BeastEngine::Vector3 position, BeastEngine::Vector3 euler);                                       //�I�u�W�F�N�g�𐶐�����
		static std::shared_ptr<BeastEngine::GameObject> Instantiate(std::string name, BeastEngine::Vector3 position, BeastEngine::Vector4 rotation);                                    //�I�u�W�F�N�g�𐶐�����
		static std::shared_ptr<BeastEngine::GameObject> Instantiate(std::string name, BeastEngine::Vector3 position, BeastEngine::Vector3 euler, std::shared_ptr<BeastEngine::Transform> parent);    //�I�u�W�F�N�g�𐶐�����
		static std::shared_ptr<BeastEngine::GameObject> Instantiate(std::string name, BeastEngine::Vector3 position, BeastEngine::Vector4 rotation, std::shared_ptr<BeastEngine::Transform> parent); //�I�u�W�F�N�g�𐶐�����

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Object)