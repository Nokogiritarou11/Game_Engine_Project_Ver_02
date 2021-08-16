#include "Gaussian_Filter.h"
#include "Material.h"

using namespace BeastEngine;
using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;

Gaussian_Filter::Gaussian_Filter(const Vector2& size, const DXGI_FORMAT format, const float& dispersion)
{
	material[0] = Material::Create("Shader/Gaussian_Shader_x_VS.hlsl", "Shader/Gaussian_Shader_x_PS.hlsl");
	material[1] = Material::Create("Shader/Gaussian_Shader_y_VS.hlsl", "Shader/Gaussian_Shader_y_PS.hlsl");
	Create_Render_Target(size, format);
	Set_Weight(dispersion);

	//���_�o�b�t�@
	Vertex v[] = {
		Vector3(-1, 1, 0),  Vector2(0,0),
		Vector3(1, 1, 0), Vector2(1,0),
		Vector3(-1,-1,0),  Vector2(0,1),
		Vector3(1,-1,0), Vector2(1,1)
	};

	//	���_�o�b�t�@�쐬
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

	//	�T���v���X�e�[�g�쐬
	D3D11_SAMPLER_DESC sd = {};
	sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP; // U
	sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP; // V
	sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP; // W
	sd.MipLODBias = 0;
	sd.MaxAnisotropy = 0; // �ő�ٕ���(1Pixel������̃e�N�X�`���_��)
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;

	auto hr = DxSystem::device->CreateSamplerState(&sd, sampler.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Gaussian_Filter::Set_Weight(const float& dispersion)
{
	float total = 0.0f;
	const int NUM_WEIGHT = 8;
	float weights[NUM_WEIGHT];

	for (int i = 0; i < NUM_WEIGHT; i++)
	{
		float pos = (float)i * 2.0f;
		weights[i] = expf(-pos * pos * dispersion);
		total += weights[i];
	}

	for (int i = 0; i < NUM_WEIGHT; i++)
	{
		weights[i] = weights[i] / total * 0.5f;
	}

	//�萔�o�b�t�@�X�V
	Vector4 weight_0 = { weights[0], weights[1], weights[2], weights[3] };
	Vector4 weight_1 = { weights[4], weights[5], weights[6], weights[7] };
	for (size_t i = 0; i < 2; ++i)
	{
		material[i]->Set_Vector4("weight_0", weight_0);
		material[i]->Set_Vector4("weight_1", weight_1);
	}
}

void Gaussian_Filter::Create_Render_Target(const Vector2& size, const DXGI_FORMAT format)
{
	texture_size = size;
	for (size_t i = 0; i < 2; ++i)
	{
		// 2�����e�N�X�`���̐ݒ�
		D3D11_TEXTURE2D_DESC texDesc;
		memset(&texDesc, 0, sizeof(texDesc));
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.Format = format;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.Width = static_cast<UINT>(size.x);
		texDesc.Height = static_cast<UINT>(size.y);
		texDesc.CPUAccessFlags = 0;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		//texDesc.SampleDesc = DxSystem::MSAA;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		// 2�����e�N�X�`���̐���
		HRESULT hr = DxSystem::device->CreateTexture2D(&texDesc, nullptr, target[i].texture_render_target.GetAddressOf());

		// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		memset(&rtvDesc, 0, sizeof(rtvDesc));
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		// �����_�[�^�[�Q�b�g�r���[�̐���
		hr = DxSystem::device->CreateRenderTargetView(target[i].texture_render_target.Get(), &rtvDesc, target[i].render_target_view.GetAddressOf());

		// �V�F�[�_���\�[�X�r���[�̐ݒ�
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		memset(&srvDesc, 0, sizeof(srvDesc));
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		// �V�F�[�_���\�[�X�r���[�̐���
		hr = DxSystem::device->CreateShaderResourceView(target[i].texture_render_target.Get(), &srvDesc, target[i].shader_resource_view.GetAddressOf());

		//�萔�o�b�t�@�X�V
		Vector2 offset = { 16.0f / texture_size.x, 16.0f / texture_size.y };
		material[i]->Set_Vector2("offset", offset);
		material[i]->Set_Vector2("size", texture_size);
	}
}

void Gaussian_Filter::Filtering_Gaussian(const ComPtr<ID3D11ShaderResourceView>& srv)
{
	//===================================================================
	// Pass1 : �������ɂڂ���
	//===================================================================

	// RTV�ݒ�
	float clearColor[4] = { 0,0,0,0 };
	DxSystem::device_context->OMSetRenderTargets(1, target[0].render_target_view.GetAddressOf(), nullptr);
	DxSystem::device_context->ClearRenderTargetView(target[0].render_target_view.Get(), clearColor);

	// ���_�o�b�t�@�̎w��
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	DxSystem::device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ���\�[�X�ݒ�
	DxSystem::device_context->PSSetShaderResources(0, 1, srv.GetAddressOf());
	DxSystem::device_context->PSSetSamplers(0, 1, sampler.GetAddressOf());

	// �}�e���A��(�萔�o�b�t�@�X�V)
	material[0]->Active();

	DxSystem::device_context->Draw(4, 0);

	//===================================================================
	// Pass2 : �c�����ɂڂ���
	//===================================================================

	// RTV�ݒ�
	DxSystem::device_context->OMSetRenderTargets(1, target[1].render_target_view.GetAddressOf(), nullptr);
	DxSystem::device_context->ClearRenderTargetView(target[1].render_target_view.Get(), clearColor);

	// ���_�o�b�t�@�̎w��
	DxSystem::device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ���\�[�X�ݒ�
	DxSystem::device_context->PSSetShaderResources(0, 1, target[0].shader_resource_view.GetAddressOf());
	DxSystem::device_context->PSSetSamplers(0, 1, sampler.GetAddressOf());

	// �}�e���A��(�萔�o�b�t�@�X�V)
	material[1]->Active();

	DxSystem::device_context->Draw(4, 0);
}