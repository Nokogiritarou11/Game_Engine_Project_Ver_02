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

SkyBox::SkyBox()
{
	material = make_shared<Material>();
	ifstream in_bin("Default_Resource\\Model\\sphere\\SkyBoxes\\envmap_miramar\\envmap_miramar.mat", ios::binary);
	if (in_bin.is_open())
	{
		stringstream bin_s_stream;
		bin_s_stream << in_bin.rdbuf();
		cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
		binaryInputArchive(material);
		Material::Initialize(material, "Default_Resource\\Model\\sphere\\SkyBoxes\\envmap_miramar\\envmap_miramar.mat");
	}
	else
	{
		shared_ptr<Material> mat = Material::Create("Default_Resource\\Model\\sphere\\SkyBoxes\\envmap_miramar\\", "envmap_miramar", L"Shader/SkyBox_Shader_PS.hlsl");
		mat->Set_Texture(Material::Texture_Type::Main, "Default_Resource\\Model\\sphere\\skyboxes\\envmap_miramar\\", "envmap_miramar.dds");
		mat->Save();
		ifstream bin("Default_Resource\\Model\\sphere\\SkyBoxes\\envmap_miramar\\envmap_miramar.mat", ios::binary);
		if (bin.is_open())
		{
			stringstream bin_s_stream;
			bin_s_stream << bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(material);
			Material::Initialize(material, "Default_Resource\\Model\\sphere\\SkyBoxes\\envmap_miramar\\envmap_miramar.mat");
		}
	}

	//Engine::fbx_converter->Direct_Load("Default_Resource\\Model\\sphere\\", "sphere", true, false, false);
	mesh = Mesh::Load_Mesh("Default_Resource\\Model\\sphere\\sphere");

	// 定数バッファの生成
	if (!constant_buffer_skybox)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Constant_Buffer_Skybox);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_skybox.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	if (!vertex_shader)
	{
		vertex_shader = Shader::Create("Shader/SkyBox_Shader_VS.hlsl", "");
	}
}

SkyBox::~SkyBox() {}

void SkyBox::Render(Vector3 pos)
{
	DxSystem::device_context->IASetInputLayout(vertex_shader->vertex_layout.Get());
	//シェーダーリソースのバインド
	material->texture[static_cast<int>(Material::Texture_Type::Main)]->Set(1); //PSSetSamplar PSSetShaderResources

	vertex_shader->Activate_VS();
	material->shader->Activate_PS(); //PS,VSSetShader

	// 定数バッファ更新
	Constant_Buffer_Skybox cbSkyBox;
	::memset(&cbSkyBox, 0, sizeof(cbSkyBox));

	//const float angle = 0;
	//float bias = 0.2f;
	//float amplitude = 10.0f;
	//float alpha = std::min<float>(1, std::max<float>(bias, amplitude * -cosf(angle * 0.01745f)));
	float alpha = 1;
	float blue_intensity = 1.0f;
	Vector4 color(1, 1, 1 * blue_intensity, alpha);

	cbSkyBox.color = color;

	cbSkyBox.world = Matrix::Identity;
	float sky_dimension = 50000;
	const Matrix scale = Matrix::CreateScale(sky_dimension, sky_dimension * 0.8f, sky_dimension);
	//const Matrix trans = Matrix::CreateTranslation(pos.x, pos.y, pos.z);
	cbSkyBox.world = scale;

	DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_skybox.GetAddressOf());
	DxSystem::device_context->PSSetConstantBuffers(1, 1, constant_buffer_skybox.GetAddressOf());
	UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	auto hr = DxSystem::device_context->Map(constant_buffer_skybox.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &cbSkyBox, sizeof(Constant_Buffer_Skybox));
		DxSystem::device_context->Unmap(constant_buffer_skybox.Get(), subresourceIndex);
	}

	// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
	UINT stride = sizeof(Mesh::vertex);
	UINT offset = 0;
	DxSystem::device_context->IASetVertexBuffers(0, 1, mesh->vertex_buffer.GetAddressOf(), &stride, &offset);
	DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	for (auto& subset : mesh->subsets)
	{
		// ↑で設定したリソースを利用してポリゴンを描画する。
		DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
	}
}