#pragma once
#include <unordered_map>
#include <vector>
#include <stdio.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
#include <tchar.h>
#include <memory>
#include "Material.h"

class Mesh
{
public:

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texcoord;
#define MAX_BONE_INFLUENCES 4
		FLOAT bone_weights[MAX_BONE_INFLUENCES] = { 1, 0, 0, 0 };
		INT bone_indices[MAX_BONE_INFLUENCES] = {};
	};

	struct material
	{
		DirectX::XMFLOAT4 color = { 0.8f, 0.8f, 0.8f, 1.0f };
		std::wstring TexPass;
		std::string TexName;
		unsigned long ID;
	};

	struct subset
	{
		u_int index_start = 0; // start number of index buffer
		u_int index_count = 0; // number of vertices (indices)
		material diffuse;
	};

	struct bone
	{
		DirectX::XMFLOAT4X4 transform;
	};

	typedef std::vector<bone> skeletal;

	struct skeletal_animation : public std::vector<skeletal>
	{
		float sampling_time = 1 / 24.0f;
		float animation_tick = 0.0f;
	};

	struct mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<subset> subsets;
		DirectX::XMFLOAT4X4 global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		std::vector<skeletal_animation> skeletal_animation;

	};
	typedef std::vector<mesh> meshes;

	struct bone_influence
	{
		int index; // index of bone
		float weight; // weight of bone
	};

	typedef std::vector<bone_influence> bone_influences_per_control_point;

	meshes skin_meshes;
	std::string name;
	Mesh();
	~Mesh();

	static std::shared_ptr<Mesh> Load_Mesh(const char* file_pass, const char* fbx_filename);
private:

};