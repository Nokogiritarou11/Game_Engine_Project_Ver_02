#pragma once
#include <string>
#include "Original_Math.h"

namespace BeastEngine
{
	class Scene;
	class GameObject;
	class Transform;
	class Camera;
	class Animator_Controller;

	//ImGui�`��Ǘ��N���X
	class Editor
	{
	public:
		Editor();
		~Editor();

		void Update();
		static void Render();

		void Print_Log(std::string log);
		[[nodiscard]] std::shared_ptr<GameObject> Get_Drag_Object() const;

		Vector2 game_view_size;
		Vector2 game_view_position;
		Vector2 game_view_center_position;
		bool render_cursor = true;

		//�f�o�b�O�J�����֘A
		std::shared_ptr<Transform> debug_camera_transform;
		std::shared_ptr<Camera> debug_camera;

	private:
		//ImGui�ݒ�
		char* font_name = "Default_Assets/Font/mplus-1p-medium.ttf";
		float font_size_pixels = 30.0f;
		float font_size = 0.6f;
		float ui_size = 0.6f;

		std::vector<std::string> debug_log = {};
		bool debug_log_changed = false;
		std::weak_ptr<GameObject> active_object;
		std::weak_ptr<GameObject> active_object_old;
		std::weak_ptr<GameObject> dragging_object;
		bool select_object{};

		std::weak_ptr<Animator_Controller> controller;

		Vector2 game_view_render_size;
		float game_view_aspect{};

		void Main_Window_Render();                //�h�b�L���O�p�̑S��ʃE�B���h�E��`�悷��
		void Debug_Log_Render();                  //�f�o�b�O���O�`��
		void Hierarchy_Render();                  //�q�G�����L�[�`��
		void Inspector_Render();                  //�C���X�y�N�^�`��
		void ScenePlayer_Render();                //�V�[���Đ��{�^���`��
		void SceneView_Render() const;            //�V�[���r���[�`��
		void GameView_Render();                   //�Q�[���r���[�`��
		void Animation_Controller_Render() const; //�A�j���[�V�����R���g���[���[�`��

		void MenuBar_Render(); //���j���[�o�[�`��
		void Scene_File_Menu_Render(); //�V�[���֘A���j���[�`��
		void Scene_Setting_Menu_Render() const; //�V�[���ݒ胁�j���[�`��
		void GameObject_Tree_Render(const std::shared_ptr<GameObject>& obj, int flag); //�C���X�y�N�^���ł̃Q�[���I�u�W�F�N�g�\���p�ċA�֐�
		void GameObject_DragMenu_Render(const std::shared_ptr<GameObject>& obj); //D&D�����ۂ̃��j���[�E�B���h�E�`��

		void ShortCut_Check();            //�V���[�g�J�b�g�L�[�̃`�F�b�N
		void Debug_Camera_Update() const; //�V�[���r���[�J�����̑���

		void Select_Reset(); //�I����ԉ������̌�n��
		void Activate_Select_Object(const std::shared_ptr<GameObject>& obj); //�Q�[���I�u�W�F�N�g�I�����̏�����
		static void Set_Debug_Draw(bool value, const std::shared_ptr<GameObject>& obj); //�I�����̃f�o�b�O�`��o�^�p�ċA�֐�
	};
}