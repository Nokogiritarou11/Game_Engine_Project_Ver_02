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

	class Renderer : public Component
	{
	public:
		void Set_Enabled(bool value); //表示するか
		[[nodiscard]] bool Get_Enabled() const { return enabled; };			 //現在アクティブか

		std::vector<std::shared_ptr<Material>> material; //使用するマテリアル
		Bounds bounds;

	protected:
		bool Draw_ImGui_Header(const std::string& component_name, bool& open);

		bool is_called = false;
		bool enabled = true;
		bool enabled_old = false;
		bool can_render = false;

		int subset_count = 0;
		std::vector<int> subset_material_index;
		Matrix world_old;

	private:
		virtual void Render(const int subset_number = 0) {};
		virtual void Render_Shadow(const int subset_number = 0) {};
		virtual void Recalculate_Frame() {};

		friend class Render_Manager;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Component>(this), enabled, bounds);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Component, BeastEngine::Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Renderer, 1)