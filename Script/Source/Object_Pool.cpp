#include "Object_Pool.h"

using namespace std;
using namespace BeastEngine;

void Object_Pool::Start()
{
	//プーリング対象のオブジェクトを生成して保持する
	for (const auto& obj : pool_list)
	{
		const vector<weak_ptr<GameObject>> add_list;
		instance_pool[obj.instance_key] = add_list;

		//キーでプーリングするオブジェクトを登録する
		auto& obj_list = instance_pool[obj.instance_key];
		for (int i = 0; i < obj.instance_counts; ++i)
		{
			const auto& add_obj = Resources::Load_Prefab(obj.obj_path);
			add_obj->Set_Active(false);
			obj_list.emplace_back(add_obj);
		}
	}
}

shared_ptr<GameObject> Object_Pool::Instance_In_Pool(const string& key, Vector3 position, Quaternion rotation)
{
	//プールからオブジェクトを取得する
	const auto& obj = Get_Object(key);
	//姿勢を更新してアクティブ化
	obj->transform->Set_Local_Position(position);
	obj->transform->Set_Local_Rotation(rotation);
	obj->Set_Active(true);
	return obj;
}

shared_ptr<GameObject> Object_Pool::Get_Object(const string& key)
{
	//プールから非アクティブなものを検索
	auto& obj_list = instance_pool[key];
	for (const auto& obj : obj_list)
	{
		const auto& o = obj.lock();
		if (!o->Get_Active())
		{
			return o;
		}
	}

	//非アクティブなものがなかった場合、新規生成、プールに追加してから返す
	for (const auto& add_data : pool_list)
	{
		if (add_data.instance_key == key)
		{
			const auto& add_obj = Resources::Load_Prefab(add_data.obj_path);
			add_obj->Set_Active(false);
			obj_list.emplace_back(add_obj);

			return add_obj;
		}
	}
	return nullptr;
}

bool Object_Pool::Draw_ImGui()
{
	bool open = false;
	if (!Draw_ImGui_Header("Object_Pool", open)) return false;

	if (open)
	{
		const float window_center = ImGui::GetWindowContentRegionWidth() * 0.5f;

		ImGui::Text(u8"要素数");
		ImGui::SameLine(window_center);
		ImGui::SetNextItemWidth(-FLT_MIN);

		int size = static_cast<int>(pool_list.size());

		if (ImGui::InputInt("##list_count", &size, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			pool_list.resize(size);
		}

		for (int i = 0; i < size; ++i)
		{
			Pool_Object& pool_obj = pool_list[i];

			ImGui::PushID(i);
			string label = u8"要素 " + to_string(i);
			if (ImGui::TreeNode(label.c_str()))
			{
				ImGui::LeftText_InputText(u8"呼び出しキー", "##instance_key", &pool_obj.instance_key, window_center);
				ImGui::LeftText_InputText(u8"プレハブパス", "##obj_path", &pool_obj.obj_path, window_center);
				ImGui::LeftText_InputInt(u8"生成数", "##", &pool_obj.instance_counts, window_center);

				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}
	return true;
}