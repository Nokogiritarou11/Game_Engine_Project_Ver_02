#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <string>
#include <wrl.h>
#include <d3dcompiler.h>
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"

class Sprite_Renderer : public Renderer
{
private:

	struct VERTEX
	{
		DirectX::XMFLOAT3 Pos;	//位置
		DirectX::XMFLOAT2 Tex;	//UV座標
		DirectX::XMFLOAT4 Color;	//頂点色
	};

	ComPtr<ID3D11Buffer> VertexBuffer;

public:
	Sprite_Renderer();
	~Sprite_Renderer();

	std::shared_ptr<Material> material;

	void Initialize(std::shared_ptr<GameObject> obj);
	void Set_Texture(std::string Material_Name, WCHAR* Shader_Name, std::string filename);
	void Render(std::shared_ptr<Camera> Render_Camera);
};

/*

class Sprite_Renderer_batch
{
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> g_pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> g_pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> g_pVertexLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> g_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> g_pRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> g_pShaderResourceView;
	D3D11_TEXTURE2D_DESC g_TEXTURE2D_DESC;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> g_pSamplerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> g_pDepthStencilState;

	const size_t MAX_INSTANCES = 256;
	struct instance
	{
		XMFLOAT4X4 ndc_transform;
		XMFLOAT4 texcoord_transform;
		XMFLOAT4 color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> instance_buffer;
public:
	struct vertex
	{
		XMFLOAT3 position;
		XMFLOAT2 texcoord;
	};

	Sprite_Renderer_batch(ID3D11Device* device, const wchar_t* TexName, size_t max_instance = 256);

	void begin(ID3D11DeviceContext* g_pImmediateContext);
	void render(ID3D11DeviceContext* g_pImmediateContext,
		float dx, float dy,
		float dw, float dh,
		float sx, float sy,
		float sw, float sh,
		float angle/*degree,
		float r, float g, float b, float a);
	void end(ID3D11DeviceContext* g_pImmediateContext);

private:
	D3D11_VIEWPORT viewport;

	UINT count_instance = 0;
	instance* instances = nullptr;
};
*/