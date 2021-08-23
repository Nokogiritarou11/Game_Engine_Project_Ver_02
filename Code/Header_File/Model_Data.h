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

		//指定したパスのメッシュを読み込む
		//第1引数にメッシュまでのパス
		//第2引数にファイル名(拡張子はいらない)を入力する
		static std::shared_ptr<BeastEngine::Model_Data> Load_Model(const std::string& file_path, const std::string& fbx_filename);

	private:
		// ノードデータを構築
		void BuildNodes(FbxNode* fbxNode, int parentNodeIndex);
		void BuildNode(FbxNode* fbxNode, int parentNodeIndex);

		// メッシュデータを構築
		void BuildMeshes(FbxNode* fbxNode);
		void BuildMesh(FbxNode* fbxNode, FbxMesh* fbxMesh);

		// アニメーションデータを構築
		void BuildAnimations(FbxScene* fbxScene);

		// インデックスの検索
		int FindNodeIndex(const char* name);

		int rootMotionNodeIndex = -1;

		void GetTexture(const FbxSurfaceMaterial* fbx_mat, const char* fbx_tex_type);
	};
}