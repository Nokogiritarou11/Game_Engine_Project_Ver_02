#pragma once
#include <string>

#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/types/array.hpp"

namespace BeastEngine
{
	//シーン内設定
	struct Project_Settings
	{
		float shadow_bias = 0.01f;         //シェーダー内で使用するシャドウバイアス
		float shadow_distance = 15.0f;     //カメラから見て影を描画する距離
		std::vector<std::string> tag{};    //ゲームオブジェクトに設定できるタグ
		std::array<std::string, 32> layer; //衝突判定などに使用できるレイヤー
		std::array<int, 32> layer_mask;    //レイヤーごとの衝突判定設定

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