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

	class Fbx_Converter
	{
	public:
		void Draw_ImGui();

	private:
		void Load_From_Fbx(bool& convert_mesh,bool& convert_material, bool& convert_animation, bool& convert_prefab) const;
		void Convert_Animation(const std::vector<std::shared_ptr<GameObject>>& bones) const;
		void Convert_Mesh() const;
		static void Bone_Decompose(std::vector<Matrix>& matrixes, std::vector<std::shared_ptr<GameObject>>& bones, const std::shared_ptr<Transform>& trans);
		[[nodiscard]] std::shared_ptr<GameObject> Create_GameObject(const std::string& n) const;

		std::shared_ptr<Model_Data> model;
		int load_state = 0;
		bool convert_mesh = true;
		bool convert_material = true;
		bool convert_animation = true;
		bool convert_prefab = true;
	};
}
