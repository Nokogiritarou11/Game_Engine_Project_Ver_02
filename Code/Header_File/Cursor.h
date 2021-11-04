#pragma once
#include "Original_Math.h"

namespace BeastEngine
{
	//カーソルの状態
	enum class CursorLock_Mode
	{
		None,
		Locked
	};

	//カーソル管理クラス
	class Cursor
	{
	public:
		static bool visible; //カーソルを描画するか
		static CursorLock_Mode cursor_lock_mode; //カーソルの状態

	private:
		friend class Engine;
		friend class Input;

		bool is_visible = true;

		static bool window_focus; //ウィンドウがフォーカスされているか
		static Vector2 lock_position; //カーソルの固定位置

		void Update(); //更新
		void Set_Cursor_Visible(bool value); //カーソルの可視状態を変更する
	};
}