#pragma once
#include "Renderer.h"

namespace BeastEngine
{
	class Mesh;
	class Material;
	class Compute_Shader;

	//���b�V�������_���[�R���|�[�l���g
	class Mesh_Renderer : public Renderer
	{
	public:
		void Set_Mesh(const std::shared_ptr<Mesh>& mesh_data); //���b�V���f�[�^��ݒ肷��

	private:
		std::shared_ptr<Mesh> mesh;

		void Initialize(const std::shared_ptr<GameObject>& obj) override; //������
		void Render(int subset_number = 0) override; //�`����s
		void Render_Shadow(int subset_number = 0) override; //�V���h�E�}�b�v�֕`����s
		bool Draw_ImGui() override; //ImGui�`��
		void Set_Active(bool value) override; //�A�N�e�B�u��Ԃ�ݒ肷��
		void Recalculate_Frame() override; //�R���s���[�g�V�F�[�_�[�Œ��_���v�Z����

		//���[���h�ϊ��p�R���X�^���g�o�b�t�@
		struct Constant_Buffer_Mesh
		{
			Matrix world = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
		};
		//�J���[���R���X�^���g�o�b�t�@
		struct Constant_Buffer_Color
		{
			Vector4	material_color;
		};

		Constant_Buffer_Mesh buffer_mesh;
		static Microsoft::WRL::ComPtr <ID3D11Buffer> constant_buffer_mesh;

		void Reset(); //�f�[�^�̃N���A

		std::string file_path;
		std::shared_ptr<Compute_Shader> compute_shader;

		friend class Fbx_Converter;
		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Renderer>(this), file_path);
		}
	};
}

REGISTER_COMPONENT(Mesh_Renderer)
CEREAL_REGISTER_TYPE(BeastEngine::Mesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Mesh_Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Mesh_Renderer, 1)