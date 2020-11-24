#pragma once

class Cursor
{
public:
	enum class CursorLockMode
	{
		None,
		Locked
	};

	static bool visible;
	static CursorLockMode lockState;

private:
	friend class Engine;

	void Update();
	bool Is_Visible = true;
	void Set_Cursor_Visible(bool value);
};