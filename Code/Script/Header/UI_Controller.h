#pragma once
#include "MonoBehaviour.h"
#include "Transform.h"
using namespace DirectX;
using namespace std;

class Game_Manager;
class Sprite_Renderer;
class Player;

class UI_Controller : public MonoBehaviour
{
public:

	void Start();
	void Update();

private:

	weak_ptr<Game_Manager> game_manager;

	weak_ptr<GameObject> Cursor_Set;
	weak_ptr<GameObject> Cursor_Move;
	weak_ptr<GameObject> Clear;
	weak_ptr<GameObject> Failed;
	weak_ptr<Player> player;
	weak_ptr<Sprite_Renderer> player_bar;
};