#include "View_Scene.h"
#include "DxSystem.h"
#include "Engine.h"
#include "GameObject.h"
#include "Particle_Manager.h"
#include "Shadow_Manager.h"
#include "SkyBox.h"
#include "Transform.h"
using namespace std;
using namespace DirectX;

void View_Scene::Render(Matrix V, Matrix P, std::shared_ptr<Transform> camera_transform)
{
	//�V���h�E�}�b�v�`��
	Render_Shadow(camera_transform);

	//�ʏ�`��
	{
		DxSystem::DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
		Clear();

		// �r���[�|�[�g�̐ݒ�
		DxSystem::SetViewPort(screen_x, screen_y);
		// �V�[���p�萔�o�b�t�@�X�V
		cb_scene.viewProjection = V * P;
		DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
		DxSystem::DeviceContext->PSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
		DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbScene.Get(), 0, 0, &cb_scene, 0, 0);

		Render_Sky(camera_transform->Get_position());
		Engine::shadow_manager->Set_PS_Resource();
		Render_3D(V, P);

		Engine::particle_manager->Render();
	}
}