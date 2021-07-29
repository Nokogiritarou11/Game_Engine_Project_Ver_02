#include "Debug_Draw_Manager.h"

using namespace std;
using namespace BeastEngine;
using namespace DirectX;
using Microsoft::WRL::ComPtr;

Debug_Draw_Manager::Debug_Draw_Manager()
{
	shader = Shader::Create("Shader/Debug_Shader_VS.hlsl", "Shader/Debug_Shader_PS.hlsl");
	lines.clear();

	default_color.m_activeObject               = { 0, 1, 0 };
	default_color.m_deactivatedObject          = { 0, 1, 0 };
	default_color.m_wantsDeactivationObject    = { 0, 1, 1 };
	default_color.m_disabledDeactivationObject = { 1, 0, 0 };
	default_color.m_disabledSimulationObject   = { 1, 1, 0 };
	default_color.m_aabb                       = { 1, 0, 0 };
	default_color.m_contactPoint               = { 1, 1, 0 };
}

// ライン描画実行
void Debug_Draw_Manager::flushLines()
{
	if (lines.size() == 0) return;

	//頂点データ構造体
	struct VertexData
	{
		XMFLOAT3 pos;
		XMFLOAT3 col;
	};
	ComPtr<ID3D11Buffer> vbuff, idxbuff;

	HRESULT hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(Line) * lines.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = lines.data();
	hr = DxSystem::device->CreateBuffer(&bd, &InitData, &vbuff);
	if (FAILED(hr))
	{
		lines.clear();
		return;
	}

	// 頂点バッファ
	UINT stride = sizeof(VertexData);
	UINT offset = 0;
	ID3D11Buffer* vb[1] = { vbuff.Get() };
	DxSystem::device_context->IASetVertexBuffers(0, 1, vb, &stride, &offset);

	// プリミティブ形状
	DxSystem::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// シェーダ
	DxSystem::device_context->IASetInputLayout(shader->vertex_layout.Get());
	shader->Activate_VS();
	shader->Activate_PS();

	// ステート
	DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(RS_State::Wire));
	DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(BS_State::Alpha_Test), nullptr, 0xffffffff);
	DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(DS_State::None_No_Write), 0);

	//描画
	DxSystem::device_context->Draw(lines.size() * 2, 0);

	//DxSystem::device_context->OMSetDepthStencilState(ds_state_on.Get(), 0);
	//DxSystem::device_context->Draw(lines.size() * 2, 0);


	lines.clear();
	DxSystem::device_context->VSSetShader(NULL, NULL, 0);
	DxSystem::device_context->GSSetShader(NULL, NULL, 0);
	DxSystem::device_context->PSSetShader(NULL, NULL, 0);

}

void Debug_Draw_Manager::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	// バッファにまとめて、あとから一括描画
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