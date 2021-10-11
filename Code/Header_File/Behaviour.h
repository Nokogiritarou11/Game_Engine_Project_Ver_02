#pragma once
#include "Component.h"

namespace BeastEngine
{
	class Behaviour : public Component
	{
	public:
		Behaviour();

		void Set_Enabled(bool value);
		[[nodiscard]] bool Get_Enabled() const;

	protected:
		bool Draw_ImGui_Header(const std::string& component_name, bool& open);

	private:
		bool enabled = true;
		bool enabled_old = false;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Behaviour)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Behaviour)
CEREAL_CLASS_VERSION(BeastEngine::Behaviour, 1)