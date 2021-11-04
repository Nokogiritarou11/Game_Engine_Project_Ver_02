#pragma once
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "DxSystem.h"
#include "Bounds.h"
#include "Object.h"

namespace BeastEngine
{
	//���b�V���f�[�^�N���X
	class Mesh final : public Object
	{
	public:
		//���_�f�[�^
		struct vertex
		{
			Vector3 position;
			Vector3 normal;
			Vector3 tangent;
			Vector2 texcoord;
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

		//�R���s���[�g�V�F�[�_�[�v�Z��̒��_�f�[�^
		struct Vertex_Default_Buffer
		{
			Vector3 position;
			Vector3 normal;
			Vector3 tangent;
			Vector3 origin_position;
			Vector3 origin_normal;
			Vector3 origin_tangent;
			Vector2 texcoord;
		};

		//�T�u�Z�b�g
		struct subset
		{
			u_int index_start = 0; //�C���f�b�N�X�o�b�t�@�̊J�n�ʒu
			u_int index_count = 0; //���_��
			u_int material_ID = 0; //�}�e���A���̃C���f�b�N�X

		private:
			friend class cereal::access;
			template<class Archive>
			void serialize(Archive& archive, std::uint32_t const version)
			{
				archive(index_start, index_count, material_ID);
			}
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<vertex> vertices;
		std::vector<u_int> indices;
		std::vector<subset>	subsets;

		Bounds boundingbox;
		std::vector<Matrix> inverse_matrixes;
		std::vector<int> node_indices;
		std::vector<std::string> default_material_paths;
		std::string file_path;

		static std::shared_ptr<Mesh> Load_Mesh(const std::string& full_path);

	private:
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Object>(this), vertices, indices, subsets, default_material_paths, inverse_matrixes, boundingbox);
		}
	};
}

CEREAL_REGISTER_TYPE(BeastEngine::Mesh)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Object, BeastEngine::Mesh)
CEREAL_CLASS_VERSION(BeastEngine::Mesh, 1)
CEREAL_CLASS_VERSION(BeastEngine::Mesh::vertex, 1)
CEREAL_CLASS_VERSION(BeastEngine::Mesh::subset, 1)