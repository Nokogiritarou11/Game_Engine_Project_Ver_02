#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "SkyBox.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
//#include "Engine.h"
//#include "FBX_Converter.h"
using namespace std;
using namespace BeastEngine;
using namespace DirectX;

SkyBox::SkyBox()
{
	material = Material::Create("Shader\\SkyBox_Shader_VS.hlsl", "Shader\\SkyBox_Shader_PS.hlsl");
	material->Set_Texture(Material::Texture_Type::Main, "Default_Resource\\Image\\SkyBox\\", "envmap_miramar.dds");

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

	// 定数バッファの生成
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Matrix);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_skybox.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

void SkyBox::Render(Vector3& pos)
{
	//シェーダーリソースのバインド
	material->texture[static_cast<int>(Material::Texture_Type::Main)]->Set(1);
	material->Active_Shader();

	// 定数バッファ更新
	constexpr float sky_dimension = 5000;
	const Matrix cbSkyBox = Matrix::CreateScale(sky_dimension) * Matrix::CreateTranslation(pos);

	DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_skybox.GetAddressOf());
	UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	auto hr = DxSystem::device_context->Map(constant_buffer_skybox.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &cbSkyBox, sizeof(Matrix));
		DxSystem::device_context->Unmap(constant_buffer_skybox.Get(), subresourceIndex);
	}

	// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
	UINT stride = sizeof(Vector3);
	UINT offset = 0;
	DxSystem::device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	DxSystem::device_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	DxSystem::device_context->DrawIndexed(index_count, 0, 0);
}