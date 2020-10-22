#include "Sprite_Renderer.h"
#include "DxSystem.h"
#include "GameObject.h"
#include "Engine.h"
#include "Include_ImGui.h"
#include "Debug.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "System_Function.h"
using namespace std;

Sprite_Renderer::Sprite_Renderer()
{
}

Sprite_Renderer::~Sprite_Renderer()
{
}

void Sprite_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	Engine::render_manager->Add(static_pointer_cast<Sprite_Renderer>(shared_from_this()));

	VERTEX v[] = {
		XMFLOAT3(-0.5f, 0.5f,0),  XMFLOAT2(0,0), XMFLOAT4(1,1,1,1), //左上
		XMFLOAT3(0.5f, 0.5f,0),  XMFLOAT2(1,0), XMFLOAT4(1,1,1,1), //右上
		XMFLOAT3(-0.5f,-0.5f,0),  XMFLOAT2(0,1), XMFLOAT4(1,1,1,1), //左下
		XMFLOAT3(0.5f,-0.5f,0),  XMFLOAT2(1,1), XMFLOAT4(1,1,1,1), //右下
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	DxSystem::Device->CreateBuffer(&bd, &res, VertexBuffer.GetAddressOf());
	texture = make_shared<Texture>();

	material.emplace_back(Material::Create("Default_Resource\\Image\\", "Default_2D", L"Shader/2D_Shader_VS.hlsl", L"Shader/2D_Shader_PS.hlsl"));

	if (file_pass != "")
	{
		texture->Load(file_pass + file_name);
	}
}

void Sprite_Renderer::Render(Matrix V, Matrix P)
{
	if (texture)
	{
		material[0]->shader->Activate();
		//頂点データ設定
		VERTEX data[4];

		Vector3 trans_pos = gameObject->transform->Get_position();

		data[0].Pos.x = trans_pos.x;
		data[0].Pos.y = trans_pos.y;
		data[0].Pos.z = 0.0f;

		data[1].Pos.x = trans_pos.x + Size.x;//trans->Width;
		data[1].Pos.y = trans_pos.y;
		data[1].Pos.z = 0.0f;

		data[2].Pos.x = trans_pos.x;
		data[2].Pos.y = trans_pos.y + Size.y;//trans->Height;
		data[2].Pos.z = 0.0f;

		data[3].Pos.x = trans_pos.x + Size.x;//trans->Width;
		data[3].Pos.y = trans_pos.y + Size.y;//trans->Height;
		data[3].Pos.z = 0.0f;

		// 中心座標を原点へ
		//float mx = trans_pos.x + trans->Width * 0.5f;
		//float my = trans_pos.y + trans->Height * 0.5f;
		float mx = trans_pos.x + Size.x * 0.5f;
		float my = trans_pos.y + Size.y * 0.5f;
		data[0].Pos.x -= mx; data[0].Pos.y -= my;
		data[1].Pos.x -= mx; data[1].Pos.y -= my;
		data[2].Pos.x -= mx; data[2].Pos.y -= my;
		data[3].Pos.x -= mx; data[3].Pos.y -= my;

		// 角度計算
		float rx, ry;
		float z = transform->Get_eulerAngles().z;
		float cos = cosf(XMConvertToRadians(z));
		float sin = sinf(XMConvertToRadians(z));

		rx = data[0].Pos.x;
		ry = data[0].Pos.y;
		data[0].Pos.x = cos * rx + -sin * ry;
		data[0].Pos.y = sin * rx + cos * ry;

		rx = data[1].Pos.x;
		ry = data[1].Pos.y;
		data[1].Pos.x = cos * rx + -sin * ry;
		data[1].Pos.y = sin * rx + cos * ry;

		rx = data[2].Pos.x;
		ry = data[2].Pos.y;
		data[2].Pos.x = cos * rx + -sin * ry;
		data[2].Pos.y = sin * rx + cos * ry;

		rx = data[3].Pos.x;
		ry = data[3].Pos.y;
		data[3].Pos.x = cos * rx + -sin * ry;
		data[3].Pos.y = sin * rx + cos * ry;

		// 動かした分戻す
		data[0].Pos.x += mx; data[0].Pos.y += my;
		data[1].Pos.x += mx; data[1].Pos.y += my;
		data[2].Pos.x += mx; data[2].Pos.y += my;
		data[3].Pos.x += mx; data[3].Pos.y += my;

		// 正規化デバイス座標系
		for (int i = 0; i < 4; i++)
		{
			data[i].Pos.x = 2.0f * data[i].Pos.x / DxSystem::GetScreenWidth() - 1.0f;
			data[i].Pos.y = 1.0f - 2.0f * data[i].Pos.y / DxSystem::GetScreenHeight();
			data[i].Pos.z = 0.0f;
		}

		//テクスチャ座標設定
		data[0].Tex.x = UV_Origin.x;
		data[0].Tex.y = UV_Origin.y;
		data[1].Tex.x = UV_Origin.x + UV_Size.x;
		data[1].Tex.y = UV_Origin.y;
		data[2].Tex.x = UV_Origin.x;
		data[2].Tex.y = UV_Origin.y + UV_Size.y;
		data[3].Tex.x = UV_Origin.x + UV_Size.x;
		data[3].Tex.y = UV_Origin.y + UV_Size.y;

		//UV座標
		float w = (float)texture->GetWidth();
		float h = (float)texture->GetHeight();
		for (int i = 0; i < 4; i++)
		{
			data[i].Tex.x = data[i].Tex.x / w;
			data[i].Tex.y = data[i].Tex.y / h;
		}
		//頂点カラー
		data[0].Color = Color;
		data[1].Color = Color;
		data[2].Color = Color;
		data[3].Color = Color;

		DxSystem::DeviceContext->IASetInputLayout(material[0]->shader->VertexLayout.Get());
		//	頂点バッファの指定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;
		DxSystem::DeviceContext->IASetVertexBuffers(
			0, 1, VertexBuffer.GetAddressOf(), // スロット, 数, バッファ
			&stride,		// １頂点のサイズ
			&offset			// 開始位置
		);
		DxSystem::DeviceContext->UpdateSubresource(VertexBuffer.Get(), 0, NULL, data, 0, 0);

		//テクスチャの設定
		texture->Set(1);

		DxSystem::DeviceContext->Draw(4, 0);
	}
}

