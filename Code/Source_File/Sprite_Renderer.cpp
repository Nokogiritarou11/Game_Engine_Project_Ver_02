#include "Sprite_Renderer.h"
#include "DxSystem.h"
#include "GameObject.h"
#include "Engine.h"
#include "Screen.h"
#include "Render_Texture.h"
#include "Render_Manager.h"
#include "Include_ImGui.h"
#include "Debug.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "System_Function.h"
#include "Asset_Manager.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

void Sprite_Renderer::Initialize(const shared_ptr<GameObject>& obj)
{
	enabled_old = enabled;

	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;

	Vertex v[] = {
		Vector3(-0.5f, 0.5f,0), Vector2(0,0), Vector4(1,1,1,1), //左上
		Vector3(0.5f, 0.5f,0),  Vector2(1,0), Vector4(1,1,1,1), //右上
		Vector3(-0.5f,-0.5f,0), Vector2(0,1), Vector4(1,1,1,1), //左下
		Vector3(0.5f,-0.5f,0),  Vector2(1,1), Vector4(1,1,1,1), //右下
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	DxSystem::device->CreateBuffer(&bd, &res, vertex_buffer.GetAddressOf());

	if (file_path != "")
	{
		texture = Texture::Load(file_path + file_name);
	}
	else
	{
		texture = Texture::Load("Default_Assets\\Image\\Default_Texture.png");
	}
	can_render = true;

	Set_Active(Get_Enabled());
}

void Sprite_Renderer::Set_Active(bool value)
{
	if (value)
	{
		if (!is_called)
		{
			if (gameobject->Get_Active())
			{
				if (Get_Enabled())
				{
					Engine::render_manager->Add(static_pointer_cast<Sprite_Renderer>(shared_from_this()));
					is_called = true;
				}
			}
		}
	}
}

void Sprite_Renderer::Recalculate_Frame()
{
	if (can_render)
	{
		//頂点データ設定
		Vertex data[4];

		Vector3 trans_pos = gameobject->transform->Get_Position();

		data[0].pos.x = trans_pos.x;
		data[0].pos.y = trans_pos.y;
		data[0].pos.z = 0.0f;

		data[1].pos.x = trans_pos.x + size.x;//trans->Width;
		data[1].pos.y = trans_pos.y;
		data[1].pos.z = 0.0f;

		data[2].pos.x = trans_pos.x;
		data[2].pos.y = trans_pos.y + size.y;//trans->Height;
		data[2].pos.z = 0.0f;

		data[3].pos.x = trans_pos.x + size.x;//trans->Width;
		data[3].pos.y = trans_pos.y + size.y;//trans->Height;
		data[3].pos.z = 0.0f;

		// 中心座標を原点へ
		float mx = trans_pos.x + size.x * 0.5f;
		float my = trans_pos.y + size.y * 0.5f;
		data[0].pos.x -= mx; data[0].pos.y -= my;
		data[1].pos.x -= mx; data[1].pos.y -= my;
		data[2].pos.x -= mx; data[2].pos.y -= my;
		data[3].pos.x -= mx; data[3].pos.y -= my;

		// 角度計算
		float rx, ry;
		float z = transform->Get_Euler_Angles().z;
		float cos = cosf(XMConvertToRadians(z));
		float sin = sinf(XMConvertToRadians(z));

		rx = data[0].pos.x;
		ry = data[0].pos.y;
		data[0].pos.x = cos * rx + -sin * ry;
		data[0].pos.y = sin * rx + cos * ry;

		rx = data[1].pos.x;
		ry = data[1].pos.y;
		data[1].pos.x = cos * rx + -sin * ry;
		data[1].pos.y = sin * rx + cos * ry;

		rx = data[2].pos.x;
		ry = data[2].pos.y;
		data[2].pos.x = cos * rx + -sin * ry;
		data[2].pos.y = sin * rx + cos * ry;

		rx = data[3].pos.x;
		ry = data[3].pos.y;
		data[3].pos.x = cos * rx + -sin * ry;
		data[3].pos.y = sin * rx + cos * ry;

		// 動かした分戻す
		data[0].pos.x += mx; data[0].pos.y += my;
		data[1].pos.x += mx; data[1].pos.y += my;
		data[2].pos.x += mx; data[2].pos.y += my;
		data[3].pos.x += mx; data[3].pos.y += my;

		// 正規化デバイス座標系
		for (int i = 0; i < 4; i++)
		{
			data[i].pos.x = 2.0f * data[i].pos.x / Engine::render_manager->game_texture->screen_x - 1.0f;
			data[i].pos.y = 1.0f - 2.0f * data[i].pos.y / Engine::render_manager->game_texture->screen_y;
			data[i].pos.z = 0.0f;
		}

		//テクスチャ座標設定
		data[0].tex.x = uv_origin.x;
		data[0].tex.y = uv_origin.y;
		data[1].tex.x = uv_origin.x + uv_size.x;
		data[1].tex.y = uv_origin.y;
		data[2].tex.x = uv_origin.x;
		data[2].tex.y = uv_origin.y + uv_size.y;
		data[3].tex.x = uv_origin.x + uv_size.x;
		data[3].tex.y = uv_origin.y + uv_size.y;

		//UV座標
		float w = static_cast<float>(texture->Get_Width());
		float h = static_cast<float>(texture->Get_Height());
		for (int i = 0; i < 4; i++)
		{
			data[i].tex.x = data[i].tex.x / w;
			data[i].tex.y = data[i].tex.y / h;
		}
		//頂点カラー
		data[0].color = color;
		data[1].color = color;
		data[2].color = color;
		data[3].color = color;


		UINT subresourceIndex = 0;
		D3D11_MAPPED_SUBRESOURCE mapped;
		auto hr = DxSystem::device_context->Map(vertex_buffer.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (SUCCEEDED(hr))
		{
			memcpy(mapped.pData, data, sizeof(data));
			DxSystem::device_context->Unmap(vertex_buffer.Get(), subresourceIndex);
		}
	}
}

void Sprite_Renderer::Render(int subset_number)
{
	if (can_render)
	{
		//	頂点バッファの指定
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		DxSystem::device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
		//テクスチャの設定
		texture->Set(1, Shader::Shader_Type::Pixel);

		// 描画
		DxSystem::device_context->Draw(4, 0);
	}
}

bool Sprite_Renderer::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Sprite_Renderer", open)) return false;

	if (open)
	{
		const float window_width = ImGui::GetWindowContentRegionWidth();
		{
			const ImVec2& size = ImVec2(100.0f, 100.0f);
			const ImVec2& uv0 = ImVec2(0.0f, 0.0f);
			const ImVec2& uv1 = ImVec2(1.0f, 1.0f);
			const int& frame_padding = 3;
			const ImVec4& bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			const ImVec4& tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::Text(u8"現在のテクスチャ::");
			ImGui::SameLine();
			ImGui::Text(file_name.c_str());

			ImGui::Dummy({ 0,0 });
			ImGui::SameLine(window_width - size.x - frame_padding);
			if (ImGui::ImageButton((void*)texture->Get_Resource().Get(), size, uv0, uv1, frame_padding, bg_col, tint_col))
			{
				string path = System_Function::Get_Open_File_Name("png", "\\Assets\\Image");
				if (path != "")
				{
					int path_i = path.find_last_of("\\") + 1;
					int ext_i = path.find_last_of(".");
					string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
					string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
					string filename = path.substr(path_i, ext_i - path_i); //ファイル名
					texture = Texture::Load(path);
					file_path = pathname;
					file_name = filename + extname;
				}
			}
		}

		float edit_size[2] = { size.x, size.y };
		int edit_uv_origin[2] = { static_cast<int>(uv_origin.x), static_cast<int>(uv_origin.y) };
		int edit_uv_size[2] = { static_cast<int>(uv_size.x),static_cast<int>(uv_size.y) };

		ImGui::Text(u8"表示サイズ");
		ImGui::SameLine(window_width * 0.4f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragFloat2("##size", edit_size, 0.1f, -FLT_MAX, FLT_MAX))
		{
			size = { edit_size[0],edit_size[1] };
		}

		ImGui::Text(u8"UV始点");
		ImGui::SameLine(window_width * 0.4f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragInt2("##uv_origin", edit_uv_origin, 0, INT_MAX))
		{
			uv_origin = { static_cast<float>(edit_uv_origin[0]),static_cast<float>(edit_uv_origin[1]) };
		}

		ImGui::Text(u8"UVサイズ");
		ImGui::SameLine(window_width * 0.4f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragInt2("##uv_size", edit_uv_size, 0, INT_MAX))
		{
			uv_size = { static_cast<float>(edit_uv_size[0]),static_cast<float>(edit_uv_size[1]) };
		}

		float edit_color[4] = { color.x,color.y,color.z,color.w };
		if (ImGui::ColorEdit4("Color", edit_color))
		{
			color = { edit_color[0],edit_color[1] ,edit_color[2] ,edit_color[3] };
		}
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
		{ Vector3(0, 0, 0), Vector2(0, 0) },
		{ Vector3(1, 0, 0), Vector2(1, 0) },
		{ Vector3(0, 1, 0), Vector2(0, 1) },
		{ Vector3(1, 1, 0), Vector2(1, 1) },
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
	rasterizer_desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WireFRAME, D3D11_FILL_SOLID
	rasterizer_desc.CullMode = D3D11_Cull_None; //D3D11_Cull_None, D3D11_Cull_Front, D3D11_Cull_Back
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
	memcpy(sampler_desc.BorderColor, &Vector4(0.0f, 0.0f, 0.0f, 0.0f), sizeof(Vector4));
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
	instances[count_instance].texcoord_transform = Vector4(sx / tw, sy / th, sw / tw, sh / th);
	instances[count_instance].color = Vector4(r, g, b, a);

	count_instance++;
}

void Sprite_Renderer_batch::end(ID3D11DeviceContext* g_pImmediateContext)
{
	g_pImmediateContext->Unmap(instance_buffer.Get(), 0);

	g_pImmediateContext->DrawInstanced(4, count_instance, 0, 0);
}
*/