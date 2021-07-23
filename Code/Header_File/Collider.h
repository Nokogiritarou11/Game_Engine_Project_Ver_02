#pragma once
#include "Component.h"

namespace BeastEngine
{
	class Collider :public BeastEngine::Component
	{
	public:
		void Set_Enabled(bool value); //表示するか
		bool Get_Enabled();			 //現在アクティブか

		bool is_trigger = false;
	private:
		bool enabled = true;
		bool enabled_old = false;

		bool Can_Multiple() override { return false; };

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Component>(this), enabled);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Collider)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Collider)
CEREAL_CLASS_VERSION(BeastEngine::Collider, 1)