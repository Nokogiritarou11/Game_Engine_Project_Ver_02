#pragma once
#include <unordered_map>
#include <memory>
#include <string>
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
		void Registration_Asset(std::shared_ptr<Object> obj);
		void Erase_Asset(const std::string& id);
		std::weak_ptr<Object> Get_Asset_From_ID(const std::string& id);

		std::unordered_map<std::string, std::shared_ptr<Mesh>> cache_mesh {};
		std::unordered_map<std::string, std::shared_ptr<Shader>> cache_shader {};
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache_compute_shader {};
		std::unordered_map<std::string, std::shared_ptr<Material>> cache_material {};
		std::unordered_map<std::string, std::shared_ptr<Texture>> cache_texture {};
		std::unordered_map<std::string, std::shared_ptr<Animation_Clip>> cache_clip {};

		void Exit();

	private:

		std::unordered_map<std::string, std::weak_ptr<Object>> cache_asset {};
	};
}