#pragma once
#include "Renderer.h"
#include "Mesh.h"
#include "Transform.h"

class SkinMesh_Renderer : public Renderer
{
public:

private:
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

	static ComPtr <ID3D11Buffer> ConstantBuffer_CbMesh;  //コンスタントバッファ(メッシュデータ)
	static ComPtr <ID3D11Buffer> ConstantBuffer_CbColor; //コンスタントバッファ(カラー)
	/*
	struct Node
	{
		const char* name;
		Node* parent;
		Vector3	scale;
		Quaternion rotation;
		Vector3	euler;
		Vector3	position;
		Matrix	localTransform;
		Matrix	worldTransform;
	};
	std::vector<Node>	  nodes;
	*/
	std::vector<weak_ptr<Transform>> bones;

	void Initialize(std::shared_ptr<GameObject> obj);
	void Render(Matrix V, Matrix P) override;
	void Render_Shadow(Matrix V, Matrix P) override;
	bool Draw_ImGui() override;
	void SetActive(bool value) override;

	void Set_Mesh(std::shared_ptr<Mesh> Mesh_Data); //メッシュデータを設定する

	// 行列計算
	//void CalculateLocalTransform();
	//void CalculateWorldTransform(const Matrix& world_transform);

	void Reset();

	std::string file_name;
	std::string file_path;

	static std::unique_ptr<Shader> shadow_shader;
	static std::unique_ptr<Shader> vertex_shader;

	friend class FBX_Converter;
	friend class Animator;
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Renderer>(this), file_name, file_path, bones);
	}
};

CEREAL_REGISTER_TYPE(SkinMesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Renderer, SkinMesh_Renderer)