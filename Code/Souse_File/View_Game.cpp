#include "View_Game.h"
#include "DxSystem.h"
#include "Render_Manager.h"
#include "GameObject.h"
using namespace std;
using namespace DirectX;

void View_Game::Render(Matrix V, Matrix P)
{
	// �r���[�|�[�g�̐ݒ�
	DxSystem::SetViewPort(screen_x, screen_y);
	//�����_�[�^�[�Q�b�g�̐ݒ�
	DxSystem::DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
	Clear();

	const Matrix C = {
		1, 0, 0, 0,
		0, 0, 1, 0,
		0, 1, 0, 0,
		0, 0, 0, 1
	};

	// �V�[���p�萔�o�b�t�@�X�V
	CbScene cbScene;
	cbScene.viewProjection = C * V * P;
	cbScene.lightDirection = { 0,-1,-1,0 };
	DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
	DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbScene.Get(), 0, 0, &cbScene, 0, 0);

	//�u�����h�X�e�[�g�ݒ�
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(DxSystem::BS_NONE), nullptr, 0xFFFFFFFF);
	//���X�^���C�U�\�ݒ�
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(DxSystem::RS_CULL_BACK));
	//�g�|���W�[�ݒ�
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
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

	//�u�����h�X�e�[�g�ݒ�
	DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(DxSystem::BS_ALPHA), nullptr, 0xFFFFFFFF);
	//���X�^���C�U�\�ݒ�
	DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(DxSystem::RS_CULL_BACK));
	//�g�|���W�[�ݒ�
	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//�f�v�X�X�e���V���X�e�[�g�ݒ�
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