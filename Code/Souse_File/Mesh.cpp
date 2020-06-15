#include "Mesh.h"
#include <locale.h>
#include <sstream>
#include <WICTextureLoader.h>
#include <d3dcompiler.h>
#include <functional>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "DxSystem.h"
using namespace std;
using namespace DirectX;

shared_ptr<Mesh> Mesh::Load_Mesh(const char* file_pass, const char* fbx_filename)
{
	shared_ptr<Mesh> mesh_ptr = make_shared<Mesh>();
	mesh_ptr->name = (string)fbx_filename;

	static unordered_map<string, Mesh::meshes> cache;
	string fullpass = (string)file_pass + (string)fbx_filename;

	auto it = cache.find(fullpass);
	if (it != cache.end())
	{
		mesh_ptr->skin_meshes = it->second;
		return mesh_ptr;
	}
	else
		//ファイルから読み込み
	{
		Mesh::meshes skin_meshes;

		ifstream in_f(fullpass);
		assert(in_f.is_open(), "File not found");

		string _s;
		getline(in_f, _s);
		int mesh_size = stoi(_s);
		skin_meshes.resize(mesh_size);
		for (int i = 0;i < mesh_size;i++)
		{
			vector<Mesh::vertex> vertices; // Vertex buffer
			vector<u_int> indices; // Index buffer

			getline(in_f, _s);
			int vertex_size = stoi(_s);
			vertices.resize(vertex_size);
			for (int j = 0;j < vertex_size;j++)
			{
				in_f >> vertices[j].position.x >> vertices[j].position.y >> vertices[j].position.z;
				in_f.ignore();

				in_f >> vertices[j].normal.x >> vertices[j].normal.y >> vertices[j].normal.z;
				in_f.ignore();

				in_f >> vertices[j].texcoord.x >> vertices[j].texcoord.y;
				in_f.ignore();

				for (int k = 0;k < MAX_BONE_INFLUENCES;k++)
				{
					if (k < MAX_BONE_INFLUENCES - 1)
					{
						getline(in_f, _s, ' ');
					}
					else
					{
						getline(in_f, _s);
					}
					vertices[j].bone_weights[k] = stof(_s);
				}

				for (int k = 0;k < MAX_BONE_INFLUENCES;k++)
				{
					if (k < MAX_BONE_INFLUENCES - 1)
					{
						getline(in_f, _s, ' ');
					}
					else
					{
						getline(in_f, _s);
					}
					vertices[j].bone_indices[k] = stoi(_s);
				}
			}

			getline(in_f, _s);
			int index_size = stoi(_s);
			indices.resize(index_size);
			for (int j = 0;j < index_size;j++)
			{
				if (j < index_size - 1)
				{
					getline(in_f, _s, ' ');
				}
				else
				{
					getline(in_f, _s);
				}
				indices[j] = stoi(_s);
			}

			getline(in_f, _s);
			int subset_size = stoi(_s);
			skin_meshes[i].subsets.resize(subset_size);
			for (int j = 0;j < subset_size;j++)
			{
				getline(in_f, _s);
				skin_meshes[i].subsets[j].index_start = stoi(_s);

				getline(in_f, _s);
				skin_meshes[i].subsets[j].index_count = stoi(_s);

				in_f >> skin_meshes[i].subsets[j].diffuse.color.x >> skin_meshes[i].subsets[j].diffuse.color.y >> skin_meshes[i].subsets[j].diffuse.color.z >> skin_meshes[i].subsets[j].diffuse.color.w;
				in_f.ignore();

				//シェーダー読んでるところ
				getline(in_f, _s);

				getline(in_f, _s);
				if (_s != "")
				{
					string Texpass = (string)file_pass + _s;
					setlocale(LC_ALL, "japanese");
					wchar_t TexPass[MAX_PATH] = { 0 };
					size_t ret = 0;
					mbstowcs_s(&ret, TexPass, MAX_PATH, Texpass.c_str(), _TRUNCATE);
					skin_meshes[i].subsets[j].diffuse.TexPass = TexPass;
					skin_meshes[i].subsets[j].diffuse.TexName = _s;
				}
				else
				{
					string Texpass = "Default_Resource\\Image\\Default_Texture.png";
					setlocale(LC_ALL, "japanese");
					wchar_t TexPass[MAX_PATH] = { 0 };
					size_t ret = 0;
					mbstowcs_s(&ret, TexPass, MAX_PATH, Texpass.c_str(), _TRUNCATE);
					skin_meshes[i].subsets[j].diffuse.TexPass = TexPass;
					skin_meshes[i].subsets[j].diffuse.TexName = _s;
				}
			}

			for (int row = 0; row < 4; row++)
			{
				for (int column = 0; column < 4; column++)
				{
					if (row == 3 && column == 3)
					{
						getline(in_f, _s);
					}
					else
					{
						getline(in_f, _s, ' ');
					}
					skin_meshes[i].global_transform.m[row][column] = stof(_s);
				}
			}

			getline(in_f, _s);
			int skeletal_animation_size = stoi(_s);
			skin_meshes[i].skeletal_animation.resize(skeletal_animation_size);
			for (int j = 0;j < skeletal_animation_size;j++)
			{
				getline(in_f, _s);
				int skeletal_size = stoi(_s);
				skin_meshes[i].skeletal_animation[j].resize(skeletal_size);
				getline(in_f, _s);
				skin_meshes[i].skeletal_animation[j].sampling_time = stof(_s);
				for (int k = 0;k < skeletal_size;k++)
				{
					getline(in_f, _s);
					int bone_size = stoi(_s);
					skin_meshes[i].skeletal_animation[j].at(k).resize(bone_size);
					for (int l = 0; l < bone_size; l++)
					{
						for (int row = 0; row < 4; row++)
						{
							for (int column = 0; column < 4; column++)
							{
								if (row == 3 && column == 3)
								{
									getline(in_f, _s);
								}
								else
								{
									getline(in_f, _s, ' ');
								}
								skin_meshes[i].skeletal_animation[j].at(k).at(l).transform.m[row][column] = stof(_s);;
							}
						}
					}
				}
			}

			// 頂点バッファの生成
			{
				D3D11_BUFFER_DESC bd            = {};
				bd.Usage                        = D3D11_USAGE_DEFAULT;
				bd.ByteWidth                    = sizeof(Mesh::vertex) * vertices.size();
				bd.BindFlags                    = D3D11_BIND_VERTEX_BUFFER;
				bd.CPUAccessFlags               = 0;
				bd.MiscFlags                    = 0;
				bd.StructureByteStride          = 0;
				D3D11_SUBRESOURCE_DATA InitData = {};
				InitData.pSysMem                = &vertices[0];				// 頂点のアドレス
				InitData.SysMemPitch            = 0;
				InitData.SysMemSlicePitch       = 0;
				HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, skin_meshes[i].vertex_buffer.GetAddressOf());
				assert(SUCCEEDED(hr), hr_trace(hr));
			}

			// インデックスバッファの生成
			{
				D3D11_BUFFER_DESC bd            = {};
				//bd.Usage                      = D3D11_USAGE_DEFAULT;
				bd.Usage                        = D3D11_USAGE_IMMUTABLE;
				bd.ByteWidth                    = sizeof(u_int) * indices.size();
				bd.BindFlags                    = D3D11_BIND_INDEX_BUFFER;
				bd.CPUAccessFlags               = 0;
				bd.MiscFlags                    = 0;
				bd.StructureByteStride          = 0;
				D3D11_SUBRESOURCE_DATA InitData = {};
				InitData.pSysMem                = &indices[0];				// 頂点のアドレス
				InitData.SysMemPitch            = 0;
				InitData.SysMemSlicePitch       = 0;
				HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, skin_meshes[i].index_buffer.GetAddressOf());
				assert(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		cache.insert(make_pair(fullpass, skin_meshes));
		mesh_ptr->skin_meshes = skin_meshes;
		return mesh_ptr;
	}
}