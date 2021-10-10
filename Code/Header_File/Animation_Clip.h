#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Object.h"

namespace BeastEngine
{
	class Transform;
	class FBX_Converter;

	class Animation_Clip final : public Object
	{
	public:
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

		[[nodiscard]] float Get_Length() const { return length; }
		[[nodiscard]] int Get_Frame_Count() const { return frame_count; }

		static std::shared_ptr<Animation_Clip> Load_Clip(const std::string& full_path);

	private:
		float length;
		int frame_count;

		friend class FBX_Converter;
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