#include "Mesh_Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Include_ImGui.h"
#include "Engine.h"
#include "Render_Manager.h"
using namespace std;

Mesh_Renderer::Mesh_Renderer()
{
}

Mesh_Renderer::~Mesh_Renderer()
{
}

void Mesh_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	Engine::render_manager->Add(static_pointer_cast<Mesh_Renderer>(shared_from_this()));
	// 定数バッファの生成
	if (!ConstantBuffer)
	{
		D3D11_BUFFER_DESC bd   = {};
		bd.Usage               = D3D11_USAGE_DEFAULT;
		bd.ByteWidth           = sizeof(cbuffer);
		bd.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags      = 0;
		bd.MiscFlags           = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

void Mesh_Renderer::Set_Mesh(std::shared_ptr<Mesh> Mesh_Data)
{
	/*
	mesh_data = Mesh_Data;
	//マテリアル
	unsigned long Subset_ID = 0;
	for (int i = 0; i < mesh_data->meshes.size(); i++)
	{
		for (int j = 0; j < mesh_data->meshes[i].subsets.size(); j++)
		{
			mesh_data->meshes[i].subsets[j].diffuse.ID = Subset_ID;
			string Mat_Name = mesh_data->name + "_" + mesh_data->meshes[i].subsets[j].diffuse.TexName;
			shared_ptr<Material> Mat = Material::Create(Mat_Name, L"Shader/Standard_Shader_VS.hlsl", L"Shader/Standard_Shader_PS.hlsl", mesh_data->meshes[i].subsets[j].diffuse.TexPass.c_str());
			material.emplace_back(Mat);
			Subset_ID++;
		}
	}
	// ノード
	const std::vector<Mesh::Node>& res_nodes = mesh_data->nodes;

	nodes.resize(res_nodes.size());
	for (size_t nodeIndex = 0; nodeIndex < nodes.size(); ++nodeIndex)
	{
		auto&& src = res_nodes.at(nodeIndex);
		auto&& dst = nodes.at(nodeIndex);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotation = src.rotation;
		dst.position = src.position;
	}
	*/
}

void Mesh_Renderer::Render(std::shared_ptr<Camera> Render_Camera)
{
	/*
	// ワールド行列、ビュー行列、プロジェクション行列を合成し行列データを取り出す。
	XMMATRIX WVP;
	XMFLOAT4X4 world_view_projection;
	WVP = XMLoadFloat4x4(&transform->Get_world_matrix()) * XMLoadFloat4x4(&Render_Camera->V) * XMLoadFloat4x4(&Render_Camera->P);
	XMStoreFloat4x4(&world_view_projection, WVP);

	DxSystem::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto& mesh : mesh_data->skin_meshes)
	{
		// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
		UINT stride = sizeof(Mesh::vertex);
		UINT offset = 0;
		DxSystem::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
		DxSystem::DeviceContext->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		for (auto& subset : mesh.subsets)
		{
			// 定数バッファの内容を更新
			cbuffer data;
			data.material_color = material[subset.diffuse.ID]->color;
			data.light_direction = DxSystem::Light_Direction;
			XMStoreFloat4x4(&data.world_view_projection,
				XMLoadFloat4x4(&mesh.global_transform) *
				XMLoadFloat4x4(&transform->coordinate_conversion) *
				XMLoadFloat4x4(&world_view_projection));
			XMStoreFloat4x4(&data.world,
				XMLoadFloat4x4(&mesh.global_transform) *
				XMLoadFloat4x4(&transform->coordinate_conversion) *
				XMLoadFloat4x4(&transform->Get_world_matrix()));
			DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer.Get(), 0, nullptr, &data, 0, 0);
			DxSystem::DeviceContext->VSSetConstantBuffers(0, 1, ConstantBuffer.GetAddressOf());
			DxSystem::DeviceContext->IASetInputLayout(material[subset.diffuse.ID]->shader->VertexLayout.Get());
			//シェーダーリソースのバインド
			material[subset.diffuse.ID]->texture->Set(); //PSSetSamplar PSSetShaderResources
			material[subset.diffuse.ID]->shader->Activate(); //PS,VSSetShader
			// ↑で設定したリソースを利用してポリゴンを描画する。
			DxSystem::DeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}
	*/
}