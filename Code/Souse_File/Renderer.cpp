#include "Renderer.h"

int Renderer::Set_BlendState = DxSystem::BS_NONE;
int Renderer::Set_RasterizerState = DxSystem::RS_CULL_NONE;
int Renderer::Set_DepthStencilState = DxSystem::DS_TRUE;
const Matrix Renderer::CorrectionMatrix = { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

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