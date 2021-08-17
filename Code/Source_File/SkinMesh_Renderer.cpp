#include "SkinMesh_Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "Render_Manager.h"
#include "Debug.h"
#include "Include_ImGui.h"
#include "Compute_Shader.h"
#include "Material.h"
#include "Mesh.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "Engine.h"
#include "Asset_Manager.h"
#include "System_Function.h"
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

ComPtr <ID3D11Buffer> SkinMesh_Renderer::constant_buffer_mesh;

void SkinMesh_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	enabled_old = enabled;

	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;
	// 定数バッファの生成
	if (!constant_buffer_mesh)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(Constant_Buffer_Mesh);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bd, nullptr, constant_buffer_mesh.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	if (!compute_shader)
	{
		compute_shader = Compute_Shader::Create("Shader/SkinMesh_CS.hlsl");
	}

	if (file_path != "")
	{
		Set_Mesh(Mesh::Load_Mesh(file_path));
	}

	Set_Active(Get_Enabled());
}

void SkinMesh_Renderer::Set_Active(bool value)
{
	if (value)
	{
		if (!is_called)
		{
			if (mesh)
			{
				if (gameobject->Get_Active_In_Hierarchy())
				{
					if (Get_Enabled())
					{
						Engine::render_manager->Add(static_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
						is_called = true;
					}
				}
			}
		}
	}
}

void SkinMesh_Renderer::Recalculate_Frame()
{
	// メッシュ用定数バッファ更新
	if (!bones.empty())
	{
		size_t size = bones.size();
		for (size_t i = 0; i < size; ++i)
		{
			Matrix world_transform = bones[i].lock()->Get_World_Matrix();
			Matrix inverse_transform = mesh->inverse_matrixes[i];
			Matrix bone_transform = inverse_transform * world_transform;
			buffer_mesh.bone_transforms[i] = bone_transform;
		}
		//頂点コンスタントバッファ
		DxSystem::device_context->CSSetConstantBuffers(1, 1, constant_buffer_mesh.GetAddressOf());
		{
			UINT subresourceIndex = 0;
			D3D11_MAPPED_SUBRESOURCE mapped;
			auto hr = DxSystem::device_context->Map(constant_buffer_mesh.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
			if (SUCCEEDED(hr))
			{
				memcpy(mapped.pData, &buffer_mesh, sizeof(Constant_Buffer_Mesh));
				DxSystem::device_context->Unmap(constant_buffer_mesh.Get(), subresourceIndex);
			}
		}

		compute_shader->Run();
	}
	else
	{
		buffer_mesh.bone_transforms[0] = transform->Get_World_Matrix();
	}

	recalculated_frame = true;
}

void SkinMesh_Renderer::Set_Mesh(shared_ptr<Mesh> Mesh_Data)
{
	if (Mesh_Data)
	{
		if (mesh != Mesh_Data)
		{
			mesh = Mesh_Data;
			file_path = mesh->file_path;
			//マテリアル
			for (auto& pass : mesh->default_material_passes)
			{
				material.push_back(Material::Load_Material(pass));
			}

			//コンピュートシェーダー設定
			compute_shader->Create_Buffer_Input(sizeof(Mesh::vertex), mesh->vertices.size(), &mesh->vertices[0]);
			compute_shader->Create_Buffer_Result(sizeof(Mesh::vertex_default_buffer), mesh->vertices.size(), nullptr);

			//AABB
			bounds = mesh->boundingbox;

			Set_Active(Get_Enabled());
		}
	}
}

void SkinMesh_Renderer::Render()
{
	if (mesh)
	{
		if (!recalculated_frame)
		{
			Recalculate_Frame();
		}

		// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
		DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		for (auto& subset : mesh->subsets)
		{
			//マテリアル設定
			material[subset.material_ID]->Active();

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

			DxSystem::device_context->VSSetShaderResources(0, 1, compute_shader->Get_SRV().GetAddressOf());
			// ↑で設定したリソースを利用してポリゴンを描画する。
			DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}
}

void SkinMesh_Renderer::Render_Shadow()
{
	if (mesh)
	{
		if (!recalculated_frame)
		{
			Recalculate_Frame();
		}

		// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
		DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		for (auto& subset : mesh->subsets)
		{
			DxSystem::device_context->VSSetShaderResources(0, 1, compute_shader->Get_SRV().GetAddressOf());
			DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
	}
}

void SkinMesh_Renderer::Reset()
{
	mesh = nullptr;
	bones.clear();
	material.clear();
}

bool SkinMesh_Renderer::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	bool open = ImGui::CollapsingHeader("SkinMesh_Renderer", ImGuiTreeNodeFlags_AllowItemOverlap);

	bool removed = false;
	if (ImGui::BeginPopupContextItem("SkinMesh_Renderer_sub"))
	{
		if (ImGui::Selectable(u8"コンポーネントを削除"))
		{
			Object::Destroy(dynamic_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
			removed = true;
		}
		ImGui::EndPopup();
	}
	if (removed)
	{
		return false;
	}

	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
	bool enable = Get_Enabled();
	if (ImGui::Checkbox("##enable", &enable))
	{
		Set_Enabled(enable);
	}

	if (open)
	{
		ImGui::Text(u8"現在のメッシュ::");
		ImGui::SameLine();
		ImGui::Text(mesh->file_path.c_str());

		int ID_mat = 0;
		if (mesh)
		{
			if (ImGui::TreeNode(u8"マテリアル"))
			{
				for (auto& mat : material)
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