#include "UI_Manager.h"
#include "Player.h"
#include "Dragon_HP.h"
using namespace std;

void UI_Manager::Start()
{
	UI_Cursor = GameObject::Find("Cursor").lock()->GetComponent<Sprite_Renderer>();
	shared_ptr<Sprite_Renderer> ui_cursor = UI_Cursor.lock();
	ui_cursor->transform->Set_position(static_cast<float>(Screen::Get_Width()) / 2 - ui_cursor->Size.x / 2, static_cast<float>(Screen::Get_Height()) / 2 - ui_cursor->Size.y / 2, 0);

	UI_HP_Player = GameObject::Find("Player_HP").lock()->GetComponent<Sprite_Renderer>();
	player_UI_max = UI_HP_Player.lock()->Size.x;
	UI_HP_Dragon = GameObject::Find("Dragon_HP").lock()->GetComponent<Sprite_Renderer>();
	dragon_UI_max = UI_HP_Dragon.lock()->Size.x;

	player = GameObject::Find("Player").lock()->GetComponent<Player>();
	player_hp_max = player.lock()->HP;
	dragon = GameObject::Find("Dragon").lock()->GetComponent<Dragon_HP>();
	dragon_hp_max = dragon.lock()->HP;

	UI_GameClear = GameObject::Find("Clear").lock()->GetComponent<Sprite_Renderer>();
	UI_GameOver = GameObject::Find("Over").lock()->GetComponent<Sprite_Renderer>();

	UI_Damage = GameObject::Find("Damage").lock()->GetComponent<Sprite_Renderer>();

	scene_timer = 0;
}

void UI_Manager::Update()
{
	shared_ptr<Player> p = player.lock();
	shared_ptr<Dragon_HP> d = dragon.lock();

	UI_HP_Player.lock()->Size.x = player_UI_max * (static_cast<float>(p->HP) / static_cast<float>(player_hp_max));
	UI_HP_Player.lock()->UV_Size.x = player_UI_max * (static_cast<float>(p->HP) / static_cast<float>(player_hp_max));
	UI_HP_Dragon.lock()->Size.x = dragon_UI_max * (static_cast<float>(d->HP) / static_cast<float>(dragon_hp_max));

	if (d->HP == 0)
	{
		if (scene_timer == 0)
		{
			UI_GameClear.lock()->SetEnabled(true);
		}
		scene_timer += Time::deltaTime;
		if (scene_timer >= 7)
		{
			Scene_Manager::LoadScene("Resouces\\Scene\\Title.bin");
		}
	}

	if (p->HP <= 0)
	{
		if (scene_timer == 0)
		{
			UI_Damage.lock()->Color.w = 1;
			UI_GameOver.lock()->SetEnabled(true);
		}
		scene_timer += Time::deltaTime;
		if (scene_timer >= 5)
		{
			Scene_Manager::LoadScene("Resouces\\Scene\\Title.bin");
		}
	}
	else
	{
		if (p->Damage_Effect)
		{
			UI_Damage.lock()->Color.w = 1;
		}
		else
		{
			UI_Damage.lock()->Color.w = 0;
		}
	}
}

bool UI_Manager::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("UI_Manager");

	bool removed = true;
	if (ImGui::BeginPopupContextItem("UI_Manager_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<UI_Manager>(shared_from_this()));
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