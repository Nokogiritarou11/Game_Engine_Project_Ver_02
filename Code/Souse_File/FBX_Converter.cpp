#include "FBX_Converter.h"
#include "GameObject.h"
#include "Resources.h"
using namespace std;

void FBX_Converter::Convert_From_FBX(const char* file_path, const char* fbx_filename)
{
	shared_ptr<Mesh> mesh_data = Mesh::Load_Mesh(file_path, fbx_filename);

	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->AddComponent<Transform>();
	obj->name = fbx_filename;

	if (mesh_data->nodes.empty())
	{
		shared_ptr<Mesh_Renderer> renderer = obj->AddComponent<Mesh_Renderer>();
		renderer->file_name = mesh_data->name;
		renderer->file_path = mesh_data->file_path;

		Resources::Create_Prefab(obj);

		renderer->transform.reset();
		renderer->gameObject.reset();
		obj->transform->gameObject.reset();
		obj->transform->transform.reset();
		obj->transform.reset();
		obj->Component_List.clear();
		obj.reset();
	}
	else
	{
		shared_ptr<SkinMesh_Renderer> renderer = obj->AddComponent<SkinMesh_Renderer>();
		renderer->file_name = mesh_data->name;
		renderer->file_path = mesh_data->file_path;

		shared_ptr<GameObject> root = make_shared<GameObject>();
		root->AddComponent<Transform>();
		root->name = "RootBone";
		root->transform->Set_parent(obj->transform);
		//root->transform->Set_localScale(1.0f, 1.0f, -1.0f);

		vector<shared_ptr<GameObject>> bone_list;
		const std::vector<Mesh::Node>& res_nodes = mesh_data->nodes;

		for (size_t i = 0; i < res_nodes.size(); ++i)
		{
			shared_ptr<GameObject> bone = make_shared<GameObject>();
			bone->AddComponent<Transform>();
			bone->transform->Set_localPosition(res_nodes[i].position);
			bone->transform->Set_localRotation(res_nodes[i].rotation);
			bone->transform->Set_localScale(res_nodes[i].scale);
			bone->name = res_nodes[i].name;
			bone_list.emplace_back(bone);
			renderer->bones.emplace_back(bone->transform);
			bone->transform->Set_parent(root->transform);
		}

		for (size_t i = 0; i < res_nodes.size(); ++i)
		{
			if (res_nodes[i].parentIndex != -1)
			{
				renderer->bones[i].lock()->Set_parent(renderer->bones[res_nodes[i].parentIndex].lock());
			}
		}

		Resources::Create_Prefab(obj);

		for (size_t i = 0; i < bone_list.size(); ++i)
		{
			bone_list[i]->transform->gameObject.reset();
			bone_list[i]->transform->transform.reset();
			bone_list[i]->transform.reset();
			bone_list[i]->Component_List.clear();
		}
		bone_list.clear();

		renderer->transform.reset();
		renderer->gameObject.reset();

		root->transform->gameObject.reset();
		root->transform->transform.reset();
		root->transform.reset();
		root->Component_List.clear();
		root.reset();

		obj->transform->gameObject.reset();
		obj->transform->transform.reset();
		obj->transform.reset();
		obj->Component_List.clear();
		obj.reset();
	}
}