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

	//AABB計算用データクラス
	struct Bounds
	{
	public:
		[[nodiscard]] Vector3 Get_Center() const { return center; }   //座標オフセットを取得する
		[[nodiscard]] Vector3 Get_Extents() const { return extents; } //半径を取得する
		[[nodiscard]] Vector3 Get_Min() const { return min; }         //最小座標を取得する
		[[nodiscard]] Vector3 Get_Max() const { return max; }         //最大座標を取得する
		[[nodiscard]] Vector3 Get_Size() const { return size; }       //幅を取得する

		void Set_Center(const Vector3& value);                           //座標オフセットを設定する
		void Set_Center(const float& x, const float& y, const float& z); //座標オフセットを設定する
		void Set_Size(const Vector3& value);                             //AABBのサイズを設定する
		void Set_Size(const float& x, const  float& y, const float& z);  //AABBのサイズを設定する

		[[nodiscard]] bool Get_Is_Culling_Frustum(const std::shared_ptr<Transform>& trans, const  std::array<Vector4, 6>& planes) const; //視錐台カリングされるか

	private:
		Vector3 center;  //中心オフセット
		Vector3 extents; //半径
		Vector3 min;     //最小座標
		Vector3 max;     //最大座標
		Vector3 size;    //幅

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(center, extents, min, max, size);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Bounds, 1)