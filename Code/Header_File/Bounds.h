#pragma once
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include <Original_Math.h>
#include <tchar.h>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

namespace BeastEngine
{
	struct Bounds
	{
	public:
		BeastEngine::Vector3 Get_center() { return center; }
		BeastEngine::Vector3 Get_extents() { return extents; }
		BeastEngine::Vector3 Get_min() { return min; }
		BeastEngine::Vector3 Get_max() { return max; }
		BeastEngine::Vector3 Get_size() { return size; }

		void Set_center(BeastEngine::Vector3 value);
		void Set_center(float x, float y, float z);
		void Set_size(BeastEngine::Vector3 value);
		void Set_size(float x, float y, float z);

	private:
		BeastEngine::Vector3 center;
		BeastEngine::Vector3 extents;
		BeastEngine::Vector3 min;
		BeastEngine::Vector3 max;
		BeastEngine::Vector3 size;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(center, extents, min, max, size);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Bounds, 1)