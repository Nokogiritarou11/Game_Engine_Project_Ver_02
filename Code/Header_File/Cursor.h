#pragma once
#include "Original_Math.h"

namespace BeastEngine
{
	//�J�[�\���̏��
	enum class CursorLock_Mode
	{
		None,
		Locked
	};

	//�J�[�\���Ǘ��N���X
	class Cursor
	{
	public:
		static bool visible; //�J�[�\����`�悷�邩
		static CursorLock_Mode cursor_lock_mode; //�J�[�\���̏��

	private:
		friend class Engine;
		friend class Input;

		bool is_visible = true;

		static bool window_focus; //�E�B���h�E���t�H�[�J�X����Ă��邩
		static Vector2 lock_position; //�J�[�\���̌Œ�ʒu

		void Update(); //�X�V
		void Set_Cursor_Visible(bool value); //�J�[�\���̉���Ԃ�ύX����
	};
}