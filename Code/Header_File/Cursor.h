#pragma once
#include "Original_Math.h"

namespace BeastEngine
{
	enum class CursorLock_Mode
	{
		None,
		Locked
	};

	class Cursor
	{
	public:

		static bool visible;
		static CursorLock_Mode cursor_lock_mode;

	private:
		friend class Engine;
		friend class Input;

		bool is_visible = true;

		static bool window_focus;
		static Vector2 lock_position;

		void Update();
		void Set_Cursor_Visible(bool value);
	};
}