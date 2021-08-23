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
			std::string			name;
			int					parentIndex;
			BeastEngine::Vector3	scale;
			BeastEngine::Quaternion	rotation;
			BeastEngine::Vector3	position;
		};

		struct Key_Pose
		{
			BeastEngine::Vector3	scale;
			BeastEngine::Quaternion rotation;
			BeastEngine::Vector3	position;
		};

		struct Keyframe
		{
			float						seconds;
			std::vector<Key_Pose>	nodeKeys;
		};
		struct Animation
		{
			std::string					name;
			float						secondsLength;
			std::vector<Keyframe>		keyframes;
		};

		std::string name;
		std::string file_path;

		std::vector<Skeleton> bones;
		std::vector<std::shared_ptr<BeastEngine::Mesh>>	meshes;
		std::vector<Animation> animations;
		std::vector<std::string> default_material_pathes;

		//�w�肵���p�X�̃��b�V����ǂݍ���
		//��1�����Ƀ��b�V���܂ł̃p�X
		//��2�����Ƀt�@�C����(�g���q�͂���Ȃ�)����͂���
		static std::shared_ptr<BeastEngine::Model_Data> Load_Model(const std::string& file_path, const std::string& fbx_filename);

	private:
		// �m�[�h�f�[�^���\�z
		void BuildNodes(FbxNode* fbxNode, int parentNodeIndex);
		void BuildNode(FbxNode* fbxNode, int parentNodeIndex);

		// ���b�V���f�[�^���\�z
		void BuildMeshes(FbxNode* fbxNode);
		void BuildMesh(FbxNode* fbxNode, FbxMesh* fbxMesh);

		// �A�j���[�V�����f�[�^���\�z
		void BuildAnimations(FbxScene* fbxScene);

		// �C���f�b�N�X�̌���
		int FindNodeIndex(const char* name);

		int rootMotionNodeIndex = -1;

		void GetTexture(const FbxSurfaceMaterial* fbx_mat, const char* fbx_tex_type);
	};
}