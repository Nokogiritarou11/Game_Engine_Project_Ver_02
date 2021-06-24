#include "FBX_Converter.h"
#include "GameObject.h"
#include "Resources.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
using namespace std;
using namespace BeastEngine;

void FBX_Converter::Convert_From_FBX(const char* file_path, const char* fbx_filename)
{
	shared_ptr<Mesh> mesh_data = Mesh::Load_Mesh(file_path, fbx_filename);

	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->Add_Component<Transform>();
	obj->name = fbx_filename;

	if (mesh_data->nodes.empty())
	{
		shared_ptr<Mesh_Renderer> renderer = obj->Add_Component<Mesh_Renderer>();
		renderer->file_name = mesh_data->name;
		renderer->file_path = mesh_data->file_path;

		Resources::Create_Prefab(obj);

		renderer->transform.reset();
		renderer->gameobject.reset();
		obj->transform->gameobject.reset();
		obj->transform->transform.reset();
		obj->transform.reset();
		obj->component_list.clear();
		obj.reset();
	}
	else
	{
		shared_ptr<SkinMesh_Renderer> renderer = obj->Add_Component<SkinMesh_Renderer>();
		renderer->file_name = mesh_data->name;
		renderer->file_path = mesh_data->file_path;

		shared_ptr<GameObject> root = make_shared<GameObject>();
		root->Add_Component<Transform>();
		root->name = "RootBone";
		root->transform->Set_Parent(obj->transform);
		//root->transform->Set_localScale(1.0f, 1.0f, -1.0f);

		vector<shared_ptr<GameObject>> bone_list;
		const std::vector<Mesh::Node>& res_nodes = mesh_data->nodes;

		for (size_t i = 0; i < res_nodes.size(); ++i)
		{
			shared_ptr<GameObject> bone = make_shared<GameObject>();
			bone->Add_Component<Transform>();
			bone->transform->Set_Local_Position(res_nodes[i].position);
			bone->transform->Set_Local_Rotation(res_nodes[i].rotation);
			bone->transform->Set_Local_Scale(res_nodes[i].scale);
			bone->name = res_nodes[i].name;
			bone_list.emplace_back(bone);
			renderer->bones.emplace_back(bone->transform);
			bone->transform->Set_Parent(root->transform);
		}

		for (size_t i = 0; i < res_nodes.size(); ++i)
		{
			if (res_nodes[i].parentIndex != -1)
			{
				renderer->bones[i].lock()->Set_Parent(renderer->bones[res_nodes[i].parentIndex].lock());
			}
		}

		obj->transform->Set_Scale(0.01f, 0.01f, 0.01f);

		Resources::Create_Prefab(obj);

		for (size_t i = 0; i < bone_list.size(); ++i)
		{
			bone_list[i]->transform->gameobject.reset();
			bone_list[i]->transform->transform.reset();
			bone_list[i]->transform.reset();
			bone_list[i]->component_list.clear();
		}
		bone_list.clear();

		renderer->transform.reset();
		renderer->gameobject.reset();

		root->transform->gameobject.reset();
		root->transform->transform.reset();
		root->transform.reset();
		root->component_list.clear();
		root.reset();

		obj->transform->gameobject.reset();
		obj->transform->transform.reset();
		obj->transform.reset();
		obj->component_list.clear();
		obj.reset();
	}
}