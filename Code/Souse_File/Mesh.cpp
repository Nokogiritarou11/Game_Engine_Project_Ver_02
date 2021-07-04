#include "Mesh.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "Engine.h"
#include "Asset_Manager.h"
using namespace std;
using namespace BeastEngine;

unordered_map<std::string, std::shared_ptr<BeastEngine::Mesh>> Mesh::cache_mesh;

shared_ptr<Mesh> Mesh::Load_Mesh(string fullpath)
{
	auto it = cache_mesh.find(fullpath);
	if (it != cache_mesh.end())
	{
		shared_ptr<Mesh> mesh = it->second;
		return mesh;
	}
	else //ファイルから読み込み
	{
		int path_i = fullpath.find_last_of("\\") + 1;//7
		int ext_i = fullpath.find_last_of(".");//10
		string pathname = fullpath.substr(0, path_i); //ファイルまでのディレクトリ
		string filename = fullpath.substr(path_i, ext_i - path_i); //ファイル名
		shared_ptr<Mesh> mesh = make_shared<Mesh>();
		mesh->name = filename;
		mesh->file_path = pathname;

		const string bin = fullpath + ".mesh";
		ifstream in_bin(bin, ios::binary);
		if (in_bin.is_open())
		{
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(mesh);

			mesh->name = (string)filename;
			mesh->file_path = (string)pathname;

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
				InitData.pSysMem = &mesh->vertices[0]; // 頂点のアドレス
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;
				HRESULT hr = DxSystem::device->CreateBuffer(&bd, &InitData, mesh->vertex_buffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}

			// インデックスバッファの生成
			{
				D3D11_BUFFER_DESC bd = {};
				//bd.Usage                      = D3D11_USAGE_DEFAULT;
				bd.Usage = D3D11_USAGE_IMMUTABLE;
				bd.ByteWidth = sizeof(u_int) * mesh->indices.size();
				//bd.ByteWidth = sizeof(u_int) * inverse_indices.size();
				bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
				bd.CPUAccessFlags = 0;
				bd.MiscFlags = 0;
				bd.StructureByteStride = 0;
				D3D11_SUBRESOURCE_DATA InitData = {};
				InitData.pSysMem = &mesh->indices[0];				// 頂点のアドレス
				//InitData.pSysMem = &inverse_indices[0];				// 頂点のアドレス
				InitData.SysMemPitch = 0;
				InitData.SysMemSlicePitch = 0;
				HRESULT hr = DxSystem::device->CreateBuffer(&bd, &InitData, mesh->index_buffer.GetAddressOf());
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}

			Engine::asset_manager->Registration_Asset(mesh);
			cache_mesh.insert(make_pair(fullpath, mesh));

			return mesh;
		}
	}

	return nullptr;
}