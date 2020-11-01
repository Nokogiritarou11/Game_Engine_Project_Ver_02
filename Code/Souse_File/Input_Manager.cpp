#include "Input_Manager.h"

Keyboard::State Input_Manager::kb;
Mouse::State Input_Manager::ms;
GamePad::State Input_Manager::pad;
Keyboard::KeyboardStateTracker Input_Manager::key_tracker;
Mouse::ButtonStateTracker Input_Manager::mouse_tracker;
GamePad::ButtonStateTracker Input_Manager::pad_tracker;
unique_ptr<Mouse> Input_Manager::mouse;
unique_ptr<GamePad> Input_Manager::gamepad;
unique_ptr<Keyboard> Input_Manager::keyboard;

Input_Manager::Input_Manager()
{
	mouse    = make_unique<Mouse>();
	//gamepad  = make_unique<GamePad>();
	keyboard = make_unique<Keyboard>();
}

void Input_Manager::Update()
{
	kb = keyboard->GetState();
	key_tracker.Update(kb);
	ms = mouse->GetState();

	mouse_tracker.Update(ms);
	/*
	pad = gamepad->GetState(0);

	if (pad.IsConnected())
	{
		pad_tracker.Update(pad);
	}
	else
	{
		pad_tracker.Reset();
	}*/

}