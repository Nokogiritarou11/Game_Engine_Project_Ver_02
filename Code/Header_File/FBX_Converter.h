#pragma once
#include <memory>
#include <string>
#include <vector>

namespace BeastEngine
{
	class Model_Data;
	class GameObject;
	class Transform;
	struct Matrix;

	//FBX�t�@�C���ϊ�
	class Fbx_Converter
	{
	public:
		void Draw_ImGui();

	private:
		void Load_From_Fbx(bool& convert_mesh,bool& convert_material, bool& convert_animation, bool& convert_prefab) const; //�ϊ������s����
		void Convert_Animation(const std::vector<std::shared_ptr<GameObject>>& bones) const; //�A�j���[�V�����N���b�v�ɕϊ�����
		void Convert_Mesh() const; //���b�V���t�@�C���ɕϊ�����
		static void Bone_Decompose(std::vector<Matrix>& matrixes, std::vector<std::shared_ptr<GameObject>>& bones, const std::shared_ptr<Transform>& trans); //�{�[���s��擾�ċA�֐�
		[[nodiscard]] std::shared_ptr<GameObject> Create_GameObject(const std::string& n) const; //�v���n�u�����邽�߂Ɉ�x�Q�[���I�u�W�F�N�g�Ƃ��Đ�������

		std::shared_ptr<Model_Data> model;
		int load_state = 0;
		bool convert_mesh = true; //���b�V���������o����
		bool convert_material = true; //�}�e���A���������o����
		bool convert_animation = true; //�A�j���[�V�����N���b�v�������o����
		bool convert_prefab = true; //�v���n�u�������o����
	};
}
