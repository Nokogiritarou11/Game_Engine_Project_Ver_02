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
#include "cereal/types/array.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

namespace BeastEngine
{
	struct Project_Settings
	{
	public:
		float shadow_bias = 0.01f;
		float shadow_distance = 15.0f;
		std::vector<std::string> tag;
		std::array<std::string, 32> layer;
		std::array<int, 32> layer_mask;

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(shadow_bias, shadow_distance, tag, layer, layer_mask);
		}

	};
}

CEREAL_CLASS_VERSION(BeastEngine::Project_Settings, 1)