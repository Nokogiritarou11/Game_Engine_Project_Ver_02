#pragma once
#include "DxSystem.h"
#include "Component.h"
#include <DirectXMath.h>
#include "Original_Math.h"
#include "Bounds.h"
#include <string>
#include <wrl.h>
#include <memory>
#include <vector>

namespace BeastEngine
{
	class Material;

	//レンダリングされるコンポーネントの基底クラス
	class Renderer : public Component
	{
	public:
		void Set_Enabled(bool value);                               //表示するか
		[[nodiscard]] bool Get_Enabled() const { return enabled; };	//現在アクティブか

		std::vector<std::shared_ptr<Material>> material; //使用するマテリアル
		std::vector<std::string> material_paths; //マテリアルのパス
		Bounds bounds; //バウンディングボックス

	protected:
		bool Draw_ImGui_Header(const std::string& component_name, bool& open); //ImGui描画時共通ヘッダー描画

		bool is_called = false;   //既にマネージャーに登録されているか
		bool enabled = true;      //描画するか
		bool enabled_old = false; //変更時トリガー用キャッシュ
		bool can_render = false;  //レンダリング可能か(データが入っているか)

		int subset_count = 0; //サブセットの数
		std::vector<int> subset_material_index; //サブセットのマテリアルインデックス
		Matrix world_old; //ワールド行列の前フレームキャッシュ

	private:
		virtual void Render(const int subset_number = 0) {}; //描画実行
		virtual void Render_Shadow(const int subset_number = 0) {}; //シャドウマップ描画実行
		virtual void Recalculate_Frame() {}; //頂点データを再計算する

		friend class Render_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled, bounds, material_paths);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Renderer, 1)