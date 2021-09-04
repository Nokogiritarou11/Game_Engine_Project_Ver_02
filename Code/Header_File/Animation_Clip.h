#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Object.h"

namespace BeastEngine
{
	class Transform;
	class FBX_Converter;

	class Animation_Clip : public BeastEngine::Object
	{
	public:
		struct Keyframe
		{
			float time;
			BeastEngine::Vector3	scale;
			BeastEngine::Quaternion rotation;
			BeastEngine::Vector3	position;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(time, scale, rotation, position);
			}
		};

		struct Animation
		{
			std::string Target_Path;
			std::vector<Keyframe> keys;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(Target_Path, keys);
			}
		};

		std::vector<Animation> animations;

		float Get_Length() { return length; }
		int Get_Frame_Count() { return frame_count; }

		static std::shared_ptr<Animation_Clip> Load_Clip(std::string fullpath);

	private:
		float length;
		int frame_count;

		friend class BeastEngine::FBX_Converter;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), animations, length, frame_count);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Animation_Clip)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Animation_Clip)
CEREAL_CLASS_VERSION(BeastEngine::Animation_Clip, 1)
CEREAL_CLASS_VERSION(BeastEngine::Animation_Clip::Keyframe, 1)
CEREAL_CLASS_VERSION(BeastEngine::Animation_Clip::Animation, 1)