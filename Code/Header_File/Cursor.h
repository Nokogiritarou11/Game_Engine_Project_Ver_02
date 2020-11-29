#pragma once
#include "Original_Math.h"

enum class CursorLockMode
{
	None,
	Locked
};

class Cursor
{
public:

	static bool visible;
	static CursorLockMode lockState;

private:
	friend class Engine;
	friend class Input;

	bool Is_Visible = true;

	static bool Window_Focus;
	static Vector2 Lock_Pos;

	void Update();
	void Set_Cursor_Visible(bool value);
};