bool Sprite_Renderer::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("SkinMesh_Renderer"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("SkinMesh_Renderer_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
		}
		if (!removed)
		{
			return false;
		}

		ImGui::Text(u8"現在のテクスチャ::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		ImGui::SameLine();
		if (ImGui::Button(u8"テクスチャを選択"))
		{
			string path = System_Function::Get_Open_File_Name();
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;//7
				int ext_i = path.find_last_of(".");//10
				string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
				string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				texture->Load(path);
				file_pass = pathname;
				file_name = filename + extname;
			}
			else
			{
				Debug::Log("ファイルを開けませんでした");
			}
		}

		static float size[2] = { 0,0 };
		static int uv_origin[2] = { 0,0 };
		static int uv_size[2] = { 0,0 };

		size[0] = Size.x; size[1] = Size.y;
		uv_origin[0] = (int)UV_Origin.x; uv_origin[1] = (int)UV_Origin.y;
		uv_size[0] = (int)UV_Size.x; uv_size[1] = (int)UV_Size.y;

		ImGui::DragFloat2(u8"表示サイズ", size, 0.1f, -FLT_MAX, FLT_MAX);
		ImGui::DragInt2(u8"UV始点", uv_origin, 0, INT_MAX);
		ImGui::DragInt2(u8"UVサイズ", uv_size, 0, INT_MAX);

		Size = { (float)size[0],(float)size[1] };
		UV_Origin = { (float)uv_origin[0],(float)uv_origin[1] };
		UV_Size = { (float)uv_size[0],(float)uv_size[1] };

		float out_color[4] = { Color.x,Color.y,Color.z,Color.w };
		ImGui::ColorEdit4("Color", out_color);
		Color = { out_color[0],out_color[1] ,out_color[2] ,out_color[3] };
	}
	return true;
}

