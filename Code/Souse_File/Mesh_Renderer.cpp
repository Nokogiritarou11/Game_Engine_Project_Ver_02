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
using namespace std;

ComPtr <ID3D11Buffer> Mesh_Renderer::ConstantBuffer_CbMesh;
ComPtr <ID3D11Buffer> Mesh_Renderer::ConstantBuffer_CbColor;
unique_ptr<Shader>	  Mesh_Renderer::shadow_shader;
unique_ptr<Shader>	  Mesh_Renderer::vertex_shader;

void Mesh_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	enabled_old = enabled;

	gameObject = obj;
	transform = obj->transform;
	// 定数バッファの生成
	if (!ConstantBuffer_CbMesh)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CbMesh);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer_CbMesh.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	if (!ConstantBuffer_CbColor)
	{
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CbColor);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		HRESULT hr = DxSystem::Device->CreateBuffer(&bd, nullptr, ConstantBuffer_CbColor.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	if (!shadow_shader)
	{
		shadow_shader = make_unique<Shader>();
		shadow_shader->Create_VS(L"Shader/Mesh_ShadowMap_Shader_VS.hlsl", "VSMain");
	}

	if (!vertex_shader)
	{
		vertex_shader = make_unique<Shader>();
		vertex_shader->Create_VS(L"Shader/Mesh_Shader_VS.hlsl", "VSMain");
	}

	if (file_path != "")
	{
		Set_Mesh(Mesh::Load_Mesh(file_path.c_str(), file_name.c_str()));
	}
	SetActive(enableSelf());
}

void Mesh_Renderer::SetActive(bool value)
{
	if (value)
	{
		if (mesh_data)
		{
			if (gameObject->activeInHierarchy())
			{
				if (enableSelf())
				{
					if (!IsCalled)
					{
						Engine::render_manager->Add(static_pointer_cast<Mesh_Renderer>(shared_from_this()));
						IsCalled = true;
					}
					Disable_flg = false;
				}
			}
		}
	}
}

