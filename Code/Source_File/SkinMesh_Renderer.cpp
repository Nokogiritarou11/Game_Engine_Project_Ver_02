#include "SkinMesh_Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "Render_Manager.h"
#include "Debug.h"
#include "Include_ImGui.h"
#include "Shader.h"
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
ComPtr <ID3D11Buffer> SkinMesh_Renderer::constant_buffer_color;
shared_ptr<Shader> SkinMesh_Renderer::shadow_shader;
shared_ptr<Shader> SkinMesh_Renderer::vertex_shader;

void SkinMesh_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	enabled_old = enabled;

	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;
	// �萔�o�b�t�@�̐���
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
		shadow_shader = make_unique<Shader>();
		shadow_shader = Shader::Create("Shader/SkinMesh_ShadowMap_Shader_VS.hlsl", "");
	}

	if (!vertex_shader)
	{
		vertex_shader = make_unique<Shader>();
		vertex_shader = Shader::Create("Shader/SkinMesh_Shader_VS.hlsl", "");
	}

	if (file_path != "")
	{
		Set_Mesh(Mesh::Load_Mesh(file_path + file_name));
	}

	Set_Active(Get_Enabled());
}

void SkinMesh_Renderer::Set_Active(bool value)
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
	// ���b�V���p�萔�o�b�t�@�X�V
	if (!bones.empty())
	{
		for (size_t i = 0; i < bones.size(); ++i)
		{
			Matrix world_transform = bones[i].lock()->Get_World_Matrix();
			Matrix inverse_transform = mesh->inverse_matrixes[i];
			Matrix bone_transform = inverse_transform * world_transform;
			buffer_mesh.bone_transforms[i] = bone_transform;
		}
	}
	else
	{
		buffer_mesh.bone_transforms[0] = transform->Get_World_Matrix();
	}
}

void SkinMesh_Renderer::Set_Mesh(shared_ptr<Mesh> Mesh_Data)
{
	if (Mesh_Data)
	{
		if (mesh != Mesh_Data)
		{
			mesh = Mesh_Data;
			file_name = mesh->name;
			file_path = mesh->file_path;
			//�}�e���A��
			for (auto& pass : mesh->default_material_passes)
			{
				shared_ptr<Material> Mat = make_shared<Material>();
				ifstream in_bin(pass, ios::binary);
				if (in_bin.is_open())
				{
					stringstream bin_s_stream;
					bin_s_stream << in_bin.rdbuf();
					cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
					binaryInputArchive(Mat);
					Material::Initialize(Mat, pass);
					material.emplace_back(Mat);
				}
			}
			Set_Active(Get_Enabled());
		}
	}
}

void SkinMesh_Renderer::Render(Matrix V, Matrix P)
{
	if (mesh)
	{
		if (!recalculated_frame)
		{
			Recalculate_Frame();
		}

		// �g�p���钸�_�o�b�t�@��V�F�[�_�[�Ȃǂ�GPU�ɋ����Ă��B
		UINT stride = sizeof(Mesh::vertex);
		UINT offset = 0;
		DxSystem::device_context->IASetVertexBuffers(0, 1, mesh->vertex_buffer.GetAddressOf(), &stride, &offset);
		DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		for (auto& subset : mesh->subsets)
		{
			DxSystem::device_context->IASetInputLayout(vertex_shader->vertex_layout.Get());

			DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_mesh.GetAddressOf());
			DxSystem::device_context->UpdateSubresource(constant_buffer_mesh.Get(), 0, 0, &buffer_mesh, 0, 0);

			//�}�e���A���R���X�^���g�o�b�t�@
			Constant_Buffer_Color cbColor;
			cbColor.material_color = material[subset.material_ID]->color;
			DxSystem::device_context->PSSetConstantBuffers(2, 1, constant_buffer_color.GetAddressOf());
			DxSystem::device_context->UpdateSubresource(constant_buffer_color.Get(), 0, 0, &cbColor, 0, 0);

			material[subset.material_ID]->Active_Texture(); //PSSetSamplar PSSetShaderResources
			//�V�F�[�_�[���\�[�X�̃o�C���h
			vertex_shader->Activate_VS(); //VSsetShader
			material[subset.material_ID]->shader->Activate_PS(); //PSSetShader

			//�u�����h�X�e�[�g�ݒ�
			if (binding_blend_state != material[subset.material_ID]->blend_state)
			{
				DxSystem::device_context->OMSetBlendState(DxSystem::Get_Blend_State(material[subset.material_ID]->blend_state), nullptr, 0xFFFFFFFF);
				binding_blend_state = material[subset.material_ID]->blend_state;
			}
			//���X�^���C�U�\�ݒ�
			if (binding_rasterizer_state != material[subset.material_ID]->rasterizer_state)
			{
				DxSystem::device_context->RSSetState(DxSystem::Get_Rasterizer_State(material[subset.material_ID]->rasterizer_state));
				binding_rasterizer_state = material[subset.material_ID]->rasterizer_state;
			}
			//�f�v�X�X�e���V���X�e�[�g�ݒ�
			if (binding_depth_stencil_State != material[subset.material_ID]->depth_stencil_state)
			{
				DxSystem::device_context->OMSetDepthStencilState(DxSystem::Get_DephtStencil_State(material[subset.material_ID]->depth_stencil_state), 1);
				binding_depth_stencil_State = material[subset.material_ID]->depth_stencil_state;
			}

			// ���Őݒ肵�����\�[�X�𗘗p���ă|���S����`�悷��B
			DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
		recalculated_frame = true;
	}
}

void SkinMesh_Renderer::Render_Shadow(Matrix V, Matrix P)
{
	if (mesh)
	{
		if (!recalculated_frame)
		{
			Recalculate_Frame();
		}

		// �g�p���钸�_�o�b�t�@��V�F�[�_�[�Ȃǂ�GPU�ɋ����Ă��B
		UINT stride = sizeof(Mesh::vertex);
		UINT offset = 0;
		DxSystem::device_context->IASetVertexBuffers(0, 1, mesh->vertex_buffer.GetAddressOf(), &stride, &offset);
		DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		for (auto& subset : mesh->subsets)
		{
			DxSystem::device_context->IASetInputLayout(vertex_shader->vertex_layout.Get());

			DxSystem::device_context->VSSetConstantBuffers(1, 1, constant_buffer_mesh.GetAddressOf());
			DxSystem::device_context->UpdateSubresource(constant_buffer_mesh.Get(), 0, 0, &buffer_mesh, 0, 0);

			//�V�F�[�_�[���\�[�X�̃o�C���h
			shadow_shader->Activate(); //PS,VSSetShader
			// ���Őݒ肵�����\�[�X�𗘗p���ă|���S����`�悷��B
			DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		}
		recalculated_frame = true;
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
		if (ImGui::Selectable(u8"�R���|�[�l���g���폜"))
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
		ImGui::Text(u8"���݂̃��b�V��::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());

		int ID_mat = 0;
		if (mesh)
		{
			if (ImGui::TreeNode(u8"�}�e���A��"))
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