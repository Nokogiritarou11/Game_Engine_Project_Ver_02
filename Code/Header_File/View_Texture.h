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
		ComPtr<ID3D11RenderTargetView>		RenderTargetView;
		ComPtr<ID3D11Texture2D>				Texture_RenderTarget;
		ComPtr<ID3D11DepthStencilView>		DepthStencilView;
		ComPtr<ID3D11Texture2D>				Texture_DepthStencil;
		ComPtr<ID3D11ShaderResourceView>	ShaderResourceView_Depth;
		ComPtr<ID3D11ShaderResourceView>	ShaderResourceView_Render;

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

		bool CreateDepthStencil(int x, int y);
		bool CreateRenderTartgetView(int x, int y);
		void Clear();

		static std::unique_ptr<BeastEngine::SkyBox> skybox;

		struct CbScene
		{
			BeastEngine::Matrix	viewProjection;
			BeastEngine::Matrix	shadowMatrix;
			BeastEngine::Vector4	lightDirection;
			BeastEngine::Vector3 lightColor;
			float	Bias;
		};
		CbScene cb_scene;

		ComPtr <ID3D11Buffer> ConstantBuffer_CbScene; //コンスタントバッファ

	private:
		void Render_Shadow_Directional(BeastEngine::Vector4 Color, float Intensity, std::shared_ptr<BeastEngine::Transform> light_transform, std::shared_ptr<BeastEngine::Transform> camera_transform);
	};
}