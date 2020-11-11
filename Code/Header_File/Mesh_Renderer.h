#pragma once
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"

class Mesh_Renderer : public Renderer
{
public:
	void Set_Mesh(std::shared_ptr<Mesh> Mesh_Data); //メッシュデータを設定する

private:
	std::shared_ptr<Mesh> mesh_data;

	void Initialize(std::shared_ptr<GameObject> obj) override;
	void Render(Matrix V, Matrix P) override;
	void Render_Shadow(Matrix V, Matrix P) override;
	bool Draw_ImGui() override;
	void SetActive(bool value) override;

	struct CbMesh
	{
		Matrix world = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	};

	struct CbColor
	{
		Vector4	materialColor;
	};

	static ComPtr <ID3D11Buffer> ConstantBuffer_CbMesh;  //コンスタントバッファ(メッシュデータ)
	static ComPtr <ID3D11Buffer> ConstantBuffer_CbColor; //コンスタントバッファ(カラー)

	void Reset();

	std::string file_name;
	std::string file_path;

	static std::unique_ptr<Shader> shadow_shader;
	static std::unique_ptr<Shader> vertex_shader;

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Renderer>(this), file_name, file_path);
	}
};

CEREAL_REGISTER_TYPE(Mesh_Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Renderer, Mesh_Renderer)