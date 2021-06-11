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
	//シャドウマップ描画
	for (vector<weak_ptr<Light>>::iterator itr = Engine::light_manager->Light_list.begin(); itr != Engine::light_manager->Light_list.end();)
	{
		if (itr->expired())
		{
			itr = Engine::light_manager->Light_list.erase(itr);
			continue;
		}
		shared_ptr<Light> m_light = itr->lock();
		if (m_light->gameObject->activeInHierarchy())
		{
			if (m_light->enableSelf())
			{
				Render_Shadow_Directional(m_light->Color, m_light->Intensity, m_light->transform, camera_transform);
			}
		}
		++itr;
	}

	//通常描画
	{
		DxSystem::DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
		Clear();

		// ビューポートの設定
		DxSystem::SetViewPort(screen_x, screen_y);
		// シーン用定数バッファ更新
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