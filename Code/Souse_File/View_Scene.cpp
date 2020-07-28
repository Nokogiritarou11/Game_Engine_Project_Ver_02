#include "View_Scene.h"
#include "DxSystem.h"
#include "Render_Manager.h"
#include "GameObject.h"
using namespace std;
using namespace DirectX;

void View_Scene::Render(Matrix V, Matrix P)
{
	// ビューポートの設定
	DxSystem::SetViewPort(screen_x, screen_y);
	//レンダーターゲットの設定
	DxSystem::DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	Clear();

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