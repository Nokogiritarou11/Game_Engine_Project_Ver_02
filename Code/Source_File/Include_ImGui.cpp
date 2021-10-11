#include "Include_ImGui.h"

using namespace std;

namespace ImGui
{
	bool LeftText_DragInt(const char* text, const char* label, int* value, const float x_offset, const float width)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);
		return ImGui::DragInt(label, value);
	}

	bool LeftText_InputInt(const char* text, const char* label, int* value, const float x_offset, const float width)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);
		return ImGui::InputInt(label, value);
	}

	bool LeftText_DragFloat(const char* text, const char* label, float* value, const float x_offset, const float width, const float drag_speed, const char* format)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);
		return ImGui::DragFloat(label, value, drag_speed, 0, 0, format);
	}

	bool LeftText_DragFloat2(const char* text, const char* label, BeastEngine::Vector2& value, const float x_offset, const float width, const float drag_speed, const char* format)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);

		float v[2] = { value.x, value.y };
		if (ImGui::DragFloat2(label, v, drag_speed, 0, 0, format))
		{
			value = BeastEngine::Vector2(v[0], v[1]);
			return true;
		}

		return false;
	}

	bool LeftText_DragFloat3(const char* text, const char* label, BeastEngine::Vector3& value, const float x_offset, const float width, const float drag_speed, const char* format)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);

		float v[3] = { value.x, value.y, value.z };
		if (ImGui::DragFloat3(label, v, drag_speed, 0, 0, format))
		{
			value = BeastEngine::Vector3(v[0], v[1], v[2]);
			return true;
		}

		return false;
	}

	bool LeftText_DragFloat4(const char* text, const char* label, BeastEngine::Vector4& value, const float x_offset, const float width, const float drag_speed, const char* format)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);

		float v[4] = { value.x, value.y, value.z, value.w };
		if (ImGui::DragFloat4(label, v, drag_speed, 0, 0, format))
		{
			value = BeastEngine::Vector4(v[0], v[1], v[2], v[3]);
			return true;
		}

		return false;
	}

	bool LeftText_ColorEdit3(const char* text, const char* label, BeastEngine::Vector3& value, const float x_offset, const float width)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);

		float v[3] = { value.x, value.y, value.z };
		if (ImGui::ColorEdit3(label, v))
		{
			value = BeastEngine::Vector3(v[0], v[1], v[2]);
			return true;
		}

		return false;
	}

	bool LeftText_ColorEdit4(const char* text, const char* label, BeastEngine::Vector4& value, const float x_offset, const float width)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);

		float v[4] = { value.x, value.y, value.z, value.w };
		if (ImGui::ColorEdit4(label, v))
		{
			value = BeastEngine::Vector4(v[0], v[1], v[2], v[3]);
			return true;
		}

		return false;
	}

	bool LeftText_Combo(const char* text, const char* label, int* current_item, const char* const items[], int items_count, const float x_offset, const float width)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);

		return ImGui::Combo(label, current_item, items, items_count);
	}

	bool LeftText_Checkbox(const char* text, const char* label, bool* value, const float x_offset)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		return ImGui::Checkbox(label, value);
	}

	bool LeftText_InputText(const char* text, const char* label, string* value, const float x_offset, const float width)
	{
		ImGui::Text(text);
		ImGui::SameLine(x_offset);
		ImGui::SetNextItemWidth(width);

		return ImGui::InputText(label, value);
	}
}