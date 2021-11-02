#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Object.h"

namespace BeastEngine
{
	class Transform;
	class Fbx_Converter;

	//�A�j���[�V�����N���b�v�f�[�^
	class Animation_Clip final : public Object
	{
	public:
		//�L�[�t���[���f�[�^
		struct Keyframe
		{
			float time;
			Vector3	scale;
			Quaternion rotation;
			Vector3	position;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(time, scale, rotation, position);
			}
		};

		//�Ώ�(Transform)�ւ̃A�j���[�V�����f�[�^
		struct Animation
		{
			std::string target_path;
			std::vector<Keyframe> keys;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(target_path, keys);
			}
		};

		std::vector<Animation> animations;

		[[nodiscard]] float Get_Length() const { return length; } //�N���b�v�̒���(�b)���擾����
		[[nodiscard]] int Get_Frame_Count() const { return frame_count; } //�N���b�v�̃t���[�������擾����

		static std::shared_ptr<Animation_Clip> Load_Clip(const std::string& full_path); //�t�@�C���p�X���w�肵�ăN���b�v��ǂݍ���

	private:
		float length; //�t���[���̒���(�b)
		int frame_count; //�N���b�v�̃t���[����

		friend class Fbx_Converter;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this), animations, length, frame_count);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animation_Clip)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animation_Clip)
CEREAL_CLASS_VERSION(BeastEngine::Animation_Clip, 1)
CEREAL_CLASS_VERSION(BeastEngine::Animation_Clip::Keyframe, 1)
CEREAL_CLASS_VERSION(BeastEngine::Animation_Clip::Animation, 1)