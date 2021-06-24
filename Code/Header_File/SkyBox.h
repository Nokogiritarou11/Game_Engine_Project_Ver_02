#pragma once
#include "Material.h"
#include "Mesh.h"
#include <vector>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

namespace BeastEngine
{
	class SkyBox
	{
	public:
		SkyBox();

		void Render(BeastEngine::Vector3 pos);

	private:
		std::shared_ptr<BeastEngine::Material> material;
		std::shared_ptr<BeastEngine::Mesh> mesh_data;

		struct Constant_Buffer_Skybox
		{
			BeastEngine::Matrix world;
			BeastEngine::Vector4 color;
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_skybox;		//コンスタントバッファ
		std::unique_ptr<BeastEngine::Shader> vertex_shader;
	};
}