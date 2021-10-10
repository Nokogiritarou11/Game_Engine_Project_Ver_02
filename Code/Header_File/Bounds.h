#pragma once
#include <unordered_map>
#include <array>
#include <wrl.h>
#include <Original_Math.h>
#include <tchar.h>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/types/memory.hpp"

namespace BeastEngine
{
	class Transform;

	struct Bounds
	{
	public:
		[[nodiscard]] Vector3 Get_Center() const { return center; }
		[[nodiscard]] Vector3 Get_Extents() const { return extents; }
		[[nodiscard]] Vector3 Get_Min() const { return min; }
		[[nodiscard]] Vector3 Get_Max() const { return max; }
		[[nodiscard]] Vector3 Get_Size() const { return size; }

		void Set_Center(const Vector3& value);
		auto Set_Center(const float& x, const float& y, const float& z) -> void;
		void Set_Size(const Vector3& value);
		void Set_Size(const float& x, const  float& y, const float& z);

		bool Get_Is_Culling_Frustum(const std::shared_ptr<Transform>& trans, const  std::array<Vector4, 6>& planes) const;

	private:
		Vector3 center;
		Vector3 extents;
		Vector3 min;
		Vector3 max;
		Vector3 size;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(center, extents, min, max, size);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Bounds, 1)