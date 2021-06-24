#pragma once
#include "DxSystem.h"
#include "Component.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Original_Math.h"
#include "Bounds.h"
#include <string>
#include <wrl.h>
#include <memory>
#include <vector>
#include <stdio.h>
#include <locale.h>

namespace BeastEngine
{
	class Material;

	class Renderer : public BeastEngine::Component
	{
	public:
		void Set_Enabled(bool value); //表示するか
		bool Get_Enabled();			 //現在アクティブか

		std::vector<std::shared_ptr<BeastEngine::Material>> material; //使用するマテリアル
		std::vector<Bounds> bounds;

	protected:
		bool is_called = false;
		bool is_disable = false;
		bool enabled = true;
		bool enabled_old = false;
		bool recalculated_frame = false;

		BeastEngine::Matrix world_old;

		static BS_State binding_blend_state;
		static RS_State binding_rasterizer_state;
		static DS_State binding_depth_stencil_State;

	private:
		virtual void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) {};
		virtual void Render_Shadow(BeastEngine::Matrix V, BeastEngine::Matrix P) {};

		friend class View_Texture;
		friend class Render_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Component>(this), enabled);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Renderer)