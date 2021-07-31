#include "View_Game.h"
#include "DxSystem.h"
#include "Engine.h"
#include "GameObject.h"
#include "Particle_Manager.h"
#include "SkyBox.h"
#include "Transform.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

void View_Game::Render(Matrix V, Matrix P, std::shared_ptr<Transform>& camera_transform)
{
	//�V���h�E�}�b�v�`��
	Render_Shadow(camera_transform);

	//�ʏ�`��
	{
#if _DEBUG
		DxSystem::device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
		Clear();
#else
		// �����_�[�^�[�Q�b�g�r���[�ݒ�
		DxSystem::Set_Default_View();
#endif
		// �r���[�|�[�g�̐ݒ�
		DxSystem::Set_ViewPort(screen_x, screen_y);
		// �V�[���p�萔�o�b�t�@�X�V
		buffer_scene.view_projection_matrix = V * P;
		DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->UpdateSubresource(constant_buffer_scene.Get(), 0, 0, &buffer_scene, 0, 0);

		Render_Sky(camera_transform->Get_Position());
		Render_3D(V, P);
		Render_2D(V, P);

		Engine::particle_manager->Render();
	}
}