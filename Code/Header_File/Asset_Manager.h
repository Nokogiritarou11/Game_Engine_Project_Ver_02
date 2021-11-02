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

	//Object継承クラスやアセットのキャッシュを管理するクラス
	class Asset_Manager
	{
	public:
		void Registration_Asset(std::shared_ptr<Object> obj);           //ObjectをIDと関連付けてリストに登録する
		void Erase_Asset(const std::string& id);                        //リストから削除する
		std::weak_ptr<Object> Get_Asset_From_Id(const std::string& id); //instance_idから登録されているObjectを取得する

		std::unordered_map<std::string, std::shared_ptr<Mesh>> cache_mesh {};                                 //メッシュデータのキャッシュ
		std::unordered_map<std::string, std::shared_ptr<Shader>> cache_shader {};                             //シェーダーのキャッシュ
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ComputeShader>> cache_compute_shader {}; //コンピュートシェーダーのキャッシュ
		std::unordered_map<std::string, std::shared_ptr<Material>> cache_material {};                         //マテリアルのキャッシュ
		std::unordered_map<std::string, std::shared_ptr<Texture>> cache_texture {};                           //テクスチャデータのキャッシュ
		std::unordered_map<std::string, std::shared_ptr<Animation_Clip>> cache_clip {};                       //アニメーションクリップのキャッシュ

		void Exit(); //後始末

	private:
		std::unordered_map<std::string, std::weak_ptr<Object>> cache_asset {};
	};
}