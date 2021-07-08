#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include "FBX_Converter.h"
#include "GameObject.h"
#include "Resources.h"
#include "Model_Data.h"
#include "Mesh_Renderer.h"
#include "SkinMesh_Renderer.h"
#include "Animation_Clip.h"
#include "Animator.h"
using namespace std;
using namespace BeastEngine;

void FBX_Converter::Convert_From_FBX(const char* file_path, const char* fbx_filename)
{
	shared_ptr<Model_Data> model = Model_Data::Load_Model(file_path, fbx_filename);

	shared_ptr<GameObject> obj = Create_GameObject(fbx_filename);

	if (model->nodes.empty())
	{
		vector<shared_ptr<GameObject>> rend_list;
		for (size_t i = 0; i < model->meshes.size(); ++i)
		{
			shared_ptr<GameObject> g = Create_GameObject(model->meshes[i]->name);
			g->transform->Set_Parent(obj->transform);
			shared_ptr<Mesh_Renderer> renderer = g->Add_Component<Mesh_Renderer>();
			renderer->file_name = model->meshes[i]->name;
			renderer->file_path = model->file_path;
			rend_list.push_back(g);
		}

		Convert_Mesh(model);
		Resources::Create_Prefab(obj);

		for each (shared_ptr<GameObject> g in rend_list)
		{
			g->transform->gameobject.reset();
			g->transform->transform.reset();
			g->transform.reset();
			g->component_list.clear();
		}
		rend_list.clear();

		obj->transform->gameobject.reset();
		obj->transform->transform.reset();
		obj->transform.reset();
		obj->component_list.clear();
		obj.reset();
	}
	else
	{
		obj->Add_Component<Animator>();

		vector<shared_ptr<GameObject>> rend_list;
		for (size_t i = 0; i < model->meshes.size(); ++i)
		{
			shared_ptr<GameObject> g = Create_GameObject(model->meshes[i]->name);
			g->transform->Set_Parent(obj->transform);
			shared_ptr<SkinMesh_Renderer> renderer = g->Add_Component<SkinMesh_Renderer>();
			renderer->file_name = model->meshes[i]->name;
			renderer->file_path = model->file_path;
			rend_list.push_back(g);
		}

		shared_ptr<GameObject> root = Create_GameObject("RootBone");
		root->transform->Set_Parent(obj->transform);

		vector<shared_ptr<GameObject>> bone_list;
		const vector<Model_Data::Skeleton>& res_nodes = model->nodes;

		for (size_t i = 0; i < res_nodes.size(); ++i)
		{
			shared_ptr<GameObject> bone = Create_GameObject(res_nodes[i].name);
			bone_list.emplace_back(bone);
			bone->transform->Set_Parent(root->transform);
		}

		for (size_t i = 0; i < res_nodes.size(); ++i)
		{
			if (res_nodes[i].parentIndex != -1)
			{
				bone_list[i]->transform->Set_Parent(bone_list[res_nodes[i].parentIndex]->transform);
			}
		}

		vector<Matrix> bone_matrixes;
		bone_matrixes.resize(model->nodes.size());

		for (size_t i = 0; i < rend_list.size(); ++i)
		{
			shared_ptr<SkinMesh_Renderer> rend = rend_list[i]->Get_Component<SkinMesh_Renderer>();
			shared_ptr<Mesh>& mesh = model->meshes[i];

			for (size_t t = 0; t < mesh->nodeIndices.size(); ++t)
			{
				rend->bones.push_back(bone_list[mesh->nodeIndices[t]]->transform);
				bone_matrixes[mesh->nodeIndices[t]] = mesh->inverse_matrixes[t].Invert();
			}
		}

		for (int i = 0; i < root->transform->Get_Child_Count(); ++i)
		{
			Bone_Decompose(bone_matrixes, bone_list, root->transform->Get_Child(i).lock());
		}

		obj->transform->Set_Scale(0.01f, 0.01f, 0.01f);
		Convert_Animation(model, bone_list);
		Convert_Mesh(model);

		Resources::Create_Prefab(obj);

		for each (shared_ptr<GameObject> g in rend_list)
		{
			g->transform->gameobject.reset();
			g->transform->transform.reset();
			g->transform.reset();
			g->component_list.clear();
		}
		rend_list.clear();

		for each (shared_ptr<GameObject> g in bone_list)
		{
			g->transform->gameobject.reset();
			g->transform->transform.reset();
			g->transform.reset();
			g->component_list.clear();
		}
		bone_list.clear();

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

void FBX_Converter::Bone_Decompose(vector<Matrix>& matrixes, vector<shared_ptr<GameObject>>& bones, shared_ptr<Transform> trans)
{
	{
		size_t i = 0;
		for (;i < bones.size(); ++i)
		{
			if (bones[i]->transform == trans) break;
		}
		Matrix m = matrixes[i];
		Vector3 translate;
		Quaternion rotation;
		Vector3 scale;
		m.Decompose(scale, rotation, translate);
		trans->Set_Position(translate);
		trans->Set_Rotation(rotation);
		trans->Set_Scale(scale);
	}

	for (int i = 0; i < trans->Get_Child_Count(); ++i)
	{
		Bone_Decompose(matrixes, bones, trans->Get_Child(i).lock());
	}
}

void FBX_Converter::Convert_Animation(shared_ptr<Model_Data> model, vector<shared_ptr<GameObject>>& bones)
{
	for (size_t i = 0; i < model->animations.size(); ++i)
	{
		Model_Data::Animation& animation = model->animations[i];

		shared_ptr<Animation_Clip> clip = make_shared<Animation_Clip>();
		clip->name = animation.name;
		clip->length = animation.secondsLength;

		clip->animations.resize(model->nodes.size());

		for (size_t t = 0; t < clip->animations.size(); ++t)
		{
			Animation_Clip::Animation& anim = clip->animations[t];

			//アニメーション対象のボーン名を登録する
			string path;
			if (model->nodes[t].parentIndex == -1)
			{
				path = "RootBone/" + model->nodes[t].name;
			}
			else
			{
				path = bones[t]->name;
				weak_ptr<Transform> trans = bones[t]->transform->Get_Parent();
				shared_ptr<Transform> target;
				while (target = trans.lock())
				{
					path = target->gameobject->name + "/" + path;
					if (target->gameobject->name != "RootBone")
					{
						trans = target->Get_Parent();
					}
					else
					{
						break;
					}
				}
			}
			anim.Target_Path = path;

			//対象ボーンのキーを登録する
			anim.keys.resize(animation.keyframes.size());
			for (size_t l = 0; l < anim.keys.size(); ++l)
			{
				anim.keys[l].time = animation.keyframes[l].seconds;
				anim.keys[l].position = animation.keyframes[l].nodeKeys[t].position;
				anim.keys[l].rotation = animation.keyframes[l].nodeKeys[t].rotation;
				anim.keys[l].scale = animation.keyframes[l].nodeKeys[t].scale;
			}
		}

		string save_path = model->file_path + animation.name + ".anim";
		{
			ofstream ss(save_path.c_str(), ios::binary);
			{
				cereal::BinaryOutputArchive o_archive(ss);
				o_archive(clip);
			}
		}
	}
}

void FBX_Converter::Convert_Mesh(std::shared_ptr<BeastEngine::Model_Data> model)
{
	for each (shared_ptr<Mesh> mesh in model->meshes)
	{
		string save_path = model->file_path + mesh->name + ".mesh";
		{
			ofstream ss(save_path.c_str(), ios::binary);
			{
				cereal::BinaryOutputArchive o_archive(ss);
				o_archive(mesh);
			}
		}
	}
}

shared_ptr<GameObject> FBX_Converter::Create_GameObject(string n)
{
	shared_ptr<GameObject> obj = make_shared<GameObject>();
	obj->Add_Component<Transform>();
	obj->name = n;

	return obj;
}