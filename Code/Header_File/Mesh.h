#pragma once
#include "Texture.h"
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include <Original_Math.h>
#include <tchar.h>
#include "fbxsdk.h"
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"


namespace BeastEngine
{
	class Material;

	class Mesh
	{
	public:

		struct Node
		{
			std::string			name;
			int					parentIndex;
			BeastEngine::Vector3	scale;
			BeastEngine::Quaternion	rotation;
			BeastEngine::Vector3	position;

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
			BeastEngine::Vector3 position;
			BeastEngine::Vector3 normal;
			BeastEngine::Vector3 tangent;
			BeastEngine::Vector2 texcoord;
			float   bone_weights[4] = { 1, 0, 0, 0 };
			int     bone_indices[4] = {};

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive
				(
					position, normal, tangent, texcoord, bone_weights, bone_indices
				);
			}
		};

		struct subset
		{
			u_int index_start = 0; // start number of index buffer
			u_int index_count = 0; // number of vertices (indices)
			BeastEngine::Vector4 color;
			u_int material_ID;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(index_start, index_count, color, material_ID);
			}
		};

		struct NodeKeyData
		{
			BeastEngine::Vector3	scale;
			BeastEngine::Quaternion rotation;
			BeastEngine::Vector3	position;

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

		struct BoundingBox
		{
			BeastEngine::Vector3 min;
			BeastEngine::Vector3 max;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(min, max);
			}
		};

		struct mesh
		{
			Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
			std::vector<subset>					 subsets;

			BoundingBox boundingbox;

			int									 nodeIndex;
			std::vector<int>					 nodeIndices;
			std::vector<BeastEngine::Matrix>					 inverseTransforms;

			std::vector<vertex> vertices;
			std::vector<u_int> indices;

			template<class Archive>
			void serialize(Archive& archive)
			{
				archive(vertices, indices, subsets, nodeIndex, nodeIndices, inverseTransforms, boundingbox);
			}
		};

		std::string name;
		std::string file_path;

		std::vector<Node>		nodes;
		std::vector<mesh>		meshes;
		std::vector<Animation>	animations;
		std::vector<std::string> Default_Material_Passes;

		//指定したパスのメッシュを読み込む
		//第1引数にメッシュまでのパス
		//第2引数にファイル名(拡張子はいらない)を入力する
		static std::shared_ptr<BeastEngine::Mesh> Load_Mesh(const char* file_path, const char* fbx_filename, const char* ignoreRootMotionNodeName = nullptr);

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

		int rootMotionNodeIndex = -1;

		void GetTexture(const FbxSurfaceMaterial* fbx_mat, const char* fbx_tex_type, std::shared_ptr<BeastEngine::Material> mat, Texture::Texture_Type tex_type);

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(nodes, meshes, animations, Default_Material_Passes);
		}
	};
}