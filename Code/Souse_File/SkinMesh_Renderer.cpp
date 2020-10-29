#include "SkinMesh_Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Engine.h"
#include "Debug.h"
#include "Include_ImGui.h"
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "System_Function.h"
using namespace std;

ComPtr <ID3D11Buffer> SkinMesh_Renderer::ConstantBuffer_CbMesh;
ComPtr <ID3D11Buffer> SkinMesh_Renderer::ConstantBuffer_CbColor;

void SkinMesh_Renderer::Initialize()
{
	SetActive(enableSelf());
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
		assert(SUCCEEDED(hr));
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
		assert(SUCCEEDED(hr));
	}
}
void SkinMesh_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	SetActive(enableSelf());
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
		assert(SUCCEEDED(hr));
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
		assert(SUCCEEDED(hr));
	}

	if (file_pass != "")
	{
		Set_Mesh(Mesh::Load_Mesh(file_pass.c_str(), file_name.c_str()));
	}
}

void SkinMesh_Renderer::SetActive(bool value)
{
	if (value)
	{
		if (gameObject->activeSelf())
		{
			if (enableSelf())
			{
				if (!IsCalled)
				{
					Engine::render_manager->Add(static_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
					IsCalled = true;
				}
				Disable_flg = false;
			}
		}
	}
}

void SkinMesh_Renderer::Set_Mesh(shared_ptr<Mesh> Mesh_Data)
{
	if (mesh_data != Mesh_Data)
	{
		mesh_data = Mesh_Data;
		file_name = mesh_data->name;
		file_pass = mesh_data->file_pass;
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
			else
			{

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
	}
}
void SkinMesh_Renderer::Render(Matrix V, Matrix P)
{
	CalculateLocalTransform();
	const Matrix C = {
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	CalculateWorldTransform(C * transform->Get_world_matrix());

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
				DxSystem::DeviceContext->IASetInputLayout(material[subset.material_ID]->shader->VertexLayout.Get());

				// メッシュ用定数バッファ更新
				CbMesh cbMesh;
				::memset(&cbMesh, 0, sizeof(cbMesh));
				if (mesh.nodeIndices.size() > 0)
				{
					for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
					{
						DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
						DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(&mesh.inverseTransforms.at(i));
						DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
						DirectX::XMStoreFloat4x4(&cbMesh.bone_transforms[i], bone_transform);
					}
				}
				else
				{
					cbMesh.bone_transforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
				}
				DxSystem::DeviceContext->VSSetConstantBuffers(1, 1, ConstantBuffer_CbMesh.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbMesh.Get(), 0, 0, &cbMesh, 0, 0);

				//マテリアルコンスタントバッファ
				CbColor cbColor;
				cbColor.materialColor = material[subset.material_ID]->color;
				DxSystem::DeviceContext->PSSetConstantBuffers(2, 1, ConstantBuffer_CbColor.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbColor.Get(), 0, 0, &cbColor, 0, 0);

				material[subset.material_ID]->Active_Texture(); //PSSetSamplar PSSetShaderResources
				//シェーダーリソースのバインド
				material[subset.material_ID]->Active_Shader(); //PS,VSSetShader
				// ↑で設定したリソースを利用してポリゴンを描画する。
				DxSystem::DeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}
}
void SkinMesh_Renderer::Render(Matrix V, Matrix P, bool Use_Material = true, shared_ptr<Shader> shader = nullptr)
{
	CalculateLocalTransform();
	const Matrix C = {
		-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	CalculateWorldTransform(C * transform->Get_world_matrix());

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
				DxSystem::DeviceContext->IASetInputLayout(material[subset.material_ID]->shader->VertexLayout.Get());

				// メッシュ用定数バッファ更新
				CbMesh cbMesh;
				::memset(&cbMesh, 0, sizeof(cbMesh));
				if (mesh.nodeIndices.size() > 0)
				{
					for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
					{
						DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
						DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(&mesh.inverseTransforms.at(i));
						DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
						DirectX::XMStoreFloat4x4(&cbMesh.bone_transforms[i], bone_transform);
					}
				}
				else
				{
					cbMesh.bone_transforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
				}
				DxSystem::DeviceContext->VSSetConstantBuffers(1, 1, ConstantBuffer_CbMesh.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbMesh.Get(), 0, 0, &cbMesh, 0, 0);

				//マテリアルコンスタントバッファ
				CbColor cbColor;
				cbColor.materialColor = material[subset.material_ID]->color;
				DxSystem::DeviceContext->PSSetConstantBuffers(2, 1, ConstantBuffer_CbColor.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbColor.Get(), 0, 0, &cbColor, 0, 0);

				//material[subset.material_ID]->texture->Set(Use_Material); //PSSetSamplar PSSetShaderResources
				//シェーダーリソースのバインド
				shader->Activate(); //PS,VSSetShader
				// ↑で設定したリソースを利用してポリゴンを描画する。
				DxSystem::DeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}
}

// ローカル変換行列計算
void SkinMesh_Renderer::CalculateLocalTransform()
{
	for (Node& node : nodes)
	{
		Matrix scale, rotation, position;
		scale = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		rotation = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotation.x, node.rotation.y, node.rotation.z, node.rotation.w));
		position = DirectX::XMMatrixTranslation(node.position.x, node.position.y, node.position.z);

		DirectX::XMStoreFloat4x4(&node.localTransform, scale * rotation * position);
	}
}

// ワールド変換行列計算
void SkinMesh_Renderer::CalculateWorldTransform(const Matrix& world_transform)
{
	for (Node& node : nodes)
	{
		Matrix localTransform = DirectX::XMLoadFloat4x4(&node.localTransform);
		if (node.parent != nullptr)
		{
			Matrix parentTransform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, localTransform * parentTransform);
		}
		else
		{
			DirectX::XMStoreFloat4x4(&node.worldTransform, localTransform * world_transform);
		}
	}
}

void SkinMesh_Renderer::Reset()
{
	mesh_data = nullptr;
	nodes.clear();
	material.clear();
}

bool SkinMesh_Renderer::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("SkinMesh_Renderer"))
	{
		bool removed = true;
		if (ImGui::BeginPopupContextItem("SkinMesh_Renderer_sub"))
		{
			if (ImGui::Selectable(u8"コンポーネントを削除"))
			{
				Object::Destroy(dynamic_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
				removed = false;
			}
			ImGui::EndPopup();
		}
		if (!removed)
		{
			return false;
		}

		ImGui::Text(u8"現在のメッシュ::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		ImGui::SameLine();
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
				//Debug::Log(pathname);
				//Debug::Log(extname);
				//Debug::Log(filename);
				Reset();
				Set_Mesh(Mesh::Load_Mesh(pathname.c_str(), filename.c_str()));
			}
			else
			{
				Debug::Log("ファイルを開けませんでした");
			}
		}

		static int ID = 0;
		if (mesh_data)
		{
			for (size_t i = 0; i < material.size(); i++)
			{
				ImGui::PushID(ID);
				material[i]->Draw_ImGui();
				ID++;
				ImGui::PopID();
			}
		}
		ID = 0;
	}
	return true;
}