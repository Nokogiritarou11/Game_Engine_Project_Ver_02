#include "View_Game.h"
#include "DxSystem.h"
#include "Render_Manager.h"
#include "GameObject.h"
using namespace std;
using namespace DirectX;

void View_Game::Render(Matrix V, Matrix P)
{
	// ビューポートの設定
	DxSystem::SetViewPort(screen_x, screen_y);
	//レンダーターゲットの設定
	DxSystem::DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	Clear();

	const Matrix C = {
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	};

	// シーン用定数バッファ更新
	CbScene cbScene;
	cbScene.viewProjection = C * V * P;
	cbScene.lightDirection = { 0,-1,-1,0 };
	DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
	DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbScene.Get(), 0, 0, &cbScene, 0, 0);

	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(DxSystem::BS_NONE), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(DxSystem::RS_CULL_BACK));
	//トポロジー設定
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DxSystem::DS_TRUE), 1);

	for (list<weak_ptr<Renderer>>::iterator itr = Render_Manager::Renderer_3D_list.begin(); itr != Render_Manager::Renderer_3D_list.end();)
	{
		if (itr->expired())
		{
			itr = Render_Manager::Renderer_3D_list.erase(itr);
			continue;
		}
		shared_ptr<Renderer> m_rend = itr->lock();
		if (m_rend->gameObject->activeSelf())
		{
			if (m_rend->enabled)
			{
				m_rend->Render(V, P);
			}
		}
		itr++;
	}

	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(DxSystem::BS_ALPHA), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(DxSystem::RS_CULL_BACK));
	//トポロジー設定
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DxSystem::DS_FALSE), 1);
	for (list<weak_ptr<Renderer>>::iterator itr = Render_Manager::Renderer_2D_list.begin(); itr != Render_Manager::Renderer_2D_list.end();)
	{
		if (itr->expired())
		{
			itr = Render_Manager::Renderer_2D_list.erase(itr);
			continue;
		}
		shared_ptr<Renderer> m_rend = itr->lock();
		if (m_rend->gameObject->activeSelf())
		{
			if (m_rend->enabled)
			{
				m_rend->Render(V, P);
			}
		}
		itr++;
	}
}