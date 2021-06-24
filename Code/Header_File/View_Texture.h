#pragma once
#include <InitGUID.h>
#include <dxgidebug.h>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <string.h>
#include <assert.h>
#include <DirectXMath.h>
#include "Original_Math.h"
using Microsoft::WRL::ComPtr;

namespace BeastEngine
{
	class Transform;
	class SkyBox;

	class View_Texture
	{
	public:
		ComPtr<ID3D11RenderTargetView>		render_target_view;
		ComPtr<ID3D11Texture2D>				texture_render_target;
		ComPtr<ID3D11DepthStencilView>		depth_stencil_view;
		ComPtr<ID3D11Texture2D>				texture_depth_stencil;
		ComPtr<ID3D11ShaderResourceView>	shader_resource_view_depth;
		ComPtr<ID3D11ShaderResourceView>	shader_resource_view_render;

		View_Texture();
		~View_Texture() {};

		void Set_Screen_Size(int x, int y);

		virtual void Render(BeastEngine::Matrix V, BeastEngine::Matrix P, std::shared_ptr<BeastEngine::Transform> camera_transform) = 0;

		void Render_Shadow(std::shared_ptr<BeastEngine::Transform> camera_transform);

		void Render_Sky(BeastEngine::Vector3 pos);
		void Render_3D(BeastEngine::Matrix V, BeastEngine::Matrix P);
		void Render_2D(BeastEngine::Matrix V, BeastEngine::Matrix P);

		int screen_x = 0;
		int screen_y = 0;

		bool Create_Depth_Stencil(int x, int y);
		bool Create_Render_Tartget_View(int x, int y);
		void Clear();

		static std::unique_ptr<BeastEngine::SkyBox> skybox;

		struct Constant_Buffer_Scene
		{
			BeastEngine::Matrix	 view_projection_matrix;
			BeastEngine::Matrix	 shadow_matrix;
			BeastEngine::Vector4 light_direction;
			BeastEngine::Vector3 light_color;
			float	bias;
		};
		Constant_Buffer_Scene buffer_scene;

		ComPtr <ID3D11Buffer> constant_buffer_scene; //コンスタントバッファ

	private:
		void Render_Shadow_Directional(BeastEngine::Vector4 color, float intensity, std::shared_ptr<BeastEngine::Transform> light_transform, std::shared_ptr<BeastEngine::Transform> camera_transform);
	};
}