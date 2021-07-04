#include "Mesh_Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "Render_Manager.h"
#include "Debug.h"
#include "Include_ImGui.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "System_Function.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace BeastEngine;


ComPtr <ID3D11Buffer> Mesh_Renderer::constant_buffer_mesh;
ComPtr <ID3D11Buffer> Mesh_Renderer::constant_buffer_color;
shared_ptr<Shader>	  Mesh_Renderer::shadow_shader;
shared_ptr<Shader>	  Mesh_Renderer::vertex_shader;

void Mesh_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	enabled_old = enabled;

	gameobject = obj;
	transform = obj->transform;
	// 定数バッファの生成
	if (!constant_buffer_mesh)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Constant_Buffer_Mesh);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_mesh.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	if (!constant_buffer_color)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Constant_Buffer_Color);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_color.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	if (!shadow_shader)
	{
		shadow_shader = Shader::Create("Shader/Mesh_ShadowMap_Shader_VS.hlsl", "");
	}

	if (!vertex_shader)
	{
		vertex_shader = Shader::Create("Shader/Mesh_Shader_VS.hlsl", "");
	}

	if (file_path != "")
	{
		Set_Mesh(Mesh::Load_Mesh(file_path + file_name));
	}
	Set_Active(Get_Enabled());
}

void Mesh_Renderer::Set_Active(bool value)
{
	if (value)
	{
		if (mesh)
		{
			if (gameobject->Get_Active_In_Hierarchy())
			{
				if (Get_Enabled())
				{
					if (!is_called)
					{
						Engine::render_manager->Add(static_pointer_cast<Mesh_Renderer>(shared_from_this()));
						is_called = true;
					}
					is_disable = false;
				}
			}
		}
	}
}

void Mesh_Renderer::Recalculate_Frame()
{
	if (!recalculated_frame)
	{
		if (world_old != transform->Get_World_Matrix())
		{
			// メッシュ用定数バッファ更新
			buffer_mesh.world = transform->Get_World_Matrix();
			//AABB更新
			world_old = transform->Get_World_Matrix();
		}

		recalculated_frame = true;
	}
}

void Mesh_Renderer::Set_Mesh(shared_ptr<Mesh> Mesh_Data)
{
	if (Mesh_Data)
	{
		if (mesh != Mesh_Data)
		{
			mesh = Mesh_Data;
			file_name = mesh->name;
			file_path = mesh->file_path;
			//マテリアル
			for (size_t i = 0; i < mesh->default_material_passes.size(); i++)
			{
				shared_ptr<Material> Mat = make_shared<Material>();
				ifstream in_bin(mesh->default_material_passes[i], ios::binary);
				if (in_bin.is_open())
				{
					stringstream bin_s_stream;
					bin_s_stream << in_bin.rdbuf();
					cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
					binaryInputArchive(Mat);
					Material::Initialize(Mat, mesh->default_material_passes[i]);
					material.push_back(Mat);
				}
			}
			//AABB
			bounds = mesh->boundingbox;
			Set_Active(Get_Enabled());
		}
	}
}

void Mesh_Renderer::Render(Matrix V, Matrix P)
{
	if (mesh)
	{
		Recalculate_Frame();

		// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
		UINT stride = sizeof(Mesh::vertex);
		UINT offset = 0;
		DxSystem::device_context->IASetVertexBuffers(0, 1, mesh->vertex_buffer.GetAddressOf(), &stride, &offset);
		DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		for (auto& subset : mesh->subsets)
		{
			DxSystem::device_context->IASetInputLayout(vertex_shader->vertex_layout.Get());

			DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_mesh.GetAddressOf());
			DxSystem::device_context->UpdateSubresource(constant_buffer_mesh.Get(), 0, 0, &buffer_mesh, 0, 0);

			//マテリアルコンスタントバッファ
			Constant_Buffer_Color cbColor;
			cbColor.material_color = material[subset.material_ID]->color;
			DxSystem::device_context->PSSetConstantBuffers(2, 1, constant_buffer_color.GetAddressOf());
			DxSystem::device_context->UpdateSubresource(constant_buffer_color.Get(), 0, 0, &cbColor, 0, 0);

			material[subset.material_ID]->Active_Texture(); //PSSetSamplar PSSetShaderResources
			//シェーダーリソースのバインド
			vertex_shader->Activate_VS();
			material[subset.material_ID]->shader->Activate_PS(); //PSSetShader

			//ブレンドステート設定
			if (binding_blend_state != material[subset.material_ID]->blend_state)
			{
				DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(material[subset.material_ID]->blend_state), nullptr, 0xFFFFFFFF);
				binding_blend_state = material[subset.material_ID]->blend_state;
			}
			//ラスタライザ―設定
			if (binding_rasterizer_state != material[subset.material_ID]->rasterizer_state)
			{
				DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(material[subset.material_ID]->rasterizer_state));
				binding_rasterizer_state = material[subset.material_ID]->rasterizer_state;
			}
			//デプスステンシルステート設定
			if (binding_depth_stencil_State != material[subset.material_ID]->depth_stencil_state)
			{
				DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(material[subset.material_ID]->depth_stencil_state), 1);
				binding_depth_stencil_State = material[subset.material_ID]->depth_stencil_state;
			}

			// ↑で設定したリソースを利用してポリゴンを描画する。
			DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}
}

void Mesh_Renderer::Render_Shadow(Matrix V, Matrix P)
{
	if (mesh)
	{
		Recalculate_Frame();

		// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
		UINT stride = sizeof(Mesh::vertex);
		UINT offset = 0;
		DxSystem::device_context->IASetVertexBuffers(0, 1, mesh->vertex_buffer.GetAddressOf(), &stride, &offset);
		DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		for (auto& subset : mesh->subsets)
		{
			DxSystem::device_context->IASetInputLayout(vertex_shader->vertex_layout.Get());

			DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_mesh.GetAddressOf());
			DxSystem::device_context->UpdateSubresource(constant_buffer_mesh.Get(), 0, 0, &buffer_mesh, 0, 0);

			//シェーダーリソースのバインド
			shadow_shader->Activate(); //PS,VSSetShader
			// ↑で設定したリソースを利用してポリゴンを描画する。
			DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}
}

void Mesh_Renderer::Reset()
{
	mesh.reset();
	material.clear();
}

bool Mesh_Renderer::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("Mesh_Renderer", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = true;
	if (ImGui::BeginPopupContextItem("Mesh_Renderer_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<Mesh_Renderer>(shared_from_this()));
			removed = false;
		}
		ImGui::EndPopup();
	}
	if (!removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	static bool enable;
	enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		ImGui::Text(u8"現在のメッシュ::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());

		for (int i = 0; i < 5; ++i) ImGui::Spacing();
		static int ID_mat = 0;
		if (mesh)
		{
			if (ImGui::TreeNode(u8"マテリアル"))
			{
				for (shared_ptr<Material> mat : material)
				{
					ImGui::PushID(ID_mat);
					mat->Draw_ImGui();
					++ID_mat;
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
		}
		ID_mat = 0;
	}
	return true;
}