#include "Debug_Draw_Manager.h"
#include "Engine.h"
#include "BulletPhysics_Manager.h"
#include "Transform.h"
#include "Material.h"
using namespace std;
using namespace BeastEngine;
using namespace DirectX;
using Microsoft::WRL::ComPtr;

Debug_Draw_Manager::Debug_Draw_Manager()
{
	material = Material::Create("Shader\\Debug_Shader_VS.hlsl", "Shader\\Debug_Shader_PS.hlsl");
	lines.clear();

	grid_length = 70;
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Vector4);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_debug.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	default_color.m_activeObject = { 0, 1, 0 };
	default_color.m_deactivatedObject = { 0, 1, 0 };
	default_color.m_wantsDeactivationObject = { 0, 1, 1 };
	default_color.m_disabledDeactivationObject = { 1, 0, 0 };
	default_color.m_disabledSimulationObject = { 1, 1, 0 };
	default_color.m_aabb = { 1, 0, 0 };
	default_color.m_contactPoint = { 1, 1, 0 };
}

// ���C���`����s
void Debug_Draw_Manager::flushLines()
{
	if (lines.size() == 0) return;

	HRESULT hr;
	ComPtr<ID3D11Buffer> vbuff;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Line) * lines.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = lines.data();
	hr = DxSystem::device->CreateBuffer(&bd, &InitData, &vbuff);
	if (FAILED(hr))
	{
		lines.clear();
		return;
	}

	// ���_�o�b�t�@
	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	ID3D11Buffer* vb[1] = { vbuff.Get() };
	DxSystem::device_context->IASetVertexBuffers(0, 1, vb, &stride, &offset);

	Vector4 color = { 0, 0, 0, 1.0f };
	UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = DxSystem::device_context->Map(constant_buffer_debug.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &color, sizeof(Vector4));
		DxSystem::device_context->Unmap(constant_buffer_debug.Get(), subresourceIndex);
	}
	DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_debug.GetAddressOf());

	//�`��
	DxSystem::device_context->Draw(lines.size() * 2, 0);
	lines.clear();
}

void Debug_Draw_Manager::Render_Grid(shared_ptr<Transform>& trans)
{
	Set_Dx_Settings();

	std::vector<Line> grids;
	const btVector3 color = { 0, 0, 0 };
	const Vector3 pos = trans->Get_Position();
	int height_level = to_string(static_cast<int>(abs(pos.y))).length() - 1;

	btVector3 from, to;
	float X, Z;
	float color_level[2];
	int count[2];

	const float next_height = abs(pos.y) / powf(10.0f, static_cast<float>(height_level + 1));
	color_level[0] = 1 - next_height;
	color_level[1] = next_height;

	for (int i = 0; i < 2; ++i)
	{
		count[i] = static_cast<int>((static_cast<float>(grid_length) / (height_level + 1 + i)) + 1);
		const float level = powf(10.0f, static_cast<float>(height_level + i));
		float half_x = static_cast<float>(count[i] - 1) * 0.5f;
		float half_z = static_cast<float>(count[i] - 1) * 0.5f;
		int center_x = static_cast<int>(pos.x) / static_cast<int>(level) * static_cast<int>(level);
		int center_z = static_cast<int>(pos.z) / static_cast<int>(level) * static_cast<int>(level);

		for (int j = 0; j < count[i]; ++j)
		{
			X = static_cast<float>(center_x - half_x * level + j * level);
			Z = static_cast<float>(center_z - half_z * level);
			from = { X, 0, Z };

			Z = static_cast<float>(center_z + half_z * level);
			to = { X, 0, Z };
			grids.push_back(Line(from, to, color, color));

			Z = static_cast<float>(center_z - half_z * level + j * level);
			X = static_cast<float>(center_x - half_x * level);
			from = { X, 0, Z };

			X = static_cast<float>(center_x + half_x * level);
			to = { X, 0, Z };
			grids.push_back(Line(from, to, color, color));
		}
	}

	HRESULT hr;
	ComPtr<ID3D11Buffer> grid_vertex_buffer;
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Line) * grids.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = grids.data();
	hr = DxSystem::device->CreateBuffer(&bd, &InitData, grid_vertex_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// ���_�o�b�t�@
	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	DxSystem::device_context->IASetVertexBuffers(0, 1, grid_vertex_buffer.GetAddressOf(), &stride, &offset);

	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::Less), 1);

	const float alpha = 0.15f;
	Vector4 color_w = { 0, 0, 0,  color_level[0] * alpha };
	//�`��
	{
		UINT subresourceIndex = 0;
		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = DxSystem::device_context->Map(constant_buffer_debug.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (SUCCEEDED(hr))
		{
			memcpy(mapped.pData, &color_w, sizeof(Vector4));
			DxSystem::device_context->Unmap(constant_buffer_debug.Get(), subresourceIndex);
		}
		DxSystem::device_context->Draw(count[0] * 4, 0);
	}
	{
		color_w = { 0, 0, 0,  color_level[1] * alpha };
		UINT subresourceIndex = 0;
		D3D11_MAPPED_SUBRESOURCE mapped;
		auto hr = DxSystem::device_context->Map(constant_buffer_debug.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (SUCCEEDED(hr))
		{
			memcpy(mapped.pData, &color_w, sizeof(Vector4));
			DxSystem::device_context->Unmap(constant_buffer_debug.Get(), subresourceIndex);
		}
		DxSystem::device_context->Draw(count[1] * 4, count[0] * 4);
	}
}

void Debug_Draw_Manager::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	// �o�b�t�@�ɂ܂Ƃ߂āA���Ƃ���ꊇ�`��
	if (lines.size() >= max_line) return;
	lines.push_back(Line(from, to, fromColor, toColor));
}

void Debug_Draw_Manager::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	drawLine(from, to, color, color);
}

Debug_Draw_Manager::Vertex::Vertex(const btVector3& p, const btVector3& c)
{
	pos[0] = p[0];
	pos[1] = p[1];
	pos[2] = p[2];
	color[0] = c[0];
	color[1] = c[1];
	color[2] = c[2];
}

void Debug_Draw_Manager::Set_Dx_Settings()
{
	// �v���~�e�B�u�`��
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// �V�F�[�_
	material->Active_Shader();

	// �X�e�[�g
	DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(RS_State::Wire));
	DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(BS_State::Alpha), nullptr, 0xffffffff);

	DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_debug.GetAddressOf());
}

void Debug_Draw_Manager::Render_Collider()
{
	Set_Dx_Settings();
	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::None_No_Write), 0);

	Vector4 color_w = { 0, 0, 0, 0.5f };
	UINT subresourceIndex = 0;
	D3D11_MAPPED_SUBRESOURCE mapped;
	auto hr = DxSystem::device_context->Map(constant_buffer_debug.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped.pData, &color_w, sizeof(Vector4));
		DxSystem::device_context->Unmap(constant_buffer_debug.Get(), subresourceIndex);
	}
	Engine::bulletphysics_manager->Render_Debug();
}