#pragma once
#include "Bounds.h"
#include "Mesh.h"
#include "Object.h"
#include <vector>
#include <wrl.h>
#include <tchar.h>
#include "fbxsdk.h"

namespace BeastEngine
{
	class Model_Data
	{
	public:
		struct Skeleton
		{
			std::string	name;
			int			parent_index = 0;
			Vector3	scale;
			Quaternion	rotation;
			Vector3	position;
		};

		struct Key_Pose
		{
			Vector3	scale;
			Quaternion rotation;
			Vector3	position;
		};

		struct Keyframe
		{
			float					seconds = 0;
			std::vector<Key_Pose>	node_keys{};
		};
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
		// �m�[�h�f�[�^���\�z
		void BuildNodes(FbxNode* fbx_node, int parent_node_index);
		void BuildNode(const FbxNode* fbx_node, int parent_node_index);

		// ���b�V���f�[�^���\�z
		void BuildMeshes(FbxNode* fbx_node);
		void BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh);

		// �A�j���[�V�����f�[�^���\�z
		void BuildAnimations(FbxScene* fbx_scene);

		// �C���f�b�N�X�̌���
		int FindNodeIndex(const char* name);

		int rootMotionNodeIndex = -1;
	};
}