#include "SkyBox.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
using namespace std;

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

	mesh_data = Mesh::Load_Mesh("Default_Resource\\Model\\sphere\\", "sphere");

	// 定数バッファの生成
	if (!ConstantBuffer_CbSkyBox)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CbSkyBox);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer_CbSkyBox.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	if (!vertex_shader)
	{
		vertex_shader = make_unique<Shader>();
		vertex_shader->Create_VS(L"Shader/SkyBox_Shader_VS.hlsl", "VSMain");
	}
}

void SkyBox::Render(Vector3 pos)
{
	DxSystem::DeviceContext->IASetInputLayout(material->shader->VertexLayout.Get());
	//シェーダーリソースのバインド
	material->texture[static_cast<int>(Texture::Texture_Type::Main)]->Set(1); //PSSetSamplar PSSetShaderResources

	vertex_shader->Activate_VS();
	material->shader->Activate_PS(); //PS,VSSetShader

	// 定数バッファ更新
	CbSkyBox cbSkyBox;
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

	DxSystem::DeviceContext->VSSetConstantBuffers(1, 1, ConstantBuffer_CbSkyBox.GetAddressOf());
	DxSystem::DeviceContext->PSSetConstantBuffers(1, 1, ConstantBuffer_CbSkyBox.GetAddressOf());
	DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbSkyBox.Get(), 0, 0, &cbSkyBox, 0, 0);

	for (auto& mesh : mesh_data->meshes)
	{
		// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
		UINT stride = sizeof(Mesh::vertex);
		UINT offset = 0;
		DxSystem::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DxSystem::DeviceContext->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		for (auto& subset : mesh.subsets)
		{
			// ↑で設定したリソースを利用してポリゴンを描画する。
			DxSystem::DeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}
}