/*
//スプライトバッチ
Sprite_Renderer_batch::Sprite_Renderer_batch(ID3D11Device* device, const wchar_t* TexName, size_t max_instance) : MAX_INSTANCES(max_instance)
{
	HRESULT hr = S_OK;

	vertex vertices[] =
	{
		{ XMFLOAT3(0, 0, 0), XMFLOAT2(0, 0) },
		{ XMFLOAT3(1, 0, 0), XMFLOAT2(1, 0) },
		{ XMFLOAT3(0, 1, 0), XMFLOAT2(0, 1) },
		{ XMFLOAT3(1, 1, 0), XMFLOAT2(1, 1) },
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;				// 頂点のアドレス
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&bd, &InitData, g_pVertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NDC_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "NDC_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};
	create_vs_from_cso(device, "Sprite_Renderer_batch_vs.cso", g_pVertexShader.GetAddressOf(), g_pVertexLayout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	create_ps_from_cso(device, "Sprite_Renderer_batch_ps.cso", g_pPixelShader.GetAddressOf());

	instance* instances = new instance[MAX_INSTANCES];
	{
		D3D11_BUFFER_DESC buffer_desc = {};
		D3D11_SUBRESOURCE_DATA subresource_data = {};

		buffer_desc.ByteWidth = sizeof(instance) * MAX_INSTANCES;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = instances;
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, instance_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	delete[] instances;

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
	rasterizer_desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = FALSE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, g_pRasterizerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	load_texture_from_file(device, TexName, g_pShaderResourceView.GetAddressOf(), &g_TEXTURE2D_DESC);

	D3D11_SAMPLER_DESC sampler_desc;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampler_desc.MipLODBias = 0;
	sampler_desc.MaxAnisotropy = 16;
	sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	memcpy(sampler_desc.BorderColor, &XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(XMFLOAT4));
	sampler_desc.MinLOD = 0;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampler_desc, g_pSamplerState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.StencilEnable = FALSE;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;
	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, g_pDepthStencilState.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Sprite_Renderer_batch::begin(ID3D11DeviceContext* g_pImmediateContext)
{
	HRESULT hr = S_OK;

	UINT strides[2] = { sizeof(vertex), sizeof(instance) };
	UINT offsets[2] = { 0, 0 };
	ID3D11Buffer* vbs[2] = { g_pVertexBuffer.Get(), instance_buffer.Get() };
	g_pImmediateContext->IASetVertexBuffers(0, 2, vbs, strides, offsets);
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pImmediateContext->IASetInputLayout(g_pVertexLayout.Get());
	g_pImmediateContext->OMSetDepthStencilState(g_pDepthStencilState.Get(), 1);
	g_pImmediateContext->RSSetState(g_pRasterizerState.Get());
	g_pImmediateContext->VSSetShader(g_pVertexShader.Get(), nullptr, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader.Get(), nullptr, 0);
	g_pImmediateContext->PSSetShaderResources(0, 1, g_pShaderResourceView.GetAddressOf());
	g_pImmediateContext->PSSetSamplers(0, 1, g_pSamplerState.GetAddressOf());

	UINT num_viewports = 1;
	g_pImmediateContext->RSGetViewports(&num_viewports, &viewport);

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = g_pImmediateContext->Map(instance_buffer.Get(), 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	instances = static_cast<instance*>(mapped_buffer.pData);

	count_instance = 0;
}
void Sprite_Renderer_batch::render(ID3D11DeviceContext* g_pImmediateContext,
	float dx, float dy,
	float dw, float dh,
	float sx, float sy,
	float sw, float sh,
	float angle,
	float r, float g, float b, float a)
{
	_ASSERT_EXPR(count_instance < MAX_INSTANCES, L"Number of instances must be less than MAX_INSTANCES.");

	float cx = dw * 0.5f, cy = dh * 0.5f; //中心座標
	FLOAT c = cos(XMConvertToRadians(angle));
	FLOAT s = sin(XMConvertToRadians(angle));
	FLOAT w = 2.0f / viewport.Width;
	FLOAT h = -2.0f / viewport.Height;
	instances[count_instance].ndc_transform._11 = w * dw * c;
	instances[count_instance].ndc_transform._21 = h * dw * s;
	instances[count_instance].ndc_transform._31 = 0.0f;
	instances[count_instance].ndc_transform._41 = 0.0f;
	instances[count_instance].ndc_transform._12 = w * dh * -s;
	instances[count_instance].ndc_transform._22 = h * dh * c;
	instances[count_instance].ndc_transform._32 = 0.0f;
	instances[count_instance].ndc_transform._42 = 0.0f;
	instances[count_instance].ndc_transform._13 = 0.0f;
	instances[count_instance].ndc_transform._23 = 0.0f;
	instances[count_instance].ndc_transform._33 = 1.0f;
	instances[count_instance].ndc_transform._43 = 0.0f;
	instances[count_instance].ndc_transform._14 = w * (-cx * c + -cy * -s + cx + dx) - 1.0f;
	instances[count_instance].ndc_transform._24 = h * (-cx * s + -cy * c + cy + dy) + 1.0f;
	instances[count_instance].ndc_transform._34 = 0.0f;
	instances[count_instance].ndc_transform._44 = 1.0f;

	float tw = static_cast<float>(g_TEXTURE2D_DESC.Width);
	float th = static_cast<float>(g_TEXTURE2D_DESC.Height);
	instances[count_instance].texcoord_transform = XMFLOAT4(sx / tw, sy / th, sw / tw, sh / th);
	instances[count_instance].color = XMFLOAT4(r, g, b, a);

	count_instance++;
}

void Sprite_Renderer_batch::end(ID3D11DeviceContext* g_pImmediateContext)
{
	g_pImmediateContext->Unmap(instance_buffer.Get(), 0);

	g_pImmediateContext->DrawInstanced(4, count_instance, 0, 0);
}
*/