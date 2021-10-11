#include "Mesh.h"
#include <sstream>
#include <functional>
#include <fstream>
#include <cassert>
#include "Engine.h"
#include "Asset_Manager.h"
#include "Misc.h"
using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace std;
using namespace BeastEngine;

shared_ptr<Mesh> Mesh::Load_Mesh(const string& full_path)
{
	auto it = Engine::asset_manager->cache_mesh.find(full_path);
	if (it != Engine::asset_manager->cache_mesh.end())
	{
		shared_ptr<Mesh> mesh = it->second;
		return mesh;
	}
	//ファイルから読み込み
	int path_i = full_path.find_last_of("\\") + 1;
	int ext_i = full_path.find_last_of(".");
	const string filename = full_path.substr(path_i, ext_i - path_i); //ファイル名

	if (ifstream in_bin(full_path, ios::binary); in_bin.is_open())
	{
		shared_ptr<Mesh> mesh;
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(mesh);

		mesh->name = filename;
		mesh->file_path = full_path;

		// 頂点バッファの生成
		{
			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(Mesh::vertex) * mesh->vertices.size();
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			bd.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA InitData = {};
			InitData.pSysMem = &mesh->vertices[0];
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			HRESULT hr = DxSystem::device->CreateBuffer(&bd, &InitData, mesh->vertex_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// インデックスバッファの生成
		{
			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_IMMUTABLE;
			bd.ByteWidth = sizeof(u_int) * mesh->indices.size();
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			bd.StructureByteStride = 0;
			D3D11_SUBRESOURCE_DATA InitData = {};
			InitData.pSysMem = &mesh->indices[0];
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			HRESULT hr = DxSystem::device->CreateBuffer(&bd, &InitData, mesh->index_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		Engine::asset_manager->Registration_Asset(mesh);
		Engine::asset_manager->cache_mesh.insert(make_pair(full_path, mesh));

		return mesh;
	}

	return nullptr;
}