#include "Sprite_Renderer.h"
#include "DxSystem.h"
#include "GameObject.h"
#include "Engine.h"
#include "Render_Texture.h"
#include "Render_Manager.h"
#include "Include_ImGui.h"
#include "Shader.h"
#include "Texture.h"
#include "System_Function.h"
#include "Asset_Manager.h"
using namespace std;
using namespace DirectX;
using namespace BeastEngine;

void Sprite_Renderer::Initialize(const shared_ptr<GameObject>& obj)
{
	enabled_old = enabled;

	//マネージャーへの登録とComponentの初期化
	gameobject = obj;
	Engine::asset_manager->Registration_Asset(shared_from_this());
	transform = obj->transform;

	constexpr Vertex v[] = {
		Vector3(-0.5f, 0.5f,0), Vector2(0,0), Vector4(1,1,1,1), //左上
		Vector3(0.5f, 0.5f,0),  Vector2(1,0), Vector4(1,1,1,1), //右上
		Vector3(-0.5f,-0.5f,0), Vector2(0,1), Vector4(1,1,1,1), //左下
		Vector3(0.5f,-0.5f,0),  Vector2(1,1), Vector4(1,1,1,1), //右下
	};

	//	頂点バッファ作成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(v);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA res;
	ZeroMemory(&res, sizeof(res));
	res.pSysMem = v;

	DxSystem::device->CreateBuffer(&bd, &res, vertex_buffer.GetAddressOf());

	//テクスチャ読み込み
	if (file_path.empty())
	{
		//デフォルトテクスチャ
		texture = Texture::Load("Default_Assets\\Image\\Default_Texture.png");
	}
	else
	{
		texture = Texture::Load(file_path + file_name);
	}
	can_render = true;

	Set_Active(Get_Enabled());
}

void Sprite_Renderer::Set_Active(const bool value)
{
	if (value)
	{
		if (!is_called)
		{
			if (gameobject->Get_Active_In_Hierarchy())
			{
				if (Get_Enabled())
				{
					//初回のみマネージャーに登録
					Engine::render_manager->Add(static_pointer_cast<Sprite_Renderer>(shared_from_this()));
					is_called = true;
				}
			}
		}
	}
}

