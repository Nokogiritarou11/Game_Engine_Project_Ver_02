#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
using namespace DirectX;
#include <stdio.h>
using namespace std;
#include <Keyboard.h>
#include <GamePad.h>
#include <Mouse.h>

class Input_Manager
{
public:
	static Keyboard::State kb;
	static Mouse::State ms;
	static GamePad::State pad;
	static Keyboard::KeyboardStateTracker key_tracker;
	static Mouse::ButtonStateTracker mouse_tracker;
	static GamePad::ButtonStateTracker pad_tracker;
	static unique_ptr<Mouse> mouse;
	static unique_ptr<GamePad> gamepad;
	static unique_ptr<Keyboard> keyboard;

	//Input_Manager();
	//~Input_Manager();

	static void Initialize();

	static void Update();
private:
};
