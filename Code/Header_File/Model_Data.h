#pragma once
#include "Mesh.h"
#include "Object.h"
#include <vector>
#include "fbxsdk.h"

namespace BeastEngine
{
	//FBXから読み込んだモデルデータ(各ファイルに変換して使うのでこのまま使うわけではない)
	class Model_Data
	{
	public:
		//ボーン
		struct Skeleton
		{
			std::string	name;
			int			parent_index = 0;
			Vector3	scale;
			Quaternion	rotation;
			Vector3	position;
		};

		//キーフレームでの姿勢
		struct Key_Pose
		{
			Vector3	scale;
			Quaternion rotation;
			Vector3	position;
		};

		//キーフレーム
		struct Keyframe
		{
			float					seconds = 0;
			std::vector<Key_Pose>	node_keys{};
		};

		//アニメーション
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

		//指定したパスのメッシュを読み込む
		//第1引数にメッシュまでのパス
		//第2引数にファイル名(拡張子はいらない)を入力する
		static std::shared_ptr<Model_Data> Load_Model(const std::string& file_path, const std::string& fbx_filename);

	private:
		void BuildNodes(FbxNode* fbx_node, int parent_node_index);     // ノードデータを構築
		void BuildNode(const FbxNode* fbx_node, int parent_node_index);// ノードデータを構築(再帰関数)

		void BuildMeshes(FbxNode* fbx_node);                  // メッシュデータを構築
		void BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh); // メッシュデータを構築(再帰関数)

		void BuildAnimations(FbxScene* fbx_scene); // アニメーションデータを構築

		int FindNodeIndex(const char* name); // インデックスの検索

		int root_motion_node_index = -1;
	};
}