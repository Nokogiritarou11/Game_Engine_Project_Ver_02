#include "SkinMesh_Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "Render_Manager.h"
#include "Include_ImGui.h"
#include "Compute_Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shadow_Manager.h"
#include "Asset_Manager.h"
#include "Debug_Draw_Manager.h"
#include "Misc.h"
#include "System_Function.h"
using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

ComPtr <ID3D11Buffer> SkinMesh_Renderer::constant_buffer_mesh;

void SkinMesh_Renderer::Initialize(const shared_ptr<GameObject>& obj)
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
			if (can_render)
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
	if (can_render)
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
	}
}

void SkinMesh_Renderer::Set_Mesh(shared_ptr<Mesh> Mesh_Data)
{
	if (Mesh_Data)
	{
		bool change = false;
		if (mesh) change = true;

		if (mesh != Mesh_Data)
		{
			mesh = Mesh_Data;
			can_render = true;
			file_path = mesh->file_path;
			//マテリアル
			for (auto& path : mesh->default_material_paths)
			{
				material.push_back(Material::Load_Material(path));
			}

			subset_count = static_cast<int>(mesh->subsets.size());
			subset_material_index.resize(subset_count);
			for (int i = 0; i < subset_count; ++i)
			{
				subset_material_index[i] = mesh->subsets[i].material_ID;
			}

			//コンピュートシェーダー設定
			compute_shader->Create_Buffer_Input(sizeof(Mesh::vertex), mesh->vertices.size(), &mesh->vertices[0]);
			compute_shader->Create_Buffer_Result(sizeof(Mesh::Vertex_Default_Buffer), mesh->vertices.size(), nullptr);

			//AABB
			if (change)
			{
				bounds = mesh->boundingbox;
			}

			Set_Active(Get_Enabled());
		}
	}
}

void SkinMesh_Renderer::Render(int subset_number)
{
	// バッファ設定
	auto& subset = mesh->subsets[subset_number];
	DxSystem::device_context->VSSetShaderResources(0, 1, compute_shader->Get_SRV().GetAddressOf());
	DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	for (size_t i = 0; i < material[subset.material_ID]->render_pass.size(); ++i)
	{
		//マテリアル設定
		material[subset.material_ID]->Active(i);
		// 描画
		DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
	}
}

void SkinMesh_Renderer::Render_Shadow(int subset_number)
{
	// バッファ設定
	auto& subset = mesh->subsets[subset_number];
	DxSystem::device_context->VSSetShaderResources(0, 1, compute_shader->Get_SRV().GetAddressOf());
	DxSystem::device_context->IASetIndexBuffer(mesh->index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	//マテリアル設定
	auto& pass = material[subset.material_ID]->render_pass[0];
	if (pass.texture_info.empty())
	{
		Engine::shadow_manager->Set_Default_Shadow_Alpha();
	}
	else
	{
		pass.main_texture->Set(1, Shader::Shader_Type::Pixel);
	}
	// 描画
	DxSystem::device_context->DrawIndexed(subset.index_count, subset.index_start, 0);
}

void SkinMesh_Renderer::Reset()
{
	mesh = nullptr;
	bones.clear();
	material.clear();
}

bool SkinMesh_Renderer::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("SkinMesh_Renderer", open)) return false;

	if (open)
	{
		const float window_width = ImGui::GetWindowContentRegionWidth();

		ImGui::Text(u8"現在のメッシュ::");
		ImGui::SameLine();
		ImGui::Text(mesh->file_path.c_str());

		if (ImGui::TreeNode(u8"バンディングボックス"))
		{
			const Vector3 min_scaled = transform->Get_Position() + (bounds.Get_Center() + bounds.Get_Min()) * transform->Get_Scale();
			const Vector3 max_scaled = transform->Get_Position() + (bounds.Get_Center() + bounds.Get_Max()) * transform->Get_Scale();
			btVector3 min = { min_scaled.x, min_scaled.y, min_scaled.z };
			btVector3 max = { max_scaled.x, max_scaled.y, max_scaled.z };
			Engine::debug_draw_manager->drawAabb(min, max, btVector3(0.0f, 0.65f, 1.0f));

			ImGui::Text(u8"中心オフセット");
			ImGui::SameLine(window_width * 0.4f);
			ImGui::SetNextItemWidth(-FLT_MIN);
			const Vector3 vec_center = bounds.Get_Center();
			float center[3] = { vec_center.x, vec_center.y, vec_center.z };
			if (ImGui::DragFloat3("##center", center, 0.1f))
			{
				bounds.Set_Center(center[0], center[1], center[2]);
			}

			ImGui::Text(u8"サイズ");
			ImGui::SameLine(window_width * 0.4f);
			ImGui::SetNextItemWidth(-FLT_MIN);
			const Vector3 vec_size = bounds.Get_Size();
			float size[3] = { vec_size.x, vec_size.y, vec_size.z };
			if (ImGui::DragFloat3("##size", size, 0.1f))
			{
				bounds.Set_Size(size[0], size[1], size[2]);
			}

			ImGui::TreePop();
		}

		int ID_mat = 0;
		if (can_render)
		{
			if (ImGui::TreeNode(u8"マテリアル"))
			{
				for (auto& mat : material)
				{
					ImGui::PushID(ID_mat);
					bool open = ImGui::TreeNodeEx(mat->name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap);
					ImGui::SameLine(window_width - 30.0f);
					if (ImGui::Button(u8"選択"))
					{
						string path = System_Function::Get_Open_File_Name("mat", "\\Assets\\Model");
						if (path != "")
						{
							material[ID_mat] = Material::Load_Material(path);
						}
					}
					if (open)
					{
						mat->Draw_ImGui();
						ImGui::TreePop();
					}
					++ID_mat;
					ImGui::PopID();
				}

				if (ImGui::TreeNode(u8"シェーダー一括変更"))
				{
					static const char* s_name[] = { "VS", "GS", "PS", "HS", "DS" };
					for (size_t i = 0; i < 5; ++i)
					{
						ImGui::PushID(i);
						ImGui::Text(s_name[i]);
						ImGui::SameLine(window_width - 25.0f);
						if (ImGui::Button(u8"選択"))
						{
							const string& path = System_Function::Get_Open_File_Name("", "\\Shader");
							if (path != "")
							{
								for (auto& mat : material)
								{
									mat->Set_Shader(path, static_cast<Shader::Shader_Type>(i));
									mat->Save();
								}
							}
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}
				ImGui::TreePop();
			}
		}
		ID_mat = 0;
	}
	return true;
}