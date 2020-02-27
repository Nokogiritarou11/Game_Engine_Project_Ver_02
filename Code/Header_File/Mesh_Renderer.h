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

	struct cbuffer /////////////////////////////////////////////////////�v�ύX
	{
		Matrix world_view_projection; //���[���h�E�r���[�E�v���W�F�N�V���������s��
		Matrix world; //���[���h�ϊ��s��
		Vector4 material_color; //�ގ��F
		Vector4 light_direction; //���C�g�i�s���� 
	};

	ComPtr <ID3D11Buffer> ConstantBuffer; //�R���X�^���g�o�b�t�@
	std::shared_ptr<Mesh> mesh_data;
};