void Mesh_Renderer::Set_Mesh(shared_ptr<Mesh> Mesh_Data)
{
	if (mesh_data != Mesh_Data)
	{
		mesh_data = Mesh_Data;
		file_name = mesh_data->name;
		file_path = mesh_data->file_path;
		//マテリアル
		for (size_t i = 0; i < mesh_data->Default_Material_Passes.size(); i++)
		{
			shared_ptr<Material> Mat = make_shared<Material>();
			ifstream in_bin(mesh_data->Default_Material_Passes[i], ios::binary);
			if (in_bin.is_open())
			{
				stringstream bin_s_stream;
				bin_s_stream << in_bin.rdbuf();
				cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
				binaryInputArchive(Mat);
				Material::Initialize(Mat, mesh_data->Default_Material_Passes[i]);
				material.push_back(Mat);
			}
		}
		SetActive(enableSelf());
	}
}
void Mesh_Renderer::Render(Matrix V, Matrix P)
{
	if (mesh_data)
	{
		for (auto& mesh : mesh_data->meshes)
		{
			// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
			UINT stride = sizeof(Mesh::vertex);
			UINT offset = 0;
			DxSystem::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
			DxSystem::DeviceContext->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			for (auto& subset : mesh.subsets)
			{
				DxSystem::DeviceContext->IASetInputLayout(vertex_shader->VertexLayout.Get());

				// メッシュ用定数バッファ更新
				CbMesh cbMesh;
				::memset(&cbMesh, 0, sizeof(cbMesh));
				cbMesh.world = transform->Get_world_matrix();
				DxSystem::DeviceContext->VSSetConstantBuffers(1, 1, ConstantBuffer_CbMesh.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbMesh.Get(), 0, 0, &cbMesh, 0, 0);

				//マテリアルコンスタントバッファ
				CbColor cbColor;
				cbColor.materialColor = material[subset.material_ID]->color;
				DxSystem::DeviceContext->PSSetConstantBuffers(2, 1, ConstantBuffer_CbColor.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbColor.Get(), 0, 0, &cbColor, 0, 0);

				material[subset.material_ID]->Active_Texture(); //PSSetSamplar PSSetShaderResources
				//シェーダーリソースのバインド
				vertex_shader->Activate_VS();
				material[subset.material_ID]->shader->Activate_PS(); //PSSetShader

				//ブレンドステート設定
				if (Set_BlendState != material[subset.material_ID]->BlendState)
				{
					DxSystem::DeviceContext->OMSetBlendState(DxSystem::GetBlendState(material[subset.material_ID]->BlendState), nullptr, 0xFFFFFFFF);
					Set_BlendState = material[subset.material_ID]->BlendState;
				}
				//ラスタライザ―設定
				if (Set_RasterizerState != material[subset.material_ID]->RasterizerState)
				{
					DxSystem::DeviceContext->RSSetState(DxSystem::GetRasterizerState(material[subset.material_ID]->RasterizerState));
					Set_RasterizerState = material[subset.material_ID]->RasterizerState;
				}
				//デプスステンシルステート設定
				if (Set_DepthStencilState != material[subset.material_ID]->DepthStencilState)
				{
					DxSystem::DeviceContext->OMSetDepthStencilState(DxSystem::GetDephtStencilState(material[subset.material_ID]->DepthStencilState), 1);
					Set_DepthStencilState = material[subset.material_ID]->DepthStencilState;
				}

				// ↑で設定したリソースを利用してポリゴンを描画する。
				DxSystem::DeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}
}
void Mesh_Renderer::Render_Shadow(Matrix V, Matrix P)
{
	if (mesh_data)
	{
		for (auto& mesh : mesh_data->meshes)
		{
			// 使用する頂点バッファやシェーダーなどをGPUに教えてやる。
			UINT stride = sizeof(Mesh::vertex);
			UINT offset = 0;
			DxSystem::DeviceContext->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);
			DxSystem::DeviceContext->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			for (auto& subset : mesh.subsets)
			{
				DxSystem::DeviceContext->IASetInputLayout(vertex_shader->VertexLayout.Get());

				// メッシュ用定数バッファ更新
				CbMesh cbMesh;
				::memset(&cbMesh, 0, sizeof(cbMesh));
				cbMesh.world = transform->Get_world_matrix();
				DxSystem::DeviceContext->VSSetConstantBuffers(1, 1, ConstantBuffer_CbMesh.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbMesh.Get(), 0, 0, &cbMesh, 0, 0);

				//マテリアルコンスタントバッファ
				CbColor cbColor;
				cbColor.materialColor = material[subset.material_ID]->color;
				DxSystem::DeviceContext->PSSetConstantBuffers(2, 1, ConstantBuffer_CbColor.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbColor.Get(), 0, 0, &cbColor, 0, 0);

				//シェーダーリソースのバインド
				shadow_shader->Activate(); //PS,VSSetShader
				// ↑で設定したリソースを利用してポリゴンを描画する。
				DxSystem::DeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}
}

void Mesh_Renderer::Reset()
{
	mesh_data.reset();
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
	enable = enableSelf();
	if (ImGui::Checkbox("##enable", &enable))
	{
		SetEnabled(enable);
	}

	if (open)
	{
		ImGui::Text(u8"現在のメッシュ::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		if (ImGui::Button(u8"メッシュを選択"))
		{
			string path = System_Function::Get_Open_File_Name();
			//Debug::Log(path);
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;//7
				int ext_i = path.find_last_of(".");//10
				string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
				string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				if (extname == ".fbx" || extname == ".mesh")
				{
					Reset();
					Set_Mesh(Mesh::Load_Mesh(pathname.c_str(), filename.c_str()));
				}
				else
				{
					Debug::Log("ファイル形式が対応していません");
				}
			}
		}

		for (int i = 0; i < 5; ++i) ImGui::Spacing();
		static int ID_mat = 0;
		if (mesh_data)
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