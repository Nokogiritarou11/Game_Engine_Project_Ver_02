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
		void Load_From_FBX(bool& convert_mesh,bool& convert_material, bool& convert_animation, bool& convert_prefab);
		void Convert_Animation(std::vector<std::shared_ptr<BeastEngine::GameObject>>& bones);
		void Convert_Mesh();
		void Bone_Decompose(std::vector<BeastEngine::Matrix>& matrixes, std::vector<std::shared_ptr<BeastEngine::GameObject>>& bones, std::shared_ptr<Transform>& trans);
		std::shared_ptr<BeastEngine::GameObject> Create_GameObject(std::string n);

		std::shared_ptr<BeastEngine::Model_Data> model;
		int load_state;
		bool convert_mesh = true;
		bool convert_material = true;
		bool convert_animation = true;
		bool convert_prefab = true;
	};
}