void Sprite_Renderer::Recalculate_Frame()
{
	if (can_render)
	{
		//頂点データ設定
		Vertex data[4];

		const Vector3 trans_pos = gameobject->transform->Get_Position();
		const Vector3 trans_scale = gameobject->transform->Get_Scale();
		const Vector2 scaled_size = size * Vector2(trans_scale.x, trans_scale.y);

		data[0].pos.x = trans_pos.x;
		data[0].pos.y = trans_pos.y;
		data[0].pos.z = 0.0f;

		data[1].pos.x = trans_pos.x + scaled_size.x;
		data[1].pos.y = trans_pos.y;
		data[1].pos.z = 0.0f;

		data[2].pos.x = trans_pos.x;
		data[2].pos.y = trans_pos.y + scaled_size.y;
		data[2].pos.z = 0.0f;

		data[3].pos.x = trans_pos.x + scaled_size.x;
		data[3].pos.y = trans_pos.y + scaled_size.y;
		data[3].pos.z = 0.0f;

		// 中心座標を原点へ
		const float mx = trans_pos.x + scaled_size.x * 0.5f;
		const float my = trans_pos.y + scaled_size.y * 0.5f;
		data[0].pos.x -= mx; data[0].pos.y -= my;
		data[1].pos.x -= mx; data[1].pos.y -= my;
		data[2].pos.x -= mx; data[2].pos.y -= my;
		data[3].pos.x -= mx; data[3].pos.y -= my;

		const float z = transform->Get_Euler_Angles().z;
		const float cos = cosf(XMConvertToRadians(z));
		const float sin = sinf(XMConvertToRadians(z));

		float rx = data[0].pos.x;
		float ry = data[0].pos.y;
		data[0].pos.x = cos * rx + -sin * ry;
		data[0].pos.y = sin * rx + cos * ry;

		rx = data[1].pos.x;
		ry = data[1].pos.y;
		data[1].pos.x = cos * rx + -sin * ry;
		data[1].pos.y = sin * rx + cos * ry;

		rx = data[2].pos.x;
		ry = data[2].pos.y;
		data[2].pos.x = cos * rx + -sin * ry;
		data[2].pos.y = sin * rx + cos * ry;

		rx = data[3].pos.x;
		ry = data[3].pos.y;
		data[3].pos.x = cos * rx + -sin * ry;
		data[3].pos.y = sin * rx + cos * ry;

		// 動かした分戻す
		data[0].pos.x += mx; data[0].pos.y += my;
		data[1].pos.x += mx; data[1].pos.y += my;
		data[2].pos.x += mx; data[2].pos.y += my;
		data[3].pos.x += mx; data[3].pos.y += my;

		// 正規化デバイス座標系
		for (auto& i : data)
		{
			i.pos.x = 2.0f * i.pos.x / Engine::render_manager->game_texture->screen_x - 1.0f;
			i.pos.y = 1.0f - 2.0f * i.pos.y / Engine::render_manager->game_texture->screen_y;
			i.pos.z = 0.0f;
		}

		//テクスチャ座標設定
		data[0].tex.x = uv_origin.x;
		data[0].tex.y = uv_origin.y;
		data[1].tex.x = uv_origin.x + uv_size.x;
		data[1].tex.y = uv_origin.y;
		data[2].tex.x = uv_origin.x;
		data[2].tex.y = uv_origin.y + uv_size.y;
		data[3].tex.x = uv_origin.x + uv_size.x;
		data[3].tex.y = uv_origin.y + uv_size.y;

		//UV座標
		const float w = static_cast<float>(texture->Get_Width());
		const float h = static_cast<float>(texture->Get_Height());
		for (auto& i : data)
		{
			i.tex.x = i.tex.x / w;
			i.tex.y = i.tex.y / h;
		}
		//頂点カラー
		data[0].color = color;
		data[1].color = color;
		data[2].color = color;
		data[3].color = color;


		constexpr UINT subresourceIndex = 0;
		D3D11_MAPPED_SUBRESOURCE mapped;
		const auto hr = DxSystem::device_context->Map(vertex_buffer.Get(), subresourceIndex, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		if (SUCCEEDED(hr))
		{
			memcpy(mapped.pData, data, sizeof(data));
			DxSystem::device_context->Unmap(vertex_buffer.Get(), subresourceIndex);
		}
	}
}

void Sprite_Renderer::Render(int subset_number)
{
	if (can_render)
	{
		//	頂点バッファの指定
		constexpr UINT stride = sizeof(Vertex);
		constexpr UINT offset = 0;
		DxSystem::device_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
		//テクスチャの設定
		texture->Set(1, Shader::Shader_Type::Pixel);

		// 描画
		DxSystem::device_context->Draw(4, 0);
	}
}

bool Sprite_Renderer::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Sprite_Renderer", open)) return false;

	if (open)
	{
		const float window_width = ImGui::GetWindowContentRegionWidth();
		{
			const auto& size = ImVec2(100.0f, 100.0f);
			const auto& uv0 = ImVec2(0.0f, 0.0f);
			const auto& uv1 = ImVec2(1.0f, 1.0f);
			const int& frame_padding = 3;
			const auto& bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
			const auto& tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

			ImGui::Text(u8"現在のテクスチャ::");
			ImGui::SameLine();
			ImGui::Text(file_name.c_str());

			ImGui::Dummy({ 0,0 });
			ImGui::SameLine(window_width - size.x - static_cast<float>(frame_padding));
			if (ImGui::ImageButton(static_cast<void*>(texture->Get_Resource().Get()), size, uv0, uv1, frame_padding, bg_col, tint_col))
			{
				if (const string path = System_Function::Get_Open_File_Name("png", "\\Assets\\Image"); path != "")
				{
					const int path_i = path.find_last_of("\\") + 1;
					const int ext_i = path.find_last_of(".");
					const string path_name = path.substr(0, path_i); //ファイルまでのディレクトリ
					const string ext_name = path.substr(ext_i, path.size() - ext_i); //拡張子
					const string filename = path.substr(path_i, ext_i - path_i); //ファイル名
					texture = Texture::Load(path);
					file_path = path_name;
					file_name = filename + ext_name;
				}
			}
		}

		float edit_size[2] = { size.x, size.y };
		int edit_uv_origin[2] = { static_cast<int>(uv_origin.x), static_cast<int>(uv_origin.y) };
		int edit_uv_size[2] = { static_cast<int>(uv_size.x),static_cast<int>(uv_size.y) };

		ImGui::Text(u8"表示サイズ");
		ImGui::SameLine(window_width * 0.4f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragFloat2("##size", edit_size, 0.1f, -FLT_MAX, FLT_MAX))
		{
			size = Vector2(edit_size[0], edit_size[1]);
		}

		ImGui::Text(u8"UV始点");
		ImGui::SameLine(window_width * 0.4f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragInt2("##uv_origin", edit_uv_origin, 0, INT_MAX))
		{
			uv_origin = Vector2(static_cast<float>(edit_uv_origin[0]), static_cast<float>(edit_uv_origin[1]));
		}

		ImGui::Text(u8"UVサイズ");
		ImGui::SameLine(window_width * 0.4f);
		ImGui::SetNextItemWidth(-FLT_MIN);
		if (ImGui::DragInt2("##uv_size", edit_uv_size, 0, INT_MAX))
		{
			uv_size = Vector2(static_cast<float>(edit_uv_size[0]), static_cast<float>(edit_uv_size[1]));
		}

		float edit_color[4] = { color.x,color.y,color.z,color.w };
		if (ImGui::ColorEdit4("Color", edit_color))
		{
			color = Vector4(edit_color[0], edit_color[1], edit_color[2], edit_color[3]);
		}
	}
	return true;
}