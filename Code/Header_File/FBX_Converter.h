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
		static void Convert_From_FBX(const char* file_path, const char* fbx_filename);
	private:
		static void Convert_Animation(std::shared_ptr<BeastEngine::Model_Data> model, std::vector<std::shared_ptr<BeastEngine::GameObject>>& bones);
		static void Convert_Mesh(std::shared_ptr<BeastEngine::Model_Data> model);
		static std::shared_ptr<BeastEngine::GameObject> Create_GameObject(std::string n);
		static void Bone_Decompose(std::vector<BeastEngine::Matrix>& matrixes, std::vector<std::shared_ptr<BeastEngine::GameObject>>& bones, std::shared_ptr<Transform> trans);
	};
}