#include "Render_Manager.h"
#include "GameObject.h"
#include "Transform.h"
#include "Animator_Manager.h"
using namespace std;

list<weak_ptr<Renderer>> Render_Manager::Renderer_3D_list;
list<weak_ptr<Renderer>> Render_Manager::Renderer_2D_list;

void Render_Manager::Reset()
{
	Renderer_3D_list.clear();
	Renderer_2D_list.clear();
}

void Render_Manager::Add(shared_ptr<Mesh_Renderer> m_rend)
{
	Renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<SkinMesh_Renderer> m_rend)
{
	Renderer_3D_list.emplace_back(m_rend);
}
void Render_Manager::Add(shared_ptr<Sprite_Renderer> m_rend)
{
	Renderer_2D_list.emplace_back(m_rend);
}

void Render_Manager::Render(std::shared_ptr<Camera> Render_Camera)
{
	// ビューポートの設定
	DxSystem::SetViewPort(DxSystem::GetScreenWidth(), DxSystem::GetScreenHeight());
	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(DxSystem::BS_NONE), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(DxSystem::RS_CULL_NONE));
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DxSystem::DS_TRUE), 1);

	Animator_Manager::Update();

	for (list<weak_ptr<Renderer>>::iterator itr = Renderer_3D_list.begin(); itr != Renderer_3D_list.end();)
	{
		if (itr->expired())
		{
			itr = Renderer_3D_list.erase(itr);
			continue;
		}
		shared_ptr<Renderer> m_rend = itr->lock();
		if (m_rend->gameObject->activeSelf())
		{
			if (m_rend->enabled)
			{
				m_rend->Render(Render_Camera);
			}
		}
		itr++;
	}

	//ブレンドステート設定
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(DxSystem::BS_ALPHA), nullptr, 0xFFFFFFFF);
	//ラスタライザ―設定
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(DxSystem::RS_CULL_BACK));
	//デプスステンシルステート設定
	DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(DxSystem::DS_FALSE), 1);
	for (list<weak_ptr<Renderer>>::iterator itr = Renderer_2D_list.begin(); itr != Renderer_2D_list.end();)
	{
		if (itr->expired())
		{
			itr = Renderer_2D_list.erase(itr);
			continue;
		}
		shared_ptr<Renderer> m_rend = itr->lock();
		if (m_rend->gameObject->activeSelf())
		{
			if (m_rend->enabled)
			{
				m_rend->Render(Render_Camera);
			}
		}
		itr++;
	}

}