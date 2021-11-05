#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <wrl.h>
#include "Renderer.h"

namespace BeastEngine
{
	class Texture;
	class Material;

	//�X�v���C�g�����_���[�R���|�[�l���g
	class Sprite_Renderer : public Renderer
	{
	public:
		Vector2 size = { 100,100 };		    //�\���T�C�Y
		Vector2 uv_origin = { 0,0 };		//�e�N�X�`����UV���_
		Vector2 uv_size = { 100,100 };		//�e�N�X�`������؂�o���T�C�Y(UV���_�)
		Vector4 color = { 1,1,1,1 };	    //�e�N�X�`���J���[({1,1,1,1}�ŃI���W�i��)
		std::shared_ptr<Texture> texture;	//�`�悷��e�N�X�`��

	private:
		void Initialize(const std::shared_ptr<GameObject>& obj) override; //������
		void Recalculate_Frame() override; //�\�����W���v�Z����
		void Render(int subset_number = 0) override; //�`����s
		bool Draw_ImGui() override; //ImGui�`��
		void Set_Active(bool value) override; //�A�N�e�B�u��Ԃ�ݒ肷��

		struct Vertex
		{
			Vector3 pos;	//�ʒu
			Vector2 tex;	//UV���W
			Vector4 color;	//���_�F
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

		std::string file_name;
		std::string file_path;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive, std::uint32_t const version)
		{
			archive(cereal::base_class<Renderer>(this), size, uv_origin, uv_size, color, file_name, file_path);
		}
	};
}

REGISTER_COMPONENT(Sprite_Renderer)
CEREAL_REGISTER_TYPE(BeastEngine::Sprite_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Sprite_Renderer)
CEREAL_CLASS_VERSION(BeastEngine::Sprite_Renderer, 1)