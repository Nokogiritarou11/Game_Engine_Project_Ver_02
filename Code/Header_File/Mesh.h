#pragma once
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "DxSystem.h"
#include "Bounds.h"
#include "Object.h"

namespace BeastEngine
{
	class Mesh : public BeastEngine::Object
	{
	public:
		struct vertex
		{
			BeastEngine::Vector3 position;
			BeastEngine::Vector3 normal;
			BeastEngine::Vector3 tangent;
			BeastEngine::Vector2 texcoord;
			float   bone_weights[4] = { 1, 0, 0, 0 };
			int     bone_indices[4] = {};

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive
				(
					position, normal, tangent, texcoord, bone_weights, bone_indices
				);
			}
		};

		struct vertex_default_buffer
		{
			//コンピュートシェーダー計算後の頂点データ
			BeastEngine::Vector3 position;
			BeastEngine::Vector3 normal;
			BeastEngine::Vector3 tangent;
			BeastEngine::Vector2 texcoord;
		};

		struct subset
		{
			u_int index_start = 0; // start number of index buffer
			u_int index_count = 0; // number of vertices (indices)
			BeastEngine::Vector4 color = { 1,1,1,1 };
			u_int material_ID = 0;

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(index_start, index_count, color, material_ID);
			}
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<vertex> vertices;
		std::vector<u_int> indices;

		std::vector<subset>	subsets;

		BeastEngine::Bounds boundingbox;
		std::vector<BeastEngine::Matrix> inverse_matrixes;
		std::vector<int> nodeIndices;

		std::vector<std::string> default_material_passes;
		std::string file_path;

		static std::shared_ptr<BeastEngine::Mesh> Load_Mesh(const std::string& fullpath);

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<BeastEngine::Object>(this), vertices, indices, subsets, default_material_passes, inverse_matrixes, boundingbox);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Mesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Mesh)
CEREAL_CLASS_VERSION(BeastEngine::Mesh, 1)
CEREAL_CLASS_VERSION(BeastEngine::Mesh::vertex, 1)
CEREAL_CLASS_VERSION(BeastEngine::Mesh::subset, 1)