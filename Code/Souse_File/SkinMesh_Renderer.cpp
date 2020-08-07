#include "SkinMesh_Renderer.h"
#include "GameObject.h"
#include "Transform.h"
#include "Render_Manager.h"
#include "Debug.h"
#include "Include_ImGui.h"
#include "Debug_UI.h"
using namespace std;

ComPtr <ID3D11Buffer> SkinMesh_Renderer::ConstantBuffer_CbMesh;
ComPtr <ID3D11Buffer> SkinMesh_Renderer::ConstantBuffer_CbColor;

void SkinMesh_Renderer::Initialize()
{
	Render_Manager::Add(static_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
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
		assert(SUCCEEDED(hr), hr_trace(hr));
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
		assert(SUCCEEDED(hr), hr_trace(hr));
	}
}
void SkinMesh_Renderer::Initialize(shared_ptr<GameObject> obj)
{
	gameObject = obj;
	transform = obj->transform;
	Render_Manager::Add(static_pointer_cast<SkinMesh_Renderer>(shared_from_this()));
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
		assert(SUCCEEDED(hr), hr_trace(hr));
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
		assert(SUCCEEDED(hr), hr_trace(hr));
	}

	if (file_pass != "")
	{
		Set_Mesh(Mesh::Load_Mesh(file_pass.c_str(), file_name.c_str()));
	}
}

void SkinMesh_Renderer::Set_Mesh(shared_ptr<Mesh> Mesh_Data)
{
	mesh_data = Mesh_Data;
	file_name = mesh_data->name;
	file_pass = mesh_data->file_pass;
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
}

void SkinMesh_Renderer::Render(Matrix V, Matrix P)
{
	UpdateAnimation(Time::deltaTime);
	CalculateLocalTransform();
	CalculateWorldTransform(transform->Get_world_matrix());

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
				DxSystem::DeviceContext->IASetInputLayout(material[subset.diffuse.ID]->shader->VertexLayout.Get());

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
				cbColor.materialColor = material[subset.diffuse.ID]->color;
				DxSystem::DeviceContext->VSSetConstantBuffers(2, 1, ConstantBuffer_CbColor.GetAddressOf());
				DxSystem::DeviceContext->UpdateSubresource(ConstantBuffer_CbColor.Get(), 0, 0, &cbColor, 0, 0);

				//シェーダーリソースのバインド
				material[subset.diffuse.ID]->texture->Set(); //PSSetSamplar PSSetShaderResources
				material[subset.diffuse.ID]->shader->Activate(); //PS,VSSetShader
				// ↑で設定したリソースを利用してポリゴンを描画する。
				DxSystem::DeviceContext->DrawIndexed(subset.index_count, subset.index_start, 0);
			}
		}
	}
}
// アニメーション再生
void SkinMesh_Renderer::PlayAnimation(int animationIndex, bool loop)
{
	currentAnimation = animationIndex;
	loopAnimation = loop;
	endAnimation = false;
	currentSeconds = 0.0f;
}

// アニメーション計算
void SkinMesh_Renderer::UpdateAnimation(float elapsedTime)
{
	if (currentAnimation < 0)
	{
		return;
	}

	if (mesh_data->animations.empty())
	{
		return;
	}

	const Mesh::Animation& animation = mesh_data->animations.at(currentAnimation);

	const std::vector<Mesh::Keyframe>& keyframes = animation.keyframes;
	int keyCount = static_cast<int>(keyframes.size());
	for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
	{
		// 現在の時間がどのキーフレームの間にいるか判定する
		const Mesh::Keyframe& keyframe0 = keyframes.at(keyIndex);
		const Mesh::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);
		if (currentSeconds >= keyframe0.seconds && currentSeconds < keyframe1.seconds)
		{
			float rate = (currentSeconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

			assert(nodes.size() == keyframe0.nodeKeys.size());
			assert(nodes.size() == keyframe1.nodeKeys.size());
			int nodeCount = static_cast<int>(nodes.size());
			for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
			{
				// ２つのキーフレーム間の補完計算
				const Mesh::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
				const Mesh::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

				Node& node = nodes[nodeIndex];

				DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotation);
				DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotation);
				DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.position);
				DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.position);

				DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
				DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
				DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

				DirectX::XMStoreFloat3(&node.scale, s);
				DirectX::XMStoreFloat4(&node.rotation, r);
				DirectX::XMStoreFloat3(&node.position, t);
			}
			break;
		}
	}

	// 最終フレーム処理
	if (endAnimation)
	{
		endAnimation = false;
		currentAnimation = -1;
		return;
	}

	// 時間経過
	currentSeconds += elapsedTime;
	if (currentSeconds >= animation.secondsLength)
	{
		if (loopAnimation)
		{
			currentSeconds -= animation.secondsLength;
		}
		else
		{
			currentSeconds = animation.secondsLength;
			endAnimation = true;
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

void SkinMesh_Renderer::Draw_ImGui()
{
	ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
	if (ImGui::CollapsingHeader("SkinMesh_Renderer"))
	{
		ImGui::Text(u8"現在のメッシュ::");
		ImGui::SameLine();
		ImGui::Text(file_name.c_str());
		ImGui::SameLine();
		if (ImGui::Button(u8"メッシュを選択"))
		{
			string path = Debug_UI::Get_Open_File_Name();
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
			for (auto& mesh : mesh_data->meshes)
			{
				for (auto& subset : mesh.subsets)
				{
					ImGui::PushID(ID);
					material[subset.diffuse.ID]->Draw_ImGui();
					ID++;
					ImGui::PopID();
				}
			}
		}
		ID = 0;
	}
}