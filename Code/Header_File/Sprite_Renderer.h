#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
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

		BeastEngine::Vector2 Size = { 100,100 };			//�\���T�C�Y
		BeastEngine::Vector2 UV_Origin = { 0,0 };		//�e�N�X�`����UV���_
		BeastEngine::Vector2 UV_Size = { 100,100 };		//�e�N�X�`������؂�o���T�C�Y(UV���_�)
		BeastEngine::Vector4 Color = { 1,1,1,1 };		//�e�N�X�`���J���[({1,1,1,1}�ŃI���W�i��)
		std::shared_ptr<BeastEngine::Texture> texture;	//�`�悷��e�N�X�`��

	private:

		void Initialize(std::shared_ptr<BeastEngine::GameObject> obj);
		void Render(BeastEngine::Matrix V, BeastEngine::Matrix P) override;
		bool Draw_ImGui() override;
		void SetActive(bool value) override;

		struct VERTEX
		{
			BeastEngine::Vector3 Pos;	//�ʒu
			BeastEngine::Vector2 Tex;	//UV���W
			BeastEngine::Vector4 Color;	//���_�F
		};

		ComPtr<ID3D11Buffer> VertexBuffer;

		std::string file_name;
		std::string file_path;

		static std::unique_ptr<BeastEngine::Shader> default_shader;

		friend class cereal::access;
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(cereal::base_class<BeastEngine::Renderer>(this), Size, UV_Origin, UV_Size, Color, file_name, file_path);
		}
	};
}
CEREAL_REGISTER_TYPE(BeastEngine::Sprite_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(BeastEngine::Renderer, BeastEngine::Sprite_Renderer)