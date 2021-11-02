#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "Object.h"

namespace BeastEngine
{
	class Transform;
	class Fbx_Converter;

	//アニメーションクリップデータ
	class Animation_Clip final : public Object
	{
	public:
		//キーフレームデータ
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

		//対象(Transform)へのアニメーションデータ
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

		[[nodiscard]] float Get_Length() const { return length; } //クリップの長さ(秒)を取得する
		[[nodiscard]] int Get_Frame_Count() const { return frame_count; } //クリップのフレーム数を取得する

		static std::shared_ptr<Animation_Clip> Load_Clip(const std::string& full_path); //ファイルパスを指定してクリップを読み込む

	private:
		float length; //フレームの長さ(秒)
		int frame_count; //クリップのフレーム数

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