#include "Title.h"
using namespace std;

void Title::Start()
{
	Cursor::lockState = CursorLockMode::None;
	Cursor::visible = false;
	UI_Title = GameObject::Find("Title");
	UI_Start = GameObject::Find("Start");
	UI_Tutorial = GameObject::Find("Tutorial");
	UI_Fade = GameObject::Find("Fade");

	UI_Title.lock()->SetActive(true);
	UI_Tutorial.lock()->SetActive(false);

	state = 0;
}

void Title::Update()
{
	static float timer = 0;
	static float fade = 0;

	if (state == 0)
	{
		if (Input::GetKeyDown(KeyCode::Space))
		{
			UI_Title.lock()->SetActive(false);
			UI_Tutorial.lock()->SetActive(true);
			++state;
		}
	}
	else if (state == 1)
	{
		if (Input::GetKeyDown(KeyCode::Space))
		{
			UI_Fade.lock()->SetActive(true);
			++state;
		}
	}
	else if (state == 2)
	{
		fade += Time::deltaTime;
		if (fade >= 2)
		{
			fade = 0;
			timer = 0;
			state = 0;
			Scene_Manager::LoadScene("Resouces\\Scene\\Main_Scene.bin");
		}
	}

	timer += Time::deltaTime * 2;
	if (static_cast<int>(timer) % 2)
	{
		UI_Start.lock()->SetActive(true);
	}
	else
	{
		UI_Start.lock()->SetActive(false);
	}
}

bool Title::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Title");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Title_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Title>(shared_from_this()));
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
	}

	return true;
}