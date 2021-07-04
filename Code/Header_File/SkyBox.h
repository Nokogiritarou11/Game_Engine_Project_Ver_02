#pragma once
#include <vector>
#include <memory>
#include "DxSystem.h"
#include "Original_Math.h"

namespace BeastEngine
{
	class Mesh;
	class Material;
	class Shader;

	class SkyBox
	{
	public:
		SkyBox();

		void Render(BeastEngine::Vector3 pos);

	private:
		std::shared_ptr<BeastEngine::Material> material;
		std::shared_ptr<BeastEngine::Mesh> mesh;

		struct Constant_Buffer_Skybox
		{
			BeastEngine::Matrix world;
			BeastEngine::Vector4 color;
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_skybox;		//コンスタントバッファ
		std::shared_ptr<BeastEngine::Shader> vertex_shader;
	};
}