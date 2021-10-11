#pragma once
#include "MonoBehaviour.h"

namespace BeastEngine
{
	class Object_Pool final : public MonoBehaviour
	{
	public:
		std::shared_ptr<GameObject> Instance_In_Pool(const std::string& key, Vector3 position, Quaternion rotation);

	private:
		void Start() override;
		bool Draw_ImGui() override;

		std::shared_ptr<GameObject> Get_Object(const std::string& key);

		struct Pool_Object
		{
			std::string instance_key;
			std::string obj_path;
			int instance_counts;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(instance_key, obj_path, instance_counts);
			}
		};

		std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> instance_pool;
		std::vector<Pool_Object> pool_list{};

		// シリアライズ関数
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<MonoBehaviour>(this), pool_list);
		}
	};
}

REGISTER_COMPONENT(Object_Pool)
CEREAL_REGISTER_TYPE(BeastEngine::Object_Pool)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::MonoBehaviour, BeastEngine::Object_Pool)
CEREAL_CLASS_VERSION(BeastEngine::Object_Pool, 1)