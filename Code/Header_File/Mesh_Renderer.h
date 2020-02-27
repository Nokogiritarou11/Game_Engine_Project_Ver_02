#pragma once
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"

class Mesh_Renderer : public Renderer
{
public:

	Mesh_Renderer();
	~Mesh_Renderer();

	void Initialize(std::shared_ptr<GameObject> obj);
	void Set_Mesh(std::shared_ptr<Mesh> Mesh_Data);
	void Render(std::shared_ptr<Camera> Render_Camera);
private:

	struct cbuffer /////////////////////////////////////////////////////要変更
	{
		Matrix world_view_projection; //ワールド・ビュー・プロジェクション合成行列
		Matrix world; //ワールド変換行列
		Vector4 material_color; //材質色
		Vector4 light_direction; //ライト進行方向 
	};

	ComPtr <ID3D11Buffer> ConstantBuffer; //コンスタントバッファ
	std::shared_ptr<Mesh> mesh_data;
};