#pragma once

#include "cereal/cereal.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"

#include <unordered_map>
#include <vector>
#include <stdio.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <Original_Math.h>
#include <string>
#include <tchar.h>
#include <memory>
#include "Material.h"
#include <fbxsdk.h>

class Mesh
{
public:

	struct vertex
	{
#define MAX_BONE_INFLUENCES 4
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
		float   bone_weights[MAX_BONE_INFLUENCES] = { 1, 0, 0, 0 };
		int     bone_indices[MAX_BONE_INFLUENCES] = {};

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				position, normal, texcoord, bone_weights, bone_indices
			);
		}
	};

	struct material
	{
		std::string TexPass;
		std::string  TexName;
		Vector4 color;
		unsigned long ID = 0;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				TexPass, TexName, color, ID
			);
		}
	};

	struct subset
	{
		u_int index_start = 0; // start number of index buffer
		u_int index_count = 0; // number of vertices (indices)
		material diffuse;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(index_start, index_count, diffuse);
		}
	};

	struct bone
	{
		Matrix transform;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(transform);
		}
	};

	struct skeletal_animation
	{
		std::vector<std::vector<bone>> bones;
		float sampling_time = 1 / 24.0f;
		float animation_tick = 0.0f;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(bones, sampling_time, animation_tick);
		}
	};

	struct bone_influence
	{
		int index; // index of bone
		float weight; // weight of bone
	};
	typedef std::vector<bone_influence> bone_influences_per_control_point;

	struct mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

		std::vector<vertex> vertices;
		std::vector<u_int> indices;
		std::vector<subset> subsets;
		Matrix global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		std::vector<skeletal_animation> skeletal_animation;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(vertices, indices, subsets, global_transform, skeletal_animation);
		}
	};

	std::vector<mesh> skin_meshes;
	std::string name;

	static std::shared_ptr<Mesh> Load_Mesh(const char* file_pass, const char* fbx_filename);

private:
	static void fetch_bone_influences(const fbxsdk::FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& influences);
	static void fetch_bone_matrices(fbxsdk::FbxMesh* fbx_mesh, std::vector<bone>& skeletal, fbxsdk::FbxTime time);
	static void fetch_animations(fbxsdk::FbxMesh* fbx_mesh, std::vector<skeletal_animation>& skeletal_animation, u_int sampling_rate = 0);
};