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

	//FBXファイル変換
	class Fbx_Converter
	{
	public:
		void Draw_ImGui();

	private:
		void Load_From_Fbx(bool& convert_mesh,bool& convert_material, bool& convert_animation, bool& convert_prefab) const; //変換を実行する
		void Convert_Animation(const std::vector<std::shared_ptr<GameObject>>& bones) const; //アニメーションクリップに変換する
		void Convert_Mesh() const; //メッシュファイルに変換する
		static void Bone_Decompose(std::vector<Matrix>& matrixes, std::vector<std::shared_ptr<GameObject>>& bones, const std::shared_ptr<Transform>& trans); //ボーン行列取得再帰関数
		[[nodiscard]] std::shared_ptr<GameObject> Create_GameObject(const std::string& n) const; //プレハブ化するために一度ゲームオブジェクトとして生成する

		std::shared_ptr<Model_Data> model;
		int load_state = 0;
		bool convert_mesh = true; //メッシュを書き出すか
		bool convert_material = true; //マテリアルを書き出すか
		bool convert_animation = true; //アニメーションクリップを書き出すか
		bool convert_prefab = true; //プレハブを書き出すか
	};
}
