#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "SkyBox.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Misc.h"
#include "Renderer.h"
using namespace std;
using namespace BeastEngine;
using namespace DirectX;

SkyBox::SkyBox()
{
	material = Material::Create("Shader\\SkyBox_Shader_VS.hlsl", "Shader\\SkyBox_Shader_PS.hlsl");
	material->Set_Texture("cubemap", Texture::Load("Default_Assets\\Image\\SkyBox\\envmap_miramar.dds"));
	material->Set_Blend_State(BS_State::Off);
	material->Set_Rasterizer_State(RS_State::Cull_None);
	material->Set_Depth_Stencil_State(DS_State::None_No_Write);

	constexpr int u_max = 10;
	constexpr int v_max = 10;
	constexpr u_int vertex_count = ((u_max - 2) * v_max) + 2;
	index_count = (((u_max - 3) * (v_max) * 2) + (v_max * 2)) * 3;

	float sphereYaw, spherePitch = 0.0f;
	Matrix Rotationx, Rotationy;
	Vector3 currVertPos;

	vector<Vector3> vertices(vertex_count);

	vertices[0] = { 0.0f, 0.0f, 1.0f };
	vertices[vertex_count - 1] = { 0.0f, 0.0f, -1.0f };
	for (u_int i = 0; i < u_max - 2; ++i)
	{
		spherePitch = (i + 1) * (3.14f / (u_max - 1));
		Rotationx = XMMatrixRotationX(spherePitch);
		for (u_int j = 0; j < v_max; ++j)
		{
			sphereYaw = j * (6.28f / (v_max));
			Rotationy = XMMatrixRotationZ(sphereYaw);
			currVertPos = XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy));
			vertices[i * v_max + j + 1] = XMVector3Normalize(currVertPos);
		}
	}

	vector<u_int> indices;
	indices.resize(index_count);
	int k = 0;
	for (u_int l = 0; l < v_max - 1; ++l)
	{
		indices[k] = 0;
		indices[k + 1] = l + 1;
		indices[k + 2] = l + 2;
		k += 3;
	}

	indices[k] = 0;
	indices[k + 1] = v_max;
	indices[k + 2] = 1;
	k += 3;

	for (u_int i = 0; i < u_max - 3; ++i)
	{
		for (u_int j = 0; j < v_max - 1; ++j)
		{
			indices[k] = i * v_max + j + 1;
			indices[k + 1] = i * v_max + j + 2;
			indices[k + 2] = (i + 1) * v_max + j + 1;

			indices[k + 3] = (i + 1) * v_max + j + 1;
			indices[k + 4] = i * v_max + j + 2;
			indices[k + 5] = (i + 1) * v_max + j + 2;

			k += 6; // next quad
		}

		indices[k] = (i * v_max) + v_max;
		indices[k + 1] = (i * v_max) + 1;
		indices[k + 2] = ((i + 1) * v_max) + v_max;

		indices[k + 3] = ((i + 1) * v_max) + v_max;
		indices[k + 4] = (i * v_max) + 1;
		indices[k + 5] = ((i + 1) * v_max) + 1;

		k += 6;
	}

	for (u_int l = 0; l < v_max - 1; ++l)
	{
		indices[k] = vertex_count - 1;
		indices[k + 1] = (vertex_count - 1) - (l + 1);
		indices[k + 2] = (vertex_count - 1) - (l + 2);
		k += 3;
	}

	indices[k] = vertex_count - 1;
	indices[k + 1] = (vertex_count - 1) - v_max;
	indices[k + 2] = vertex_count - 2;

	// 頂点バッファの生成
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Vector3) * vertices.size();
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = &vertices[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, &InitData, vertex_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// インデックスバッファの生成
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(u_int) * indices.size();
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = &indices[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, &InitData, index_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void SkyBox::Render(const Vector3& pos)
{
	// 定数バッファ更新
	constexpr float sky_dimension = 5000;
	Matrix world = Matrix::CreateScale(sky_dimension) * Matrix::CreateTranslation(pos);
	material->Set_Matrix("world", world);

	//シェーダーリソースのバインド
	material->Active();

	// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
	UINT stride = sizeof(Vector3);
	UINT offset = 0;
	DxSystem::device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	DxSystem::device_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	DxSystem::device_context->DrawIndexed(index_count, 0, 0);
}