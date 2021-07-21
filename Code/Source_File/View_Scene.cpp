#include "View_Scene.h"
#include "DxSystem.h"
#include "Engine.h"
#include "GameObject.h"
#include "Particle_Manager.h"
#include "SkyBox.h"
#include "Transform.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

void View_Scene::Render(Matrix V, Matrix P, std::shared_ptr<Transform> camera_transform)
{
	//シャドウマップ描画
	Render_Shadow(camera_transform);

	//通常描画
	{
		DxSystem::device_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
		Clear();

		// ビューポートの設定
		DxSystem::Set_ViewPort(screen_x, screen_y);
		// シーン用定数バッファ更新
		buffer_scene.view_projection_matrix = V * P;
		DxSystem::device_context->VSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->PSSetConstantBuffers(0, 1, constant_buffer_scene.GetAddressOf());
		DxSystem::device_context->UpdateSubresource(constant_buffer_scene.Get(), 0, 0, &buffer_scene, 0, 0);

		Render_Sky(camera_transform->Get_Position());
		Render_3D(V, P);

		Engine::particle_manager->Render();
	}
}