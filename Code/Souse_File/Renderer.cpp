#include "Renderer.h"

BS_State Renderer::Set_BlendState = BS_State::Off;
RS_State Renderer::Set_RasterizerState = RS_State::Cull_None;
DS_State Renderer::Set_DepthStencilState = DS_State::LEqual;
const Matrix Renderer::CorrectionMatrix = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1 };

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