#include "Renderer.h"

DxSystem::BS_State Renderer::Set_BlendState = DxSystem::BS_State::BS_NONE;
DxSystem::RS_State Renderer::Set_RasterizerState = DxSystem::RS_State::RS_CULL_NONE;
DxSystem::DS_State Renderer::Set_DepthStencilState = DxSystem::DS_State::DS_TRUE;
const Matrix Renderer::CorrectionMatrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1 };

Renderer::Renderer()
{
	enabled_old = enabled;
}

void Renderer::SetEnabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		SetActive(value);
	}
}

bool Renderer::enableSelf()
{
	return enabled;
}