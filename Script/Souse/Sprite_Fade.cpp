#include "Sprite_Fade.h"

using namespace std;

void Sprite_Fade::Start()
{
	Cursor::lockState = CursorLockMode::None;
	Cursor::visible = false;
	UI_Fade = GetComponent<Sprite_Renderer>();
}

void Sprite_Fade::Update()
{
	timer += Time::deltaTime;
	if (FadeOut)
	{
		float w = 1.0f - timer / Fade_Time;
		UI_Fade.lock()->Color.w = w;
		if (w <= 0.0f)
		{
			UI_Fade.lock()->Color.w = 0;
			timer = 0;
			gameObject->SetActive(false);
		}
	}
	else
	{
		float w = timer / Fade_Time;
		UI_Fade.lock()->Color.w = w;
		if (w >= 1.0f)
		{
			UI_Fade.lock()->Color.w = 1;
			timer = 0;
			SetEnabled(false);
		}
	}
}

bool Sprite_Fade::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Sprite_Fade");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Sprite_Fade_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Sprite_Fade>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	if (open)
	{
		ImGui::DragFloat("Fade_Time", &Fade_Time);
		ImGui::Checkbox("FadeOut", &FadeOut);
	}

	return true;
}