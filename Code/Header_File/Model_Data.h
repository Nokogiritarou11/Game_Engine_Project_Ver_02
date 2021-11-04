#pragma once
#include "Mesh.h"
#include "Object.h"
#include <vector>
#include "fbxsdk.h"

namespace BeastEngine
{
	//FBX����ǂݍ��񂾃��f���f�[�^(�e�t�@�C���ɕϊ����Ďg���̂ł��̂܂܎g���킯�ł͂Ȃ�)
	class Model_Data
	{
	public:
		//�{�[��
		struct Skeleton
		{
			std::string	name;
			int			parent_index = 0;
			Vector3	scale;
			Quaternion	rotation;
			Vector3	position;
		};

		//�L�[�t���[���ł̎p��
		struct Key_Pose
		{
			Vector3	scale;
			Quaternion rotation;
			Vector3	position;
		};

		//�L�[�t���[��
		struct Keyframe
		{
			float					seconds = 0;
			std::vector<Key_Pose>	node_keys{};
		};

		//�A�j���[�V����
		struct Animation
		{
			std::string			  name;
			float				  seconds_length = 0;
			std::vector<Keyframe> keyframes{};
		};

		std::string name;
		std::string file_path;

		std::vector<Skeleton> bones;
		std::vector<std::shared_ptr<Mesh>>	meshes;
		std::vector<Animation> animations;
		std::vector<std::string> default_material_paths;

		//�w�肵���p�X�̃��b�V����ǂݍ���
		//��1�����Ƀ��b�V���܂ł̃p�X
		//��2�����Ƀt�@�C����(�g���q�͂���Ȃ�)����͂���
		static std::shared_ptr<Model_Data> Load_Model(const std::string& file_path, const std::string& fbx_filename);

	private:
		void BuildNodes(FbxNode* fbx_node, int parent_node_index);     // �m�[�h�f�[�^���\�z
		void BuildNode(const FbxNode* fbx_node, int parent_node_index);// �m�[�h�f�[�^���\�z(�ċA�֐�)

		void BuildMeshes(FbxNode* fbx_node);                  // ���b�V���f�[�^���\�z
		void BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh); // ���b�V���f�[�^���\�z(�ċA�֐�)

		void BuildAnimations(FbxScene* fbx_scene); // �A�j���[�V�����f�[�^���\�z

		int FindNodeIndex(const char* name); // �C���f�b�N�X�̌���

		int root_motion_node_index = -1;
	};
}