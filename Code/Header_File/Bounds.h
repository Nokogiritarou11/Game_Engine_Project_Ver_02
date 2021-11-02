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

	//AABB�v�Z�p�f�[�^�N���X
	struct Bounds
	{
	public:
		[[nodiscard]] Vector3 Get_Center() const { return center; }   //���W�I�t�Z�b�g���擾����
		[[nodiscard]] Vector3 Get_Extents() const { return extents; } //���a���擾����
		[[nodiscard]] Vector3 Get_Min() const { return min; }         //�ŏ����W���擾����
		[[nodiscard]] Vector3 Get_Max() const { return max; }         //�ő���W���擾����
		[[nodiscard]] Vector3 Get_Size() const { return size; }       //�����擾����

		void Set_Center(const Vector3& value);                           //���W�I�t�Z�b�g��ݒ肷��
		void Set_Center(const float& x, const float& y, const float& z); //���W�I�t�Z�b�g��ݒ肷��
		void Set_Size(const Vector3& value);                             //AABB�̃T�C�Y��ݒ肷��
		void Set_Size(const float& x, const  float& y, const float& z);  //AABB�̃T�C�Y��ݒ肷��

		[[nodiscard]] bool Get_Is_Culling_Frustum(const std::shared_ptr<Transform>& trans, const  std::array<Vector4, 6>& planes) const; //������J�����O����邩

	private:
		Vector3 center;  //���S�I�t�Z�b�g
		Vector3 extents; //���a
		Vector3 min;     //�ŏ����W
		Vector3 max;     //�ő���W
		Vector3 size;    //��

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(center, extents, min, max, size);
		}
	};
}

CEREAL_CLASS_VERSION(BeastEngine::Bounds, 1)