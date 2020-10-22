#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
#include <Original_Math.h>
#include <array>
#include "Sprite_Renderer.h"
using namespace DirectX;

class Player;

class Stage_Manager : public MonoBehaviour
{
public:
	void Start();
	void Update();

	bool Draw_ImGui();

	float Scroll_Speed;
private:
	std::weak_ptr<GameObject> Building[2];

	std::vector<std::weak_ptr<GameObject>> Bridge_Normal;
	//weak_ptr<GameObject> Bridge_Jump;
	int bridge_normal_count;
	const Vector3 bridge_offset = { 0,0,39.8f };

	std::vector<std::weak_ptr<GameObject>> Block_obj;
	std::vector<std::weak_ptr<GameObject>> Gas_obj;
	std::vector<std::weak_ptr<GameObject>> Bonus_obj;
	std::vector<std::weak_ptr<GameObject>> Jump_obj;
	std::vector<std::weak_ptr<GameObject>> Floor_obj;

	std::weak_ptr<Player> player;
	std::weak_ptr<GameObject> camera;
	Vector3 game_camera_pos;
	Vector3 start_camera_pos;
	Quaternion game_camera_rot;
	Quaternion start_camera_rot;
	float camera_timer = 0;

	int Stage_State = 0;
	float Scroll_Speed_Set;
	bool Boost_Old = false;
	float Score = 0;

	std::vector<std::array<float, 3>> Instance_Pattern;
	std::vector<std::array<float, 3>> Block_Pattern;
	std::vector<std::array<float, 3>> Gas_Pattern;
	std::vector<std::array<float, 3>> Bonus_Pattern;

	std::weak_ptr<Sprite_Renderer> UI_Black;
	std::weak_ptr<Sprite_Renderer> UI_MainWindow;
	std::weak_ptr<Sprite_Renderer> UI_Boost;
	std::weak_ptr<Sprite_Renderer> UI_Boost_OK;
	std::weak_ptr<Sprite_Renderer> UI_Gas;
	std::weak_ptr<Sprite_Renderer> UI_Title;
	std::weak_ptr<Sprite_Renderer> UI_Start;
	std::weak_ptr<Sprite_Renderer> UI_Over;
	std::weak_ptr<Sprite_Renderer> UI_Tutorial01;
	std::weak_ptr<Sprite_Renderer> UI_Tutorial02;
	float UI_flash_Timer = 0;
	bool UI_Black_Old = true;
	bool UI_Flush_Old = true;

	std::weak_ptr<Sprite_Renderer> UI_Boost_Timer;
	std::weak_ptr<Sprite_Renderer> UI_Score_0001;
	std::weak_ptr<Sprite_Renderer> UI_Score_0010;
	std::weak_ptr<Sprite_Renderer> UI_Score_0100;
	std::weak_ptr<Sprite_Renderer> UI_Score_1000;

	void Create_Objects(Vector3 Instance_Pos);
	std::shared_ptr<GameObject> Instance_Object(std::vector<std::weak_ptr<GameObject>>& objects);

	void Number_Sprite(std::weak_ptr<Sprite_Renderer> renderer, int number);

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<MonoBehaviour>(this), Scroll_Speed);
	}
};

CEREAL_REGISTER_TYPE(Stage_Manager)
CEREAL_REGISTER_POLYMORPHIC_RELATION(MonoBehaviour, Stage_Manager)
CEREAL_CLASS_VERSION(Stage_Manager, 1)