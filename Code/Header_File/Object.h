#pragma once
#include <string>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <memory>

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/utility.hpp"
#include "cereal/types/polymorphic.hpp"
#include "cereal/types/base_class.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/unordered_map.hpp"


namespace BeastEngine
{
	class Transform;
	class GameObject;
	class Component;

	//�G���W�����Ŏg�p�����I�u�W�F�N�g�̊��N���X
	class Object : public std::enable_shared_from_this<Object>
	{
	public:
		std::string name; //���O

		Object();
		Object(const Object& obj);
		virtual ~Object();

		[[nodiscard]] std::string To_String() const { return name; }; //���O��Ԃ�
		[[nodiscard]] std::string Get_Instance_Id() const { return instance_id; }; //���O��Ԃ�
		static void Destroy(std::shared_ptr<GameObject> obj); //�I�u�W�F�N�g���폜����
		static void Destroy(std::shared_ptr<Component> comp); //�I�u�W�F�N�g���폜����

		static std::shared_ptr<Object> Instantiate(std::weak_ptr<Object> original); //�I�u�W�F�N�g�𐶐�����

	private:
		std::string instance_id; //�C���X�^���X���ɐ�������郆�j�[�NID

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(name);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Object)
CEREAL_CLASS_VERSION(BeastEngine::Object, 1)