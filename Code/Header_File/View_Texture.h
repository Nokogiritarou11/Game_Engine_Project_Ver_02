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
#include "SkyBox.h"
#include "Transform.h"
#include "Light_Manager.h"
using Microsoft::WRL::ComPtr;

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
	virtual void Render(Matrix V, Matrix P, std::shared_ptr<Transform> trans) = 0;
	void Render_Sky(std::shared_ptr<Transform> trans);
	void Render_3D(Matrix V, Matrix P, bool Is_Shadow);
	void Render_2D(Matrix V, Matrix P);

	int screen_x = 0;
	int screen_y = 0;

	bool CreateDepthStencil(int x, int y);
	bool CreateRenderTartgetView(int x, int y);
	void Clear();

	static std::unique_ptr<SkyBox> skybox;

	struct CbScene
	{
		Matrix	viewProjection;
		Matrix	shadowMatrix;
		Vector4	lightDirection;
		Vector3 lightColor;
		float	Bias;
	};

	ComPtr <ID3D11Buffer> ConstantBuffer_CbScene; //コンスタントバッファ

private:
};