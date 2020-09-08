#pragma once

#include <unordered_map>
#include <vector>
#include <wrl.h>
#include <Original_Math.h>
#include <tchar.h>
#include "Material.h"
#include <fbxsdk.h>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"


class Mesh
{
public:

	struct Node
	{
		std::string			name;
		int					parentIndex;
		Vector3	scale;
		Vector4	rotation;
		Vector3	position;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				position, scale, rotation, parentIndex, name
			);
		}
	};

	struct vertex
	{
		Vector3 position;
		Vector3 normal;
		Vector2 texcoord;
		float   bone_weights[4] = { 1, 0, 0, 0 };
		int     bone_indices[4] = {};

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
			archive(TexPass, TexName, color, ID);
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

	struct NodeKeyData
	{
		Vector3	scale;
		Vector4	rotation;
		Vector3	position;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(scale, rotation, position);
		}
	};

	struct Keyframe
	{
		float						seconds;
		std::vector<NodeKeyData>	nodeKeys;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(seconds, nodeKeys);
		}
	};
	struct Animation
	{
		std::string					name;
		float						secondsLength;
		std::vector<Keyframe>		keyframes;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name, secondsLength, keyframes);
		}
	};

	struct mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<subset>					 subsets;

		int									 nodeIndex;
		std::vector<int>					 nodeIndices;
		std::vector<Matrix>					 inverseTransforms;

		std::vector<vertex> vertices;
		std::vector<u_int> indices;

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(vertices, indices, subsets, nodeIndex, nodeIndices, inverseTransforms);
		}
	};

	std::string name;
	std::string file_pass;

	std::vector<Node>		nodes;
	std::vector<mesh>		meshes;
	std::vector<Animation>	animations;

	static std::shared_ptr<Mesh> Load_Mesh(const char* file_pass, const char* fbx_filename, const char* ignoreRootMotionNodeName = nullptr);

private:
	// ノードデータを構築
	void BuildNodes(FbxNode* fbxNode, int parentNodeIndex);
	void BuildNode(FbxNode* fbxNode, int parentNodeIndex);

	// メッシュデータを構築
	void BuildMeshes(FbxNode* fbxNode);
	void BuildMesh(FbxNode* fbxNode, FbxMesh* fbxMesh);

	// アニメーションデータを構築
	void BuildAnimations(FbxScene* fbxScene);

	// インデックスの検索
	int FindNodeIndex(const char* name);

	int						rootMotionNodeIndex = -1;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(nodes, meshes, animations);
	}
};