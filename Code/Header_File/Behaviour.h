#pragma once
#include "Component.h"

namespace BeastEngine
{
	class Behaviour : public BeastEngine::Component
	{
	public:
		Behaviour();

		void Set_Enabled(bool value);
		bool Get_Enabled();

	private:
		bool enabled = true;
		bool enabled_old = false;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Component>(this), enabled);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Behaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Behaviour)