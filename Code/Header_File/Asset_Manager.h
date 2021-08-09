#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <utility>
#include "DxSystem.h"

namespace BeastEngine
{
	class Object;
	class Mesh;
	class Shader;
	class Compute_Shader;
	class Texture;
	class Material;
	class Animation_Clip;

	class Asset_Manager
	{
	public:
		void Registration_Asset(std::shared_ptr<BeastEngine::Object> obj);
		void Erase_Asset(std::string id);
		std::weak_ptr<BeastEngine::Object> Get_Asset_From_ID(std::string id);

		std::unordered_map<std::string, std::shared_ptr<BeastEngine::Mesh>> cache_mesh;
		std::unordered_map<std::string, std::shared_ptr<BeastEngine::Shader>> cache_shader;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache_compute_shader;
		std::unordered_map<std::string, std::shared_ptr<BeastEngine::Material>> cache_material;
		std::unordered_map<std::string, std::shared_ptr<BeastEngine::Texture>> cache_texture;
		std::unordered_map<std::string, std::shared_ptr<BeastEngine::Animation_Clip>> cache_clip;

		void Exit();

	private:

		std::unordered_map<std::string, std::weak_ptr<BeastEngine::Object>> cache_asset;
	};
}