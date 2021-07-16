#pragma once
#include <memory>
#include <string>

namespace BeastEngine
{
	class Model_Data;
	class GameObject;
	class Transform;
	struct Matrix;

	class FBX_Converter
	{
	public:
		void Draw_ImGui();
	private:
		std::shared_ptr<BeastEngine::Model_Data> model;
		void Load_From_FBX(bool& convert_mesh, bool& convert_animation, bool& convert_prefab);
		void Convert_Animation(std::vector<std::shared_ptr<BeastEngine::GameObject>>& bones);
		void Convert_Mesh();
		void Bone_Decompose(std::vector<BeastEngine::Matrix>& matrixes, std::vector<std::shared_ptr<BeastEngine::GameObject>>& bones, std::shared_ptr<Transform>& trans);
		std::shared_ptr<BeastEngine::GameObject> Create_GameObject(std::string n);
		int load_state;
	};
}