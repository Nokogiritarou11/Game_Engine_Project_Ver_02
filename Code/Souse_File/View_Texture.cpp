#include "View_Texture.h"
#include "DxSystem.h"
#include "Light_Manager.h"
#include "Engine.h"
#include "Render_Manager.h"
#include "Shadow_Manager.h"
#include "SkyBox.h"
#include "Transform.h"
using namespace std;
using namespace DirectX;

unique_ptr<SkyBox> View_Texture::skybox;

View_Texture::View_Texture()
{
	CreateRenderTartgetView(800, 600);
	CreateDepthStencil(800, 600);

	HRESULT hr;
	// 定数バッファの生成
	if (!ConstantBuffer_CbScene)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CbScene);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer_CbScene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	skybox = make_unique<SkyBox>();
}

void View_Texture::Clear()
{
	float clearColor[4] = { 0,1,0,0 };
	DxSystem::DeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
	DxSystem::DeviceContext->ClearDepthStencilView(DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void View_Texture::Set_Screen_Size(int x, int y)
{
	if (screen_x != x || screen_y != y)
	{

		CreateRenderTartgetView(x, y);
		CreateDepthStencil(x, y);

		screen_x = x;
		screen_y = y;
	}
}

bool View_Texture::CreateDepthStencil(int x, int y)
{
	// 深度ステンシル設定
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.Width = x;
	td.Height = y;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G8X24_TYPELESS;
	td.SampleDesc = DxSystem::MSAA;
	//td.SampleDesc.Count = 1;
	//td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	// 深度ステンシルテクスチャ生成
	HRESULT hr = DxSystem::Device->CreateTexture2D(&td, NULL, Texture_DepthStencil.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// 深度ステンシルビュー設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvd.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvd.Texture2D.MipSlice = 0;

	// 深度ステンシルビュー生成
	hr = DxSystem::Device->CreateDepthStencilView(Texture_DepthStencil.Get(), &dsvd, DepthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// シェーダリソースビュー設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	ZeroMemory(&srvd, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvd.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvd.Texture2D.MostDetailedMip = 0;
	srvd.Texture2D.MipLevels = 1;

	// シェーダリソースビュー生成
	hr = DxSystem::Device->CreateShaderResourceView(Texture_DepthStencil.Get(), &srvd, ShaderResourceView_Depth.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	return true;
}

bool View_Texture::CreateRenderTartgetView(int x, int y)
{
	// 2次元テクスチャの設定
	D3D11_TEXTURE2D_DESC texDesc;
	memset(&texDesc, 0, sizeof(texDesc));
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.Width = x;
	texDesc.Height = y;
	texDesc.CPUAccessFlags = 0;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc = DxSystem::MSAA;
	//texDesc.SampleDesc.Count = 1;
	// 2次元テクスチャの生成
	HRESULT hr = DxSystem::Device->CreateTexture2D(&texDesc, NULL, Texture_RenderTarget.GetAddressOf());

	// レンダーターゲットビューの設定
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	memset(&rtvDesc, 0, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	rtvDesc.Texture2D.MipSlice = 0;

	// レンダーターゲットビューの生成
	hr = DxSystem::Device->CreateRenderTargetView(Texture_RenderTarget.Get(), &rtvDesc, RenderTargetView.GetAddressOf());

	// シェーダリソースビューの設定
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	memset(&srvDesc, 0, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	// シェーダリソースビューの生成
	hr = DxSystem::Device->CreateShaderResourceView(Texture_RenderTarget.Get(), &srvDesc, ShaderResourceView_Render.GetAddressOf());

	return true;
}

void View_Texture::Render_Sky(Vector3 pos)
{
	//トポロジー設定
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(BS_State::Off), nullptr, 0xFFFFFFFF);
	Renderer::Set_BlendState = BS_State::Off;
	//ラスタライザ―設定
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(RS_State::Cull_None));
	Renderer::Set_RasterizerState = RS_State::Cull_None;
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DS_State::None_No_Write), 1);
	Renderer::Set_DepthStencilState = DS_State::None_No_Write;

	skybox->Render(pos);
}

void View_Texture::Render_Shadow_Directional(Vector4 Color, float Intensity, std::shared_ptr<Transform> light_transform, std::shared_ptr<Transform> camera_transform)
{
	Matrix V, P, VP;
	Vector3 Look_pos = camera_transform->Get_position();
	V = XMMatrixLookAtRH(Look_pos - camera_transform->Get_forward(), Look_pos, camera_transform->Get_up());
	float size = (float)Engine::shadow_manager->Get_Shadow_Map_Texture_Size();
	P = XMMatrixOrthographicRH(size, size, 0.1f, 100.f);
	VP = V * P;

	cb_scene.viewProjection = VP;
	static const Matrix SHADOW_BIAS = {
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f };
	cb_scene.shadowMatrix = VP * SHADOW_BIAS;
	const Vector3 forward = light_transform->Get_forward();
	cb_scene.lightDirection = { forward.x, forward.y, forward.z, 0 };
	cb_scene.lightColor = { Color.x, Color.y, Color.z };
	cb_scene.Bias = Engine::shadow_manager->Get_Shadow_Bias();

	DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
	DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbScene.Get(), 0, 0, &cb_scene, 0, 0);

	//トポロジー設定
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(BS_State::Off), nullptr, 0xFFFFFFFF);
	Renderer::Set_BlendState = BS_State::Off;
	//ラスタライザ―設定
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(RS_State::Cull_Front));
	Renderer::Set_RasterizerState = RS_State::Cull_Front;
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DS_State::LEqual), 1);
	Renderer::Set_DepthStencilState = DS_State::LEqual;

	shared_ptr<Renderer> p_rend = nullptr;
	bool expired = false;
	bool disabled = false;
	for (weak_ptr<Renderer> r : Engine::render_manager->Renderer_3D_list)
	{
		if (!r.expired())
		{
			p_rend = r.lock();
			if (p_rend->gameObject->activeInHierarchy())
			{
				if (p_rend->enableSelf())
				{
					p_rend->Render_Shadow(V, P);
				}
			}
			else
			{
				p_rend->Disable_flg = true;
				disabled = true;
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(Engine::render_manager->Renderer_3D_list.begin(), Engine::render_manager->Renderer_3D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
		Engine::render_manager->Renderer_3D_list.erase(removeIt, Engine::render_manager->Renderer_3D_list.end());
	}
	if (disabled)
	{
		auto removeIt = remove_if(Engine::render_manager->Renderer_3D_list.begin(), Engine::render_manager->Renderer_3D_list.end(), [](weak_ptr<Renderer> r) { shared_ptr<Renderer> rend = r.lock(); rend->IsCalled = false; return rend->Disable_flg; });
		Engine::render_manager->Renderer_3D_list.erase(removeIt, Engine::render_manager->Renderer_3D_list.end());
	}
}

void View_Texture::Render_3D(Matrix V, Matrix P)
{
	//トポロジー設定
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::shadow_manager->Set_PS_Resource();

	shared_ptr<Renderer> p_rend = nullptr;
	bool expired = false;
	bool disabled = false;
	for (weak_ptr<Renderer> r : Engine::render_manager->Renderer_3D_list)
	{
		if (!r.expired())
		{
			p_rend = r.lock();
			if (p_rend->gameObject->activeInHierarchy())
			{
				if (p_rend->enableSelf())
				{
					p_rend->Render(V, P);
				}
			}
			else
			{
				p_rend->Disable_flg = true;
				disabled = true;
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(Engine::render_manager->Renderer_3D_list.begin(), Engine::render_manager->Renderer_3D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
		Engine::render_manager->Renderer_3D_list.erase(removeIt, Engine::render_manager->Renderer_3D_list.end());
	}
	if (disabled)
	{
		auto removeIt = remove_if(Engine::render_manager->Renderer_3D_list.begin(), Engine::render_manager->Renderer_3D_list.end(), [](weak_ptr<Renderer> r) { shared_ptr<Renderer> rend = r.lock(); rend->IsCalled = false; return rend->Disable_flg; });
		Engine::render_manager->Renderer_3D_list.erase(removeIt, Engine::render_manager->Renderer_3D_list.end());
	}
}

void View_Texture::Render_2D(Matrix V, Matrix P)
{
	//トポロジー設定
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(BS_State::Alpha), nullptr, 0xFFFFFFFF);
	Renderer::Set_BlendState = BS_State::Alpha;
	//ラスタライザ―設定
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(RS_State::Cull_None));
	Renderer::Set_RasterizerState = RS_State::Cull_None;
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DS_State::None_No_Write), 1);
	Renderer::Set_DepthStencilState = DS_State::None_No_Write;
	shared_ptr<Renderer> p_rend = nullptr;
	bool expired = false;
	bool disabled = false;
	for (weak_ptr<Renderer> r : Engine::render_manager->Renderer_2D_list)
	{
		if (!r.expired())
		{
			p_rend = r.lock();
			if (p_rend->gameObject->activeInHierarchy())
			{
				if (p_rend->enableSelf())
				{
					p_rend->Render(V, P);
				}
			}
			else
			{
				p_rend->Disable_flg = true;
				disabled = true;
			}
		}
		else
		{
			expired = true;
		}
	}
	if (expired)
	{
		auto removeIt = remove_if(Engine::render_manager->Renderer_2D_list.begin(), Engine::render_manager->Renderer_2D_list.end(), [](weak_ptr<Renderer> r) { return r.expired(); });
		Engine::render_manager->Renderer_2D_list.erase(removeIt, Engine::render_manager->Renderer_2D_list.end());
	}
	if (disabled)
	{
		auto removeIt = remove_if(Engine::render_manager->Renderer_2D_list.begin(), Engine::render_manager->Renderer_2D_list.end(), [](weak_ptr<Renderer> r) { shared_ptr<Renderer> rend = r.lock(); rend->IsCalled = false; return rend->Disable_flg; });
		Engine::render_manager->Renderer_2D_list.erase(removeIt, Engine::render_manager->Renderer_2D_list.end());
	}
}