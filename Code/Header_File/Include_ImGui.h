#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"
#include "imgui_internal.h"
#include "IconsFontAwesome4.h"
#include "Original_Math.h"
#include <string>

namespace ImGui
{
	bool LeftText_DragInt(const char* text, const char* label, int* value, const float x_offset, const float width = -FLT_MIN);
	bool LeftText_InputInt(const char* text, const char* label, int* value, const float x_offset, const float width = -FLT_MIN);

	bool LeftText_DragFloat(const char* text, const char* label, float* value, const float x_offset, const float width = -FLT_MIN, const float drag_speed = 0.01f, const char* format = "%.3f");
	bool LeftText_DragFloat2(const char* text, const char* label, BeastEngine::Vector2& value, const float x_offset, const float width = -FLT_MIN, const float drag_speed = 0.01f, const char* format = "%.3f");
	bool LeftText_DragFloat3(const char* text, const char* label, BeastEngine::Vector3& value, const float x_offset, const float width = -FLT_MIN, const float drag_speed = 0.01f, const char* format = "%.3f");
	bool LeftText_DragFloat4(const char* text, const char* label, BeastEngine::Vector4& value, const float x_offset, const float width = -FLT_MIN, const float drag_speed = 0.01f, const char* format = "%.3f");
	bool LeftText_ColorEdit3(const char* text, const char* label, BeastEngine::Vector3& value, const float x_offset, const float width = -FLT_MIN);
	bool LeftText_ColorEdit4(const char* text, const char* label, BeastEngine::Vector4& value, const float x_offset, const float width = -FLT_MIN);

	bool LeftText_Combo(const char* text, const char* label, int* current_item, const char* const items[], int items_count, const float x_offset, const float width = -FLT_MIN);

	bool LeftText_Checkbox(const char* text, const char* label, bool* value, const float x_offset);

	bool LeftText_InputText(const char* text, const char* label, std::string* value, const float x_offset, const float width = -FLT_MIN);
}