#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <wrl.h>
#include "Renderer.h"

namespace BeastEngine
{
	class Shader;
	class Texture;

	class Sprite_Renderer : public BeastEngine::Renderer
	{
	public:

		BeastEngine::Vector2 size = { 100,100 };			//�\���T�C�Y
		BeastEngine::Vector2 uv_origin = { 0,0 };		//�e�N�X�`����UV���_
		BeastEngine::Vector2 uv_size = { 100,100 };		//�e�N�X�`������؂�o���T�C�Y(UV���_�)
		BeastEngine::Vector4 color = { 1,1,1,1 };		//�e�N�X�`���J���[({1,1,1,1}�ŃI���W�i��)
		std::shared_ptr<BeastEngine::Texture> texture;	//�`�悷��e�N�X�`��

	private:

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj);
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		bool Draw_ImGui() override;
		void Set_Active(bool value) override;

		struct Vertex
		{
			BeastEngine::Vector3 pos;	//�ʒu
			BeastEngine::Vector2 tex;	//UV���W
			BeastEngine::Vector4 color;	//���_�F
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;

		std::string file_name;
		std::string file_path;

		static std::shared_ptr<BeastEngine::Shader> default_shader;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), size, uv_origin, uv_size, color, file_name, file_path);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Sprite_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Sprite_Renderer)