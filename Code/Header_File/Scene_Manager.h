#pragma once
#include <string>
#include <memory>

namespace BeastEngine
{
	struct Project_Settings;
	class Scene;
	class Editor;
	class Engine;
	class BulletPhysics_Manager;

	//�V�[���Ǘ��N���X
	class Scene_Manager
	{
	public:
		Scene_Manager();

		void Load_Scene(const std::string& scene_path);	//�t�@�C���p�X����V�[���t�@�C�������[�h���A�؂�ւ���
		std::unique_ptr<Scene>& Get_Active_Scene();     //���݃A�N�e�B�u�ȃV�[�����擾����

		bool run = false;   //�Đ�����
		bool pause = false; //�|�[�Y����

	private:
		std::unique_ptr<Scene> CreateScene_From_File();                      //�V�[�����t�@�C���_�C�A���O����ǂݍ���
		std::unique_ptr<Scene> CreateScene_From_File(std::string file_path); //�t�@�C���p�X����V�[�����r�ݍ���

		void Exit() const; //�V�[�����J������
		void Update();     //�X�V
		void Create_Scene_Default(const std::string& file_path, const std::string& file_name); //�V�[����V�K�쐬����
		void Save_Scene(const std::string& save_path); //�V�[����ۑ�����
		void Save_Settings();     //�V�[���ݒ��ۑ�����
		void Start_Debug_Scene(); //�f�o�b�O�p�̃V�[�����Đ��J�n����
		void End_Debug_Scene();   //�f�o�b�O�p�̃V�[���̍Đ����I������

		std::unique_ptr<Project_Settings> settings; //�V�[���ݒ�
		std::unique_ptr<Scene> active_scene;        //���݃A�N�e�B�u�ȃV�[��
		std::unique_ptr<Scene> behind_scene;        //�f�o�b�O���ɑҔ������Ă���V�[��
		std::string behind_path;                    //�f�o�b�O�V�[���Đ����ɓǂݍ��ނ��߂̃p�X
		std::string last_save_path;                 //�㏑���Z�[�u�p�t�@�C���p�X

		bool load = false; //���̃V�[�������[�h���邩
		std::string next_scene_path; //���ɓǂݍ��ރV�[���̃t�@�C���p�X

		friend class Editor;
		friend class Engine;
		friend class BulletPhysics_Manager;
	};
}