#include "View_Game.h"
#include "DxSystem.h"
#include "Light_Manager.h"
#include "GameObject.h"
using namespace std;
using namespace DirectX;

void View_Game::Render(Matrix V, Matrix P, std::shared_ptr<Transform> trans)
{
	bool Cleared = false;
	CbScene cbScene;
	for (list<weak_ptr<Light>>::iterator itr = Light_Manager::Light_list.begin(); itr != Light_Manager::Light_list.end();)
	{
		if (itr->expired())
		{
			itr = Light_Manager::Light_list.erase(itr);
			continue;
		}
		shared_ptr<Light> m_light = itr->lock();
		if (m_light->gameObject->activeSelf())
		{
			if (m_light->enabled)
			{

				//シャドウマップ描画
				{
					m_light->Set(trans);
					cbScene.viewProjection = m_light->V * m_light->P;
					static const Matrix SHADOW_BIAS = {
						0.5f, 0.0f, 0.0f, 0.0f,
						0.0f, -0.5f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.5f, 0.5f, 0.0f, 1.0f };
					cbScene.shadowMatrix   = (m_light->V * m_light->P) * SHADOW_BIAS;
					cbScene.lightDirection = m_light->Direction;
					cbScene.lightColor	   = { m_light->Color.x,m_light->Color.y,m_light->Color.z };
					cbScene.Bias		   = m_light->Bias;

					DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
					DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbScene.Get(), 0, 0, &cbScene, 0, 0);

					Render_3D(m_light->V, m_light->P, false, m_light->shader);

				}
				//通常描画
				{
#if _DEBUG
					DxSystem::DeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthStencilView.Get());
					if (!Cleared)
					{
						Clear();
						Cleared = true;
					}
#else
					// レンダーターゲットビュー設定
					DxSystem::SetDefaultView();
#endif
					// ビューポートの設定
					DxSystem::SetViewPort(screen_x, screen_y);
					// シーン用定数バッファ更新
					cbScene.viewProjection = V * P;
					DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
					DxSystem::DeviceContext->PSSetConstantBuffers(0, 1, ConstantBuffer_CbScene.GetAddressOf());
					DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbScene.Get(), 0, 0, &cbScene, 0, 0);

					Render_Sky(trans);
					DxSystem::DeviceContext->PSSetSamplers(0, 1, m_light->sampler.GetAddressOf());
					DxSystem::DeviceContext->PSSetShaderResources(0, 1, m_light->ShaderResourceView.GetAddressOf());
					Render_3D(V, P);
					Render_2D(V, P);
					}
				}
			}
		itr++;
		}
}