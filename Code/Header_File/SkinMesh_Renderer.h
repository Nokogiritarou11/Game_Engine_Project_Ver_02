#pragma once
#include "Renderer.h"
#include "Mesh.h"

class SkinMesh_Renderer : public Renderer
{
public:

	void Initialize();
	void Initialize(std::shared_ptr<GameObject> obj);
	void Set_Mesh(std::shared_ptr<Mesh> Mesh_Data);
	void Render(Matrix V, Matrix P, bool Use_Material = true);

	bool Draw_ImGui();

	struct Node
	{
		const char* name;
		Node* parent;
		Vector3	scale;
		Vector4	rotation;
		Vector3	position;
		Matrix	localTransform;
		Matrix	worldTransform;
	};

	// 行列計算
	void CalculateLocalTransform();
	void CalculateWorldTransform(const Matrix& world_transform);

	//const std::vector<Node>& GetNodes() const { return nodes; }
	//std::vector<Node>& GetNodes() { return nodes; }

	std::shared_ptr<Mesh> mesh_data;

	struct CbMesh
	{
#define MAX_BONES 128
		Matrix bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	};
	struct CbColor
	{
		Vector4	materialColor;
	};

	static ComPtr <ID3D11Buffer> ConstantBuffer_CbMesh; //コンスタントバッファ
	static ComPtr <ID3D11Buffer> ConstantBuffer_CbColor; //コンスタントバッファ

	std::vector<Node>	  nodes;

private:

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Renderer>(this), file_name, file_pass);
	}

	std::string file_name;
	std::string file_pass;

};

CEREAL_REGISTER_TYPE(SkinMesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Renderer, SkinMesh_Renderer)