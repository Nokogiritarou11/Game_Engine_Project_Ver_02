#include "Renderer.h"
using namespace BeastEngine;

BS_State Renderer::binding_blend_state = BS_State::Off;
RS_State Renderer::binding_rasterizer_state = RS_State::Cull_None;
DS_State Renderer::binding_depth_stencil_State = DS_State::LEqual;

void Renderer::Set_Enabled(bool value)
{
	if (value != enabled_old)
	{
		enabled = value;
		enabled_old = value;
		Set_Active(value);
	}
}

bool Renderer::Get_Enabled()
{
	return enabled;
}