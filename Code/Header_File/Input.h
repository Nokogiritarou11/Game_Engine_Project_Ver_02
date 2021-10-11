#pragma once
#include "Original_Math.h"
#include <Windows.h>
#include <Xinput.h>
#pragma comment(lib,"xinput.lib ")


namespace BeastEngine
{
	enum class Key_Code
	{
		None = 0x00,
		Mouse0 = 0x01,
		Mouse1 = 0x02,
		Mouse2 = 0x04,
		Mouse3 = 0x05,
		Mouse4 = 0x06,
		Backspace = 0x08,
		Tab = 0x09,
		Clear = 0x0C,
		Return = 0x0D,
		Pause = 0x13,
		Escape = 0x1B,
		Space = 0x20,
		Delete = 0x2E,
		Keypad0 = 0x60,
		Keypad1 = 0x61,
		Keypad2 = 0x62,
		Keypad3 = 0x63,
		Keypad4 = 0x64,
		Keypad5 = 0x65,
		Keypad6 = 0x66,
		Keypad7 = 0x67,
		Keypad8 = 0x68,
		Keypad9 = 0x69,
		KeypadPeriod = 0x6E,
		KeypadDivide = 0x6F,
		KeypadMultiply = 0x6A,
		KeypadMinus = 0x6D,
		KeypadPlus = 0x6B,
		LeftArrow = 0x25,
		UpArrow = 0x26,
		RightArrow = 0x27,
		DownArrow = 0x28,
		Insert = 0x2D,
		PageUp = 0x21,
		PageDown = 0x22,
		End = 0x23,
		Home = 0x24,
		F1 = 0x70,
		F2 = 0x71,
		F3 = 0x72,
		F4 = 0x73,
		F5 = 0x74,
		F6 = 0x75,
		F7 = 0x76,
		F8 = 0x77,
		F9 = 0x78,
		F10 = 0x79,
		F11 = 0x7A,
		F12 = 0x7B,
		F13 = 0x7C,
		F14 = 0x7D,
		F15 = 0x7E,
		F16 = 0x7F,
		F17 = 0x80,
		F18 = 0x81,
		F19 = 0x82,
		F20 = 0x83,
		F21 = 0x84,
		F22 = 0x85,
		F23 = 0x86,
		F24 = 0x87,
		Alpha0 = 0x30,
		Alpha1 = 0x31,
		Alpha2 = 0x32,
		Alpha3 = 0x33,
		Alpha4 = 0x34,
		Alpha5 = 0x35,
		Alpha6 = 0x36,
		Alpha7 = 0x37,
		Alpha8 = 0x38,
		Alpha9 = 0x39,
		Comma = 0xBC,
		Minus = 0xBD,
		Period = 0xBE,
		Slash = 0xBF,
		Colon = 0xBA,
		Semicolon = 0xBB,
		At = 0xC0,
		LeftBracket = 0xDB,
		Backslash = 0xDC,
		RightBracket = 0xDD,
		Caret = 0xDE,
		BackQuote = 0xE2,
		A = 0x41,
		B = 0x42,
		C = 0x43,
		D = 0x44,
		E = 0x45,
		F = 0x46,
		G = 0x47,
		H = 0x48,
		I = 0x49,
		J = 0x4A,
		K = 0x4B,
		L = 0x4C,
		M = 0x4D,
		N = 0x4E,
		O = 0x4F,
		P = 0x50,
		Q = 0x51,
		R = 0x52,
		S = 0x53,
		T = 0x54,
		U = 0x55,
		V = 0x56,
		W = 0x57,
		X = 0x58,
		Y = 0x59,
		Z = 0x5A,
		Numlock = 0x90,
		CapsLock = 0xF0,
		ScrollLock = 0x91,
		LeftShift = 0xA0,
		RightShift = 0xA1,
		LeftControl = 0xA2,
		RightControl = 0xA3,
		LeftAlt = 0xA4,
		RightAlt = 0xA5,
	};

	enum class Button_Code
	{
		DPad_Up,
		DPad_Down,
		DPad_Left,
		DPad_Right,
		A,
		B,
		X,
		Y,
		Start,
		Back,
		Left_Thumb,
		Right_Thumb,
		Left_Shoulder,
		Right_Shoulder,
	};

	class Input
	{
	public:
		static void Update();

		static bool Get_Key(Key_Code key);
		static bool Get_Key_Down(Key_Code key);
		static bool Get_Key_Up(Key_Code key);

		static bool Get_Mouse_Button(int button);
		static bool Get_Mouse_Button_Down(int button);
		static bool Get_Mouse_Button_Up(int button);

		static Vector2 Get_Mouse_Position();
		static Vector2 Get_Mouse_Relative_Position();

		static bool Get_Pad_Button(Button_Code button, int pad_id = 0);
		static bool Get_Pad_Button_Down(Button_Code button, int pad_id = 0);
		static bool Get_Pad_Button_Up(Button_Code button, int pad_id = 0);

		static float Get_Pad_Trigger_Left(int pad_id = 0);
		static float Get_Pad_Trigger_Right(int pad_id = 0);

		static Vector2 Get_Pad_Axis_Left(int pad_id = 0);
		static Vector2 Get_Pad_Axis_Right(int pad_id = 0);

	private:
		struct Controller_Input_Result
		{
			bool buttons[14];
			float left_trigger;
			float right_trigger;
			Vector2 left_axis;
			Vector2 right_axis;
		};

		struct Controller_State
		{
			XINPUT_STATE state{};
			bool is_connected{};
			Controller_Input_Result result;
			Controller_Input_Result result_old;

			void Update();
		};

		static Controller_State controller_state[4];

		static BYTE key_state[256];
		static BYTE key_state_old[256];

		static Vector2 mouse_position;
		static Vector2 mouse_position_old;
	};
}