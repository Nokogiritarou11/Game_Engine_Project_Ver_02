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
#include "Include_ImGui.h"
#include "System_Function.h"
#include "Engine.h"
#include "Asset_Manager.h"
#include "Debug.h"
#include "Material.h"
#include "Texture.h"
using namespace std;
using namespace BeastEngine;

void FBX_Converter::Draw_ImGui()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"FBXコンバート", NULL, window_flags);

	if (load_state == 0)
	{
		if (ImGui::Button(u8"FBX読み込み"))
		{
			string path = System_Function::Get_Open_File_Name("fbx", "\\Resouces\\Model");
			if (path != "")
			{
				int path_i = path.find_last_of("\\") + 1;//7
				int ext_i = path.find_last_of(".");//10
				const string pathname = path.substr(0, path_i); //ファイルまでのディレクトリ
				const string extname = path.substr(ext_i, path.size() - ext_i); //拡張子
				const string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				if (extname == ".fbx")
				{
					model = Model_Data::Load_Model(pathname, filename);
					convert_mesh = true;
					convert_material = true;
					convert_animation = true;
					convert_prefab = true;
					++load_state;
				}
				else
				{
					Debug::Log(u8"ファイル形式が対応していません");
				}
			}
		}
	}
	else if (load_state == 1)
	{
		ImGui::Text(model->name.c_str());
		ImGui::Checkbox(u8"メッシュデータに変換する", &convert_mesh);
		ImGui::Checkbox(u8"マテリアルに変換する", &convert_material);
		if (!model->bones.empty())
		{
			ImGui::Checkbox(u8"アニメーションデータに変換する", &convert_animation);
		}
		else
		{
			convert_animation = false;
		}
		ImGui::Checkbox(u8"プレハブデータに変換する", &convert_prefab);

		if (convert_mesh || convert_material || convert_animation || convert_prefab)
		{
			if (ImGui::Button(u8"変換開始"))
			{
				Load_From_FBX(convert_mesh, convert_material, convert_animation, convert_prefab);
				Debug::Log(u8"FBXの変換が完了しました");
				model.reset();
				convert_mesh = false;
				convert_material = false;
				convert_animation = false;
				convert_prefab = false;
				load_state = 0;
			}
		}
		else
		{
			if (ImGui::Button(u8"キャンセル"))
			{
				model.reset();
				convert_mesh = false;
				convert_material = false;
				convert_animation = false;
				convert_prefab = false;
				load_state = 0;
			}
		}
	}

	ImGui::End();
}

void FBX_Converter::Load_From_FBX(bool& convert_mesh, bool& convert_material, bool& convert_animation, bool& convert_prefab)
{
	shared_ptr<GameObject> obj = Create_GameObject(model->name);

	if (model->bones.empty())
	{
		vector<shared_ptr<GameObject>> rend_list;
		for (auto& mesh : model->meshes)
		{
			shared_ptr<GameObject> g = Create_GameObject(mesh->name);
			g->transform->Set_Parent(obj->transform);
			shared_ptr<Mesh_Renderer> renderer = g->Add_Component<Mesh_Renderer>();
			renderer->file_path = model->file_path + mesh->name + ".mesh";

			Vector3 translate;
			Quaternion rotation;
			Vector3 scale;
			mesh->inverse_matrixes[0].Decompose(scale, rotation, translate);
			g->transform->Set_Position(translate);
			g->transform->Set_Rotation(rotation);
			g->transform->Set_Scale(scale);

			rend_list.push_back(g);
		}

		obj->transform->Set_Scale(0.01f, 0.01f, 0.01f);

		if (convert_mesh)
		{
			Convert_Mesh();
		}

		if (convert_prefab)
		{
			Resources::Create_Prefab(obj);
		}

		if (convert_material)
		{
			for (auto& path : model->default_material_pathes)
			{
				int path_i = path.find_last_of("\\") + 1;
				int ext_i = path.find_last_of(".");
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				shared_ptr<Material> mat = Material::Create("Shader\\Standard_Shader_VS.hlsl", "Shader\\Standard_Shader_PS.hlsl");
				mat->name = filename;
				mat->Set_Texture("diffuseMap", Texture::Load("Default_Resource\\Image\\Default_Texture.png"));
				mat->Set_Texture("normalMap", Texture::Load("Default_Resource\\Image\\Default_NormalMap.png"));
				mat->Save(path);
			}
		}

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
		for (auto& mesh : model->meshes)
		{
			shared_ptr<GameObject> g = Create_GameObject(mesh->name);
			g->transform->Set_Parent(obj->transform);
			shared_ptr<SkinMesh_Renderer> renderer = g->Add_Component<SkinMesh_Renderer>();
			renderer->file_path = model->file_path + mesh->name + ".mesh";
			rend_list.push_back(g);
		}

		shared_ptr<GameObject> root = Create_GameObject("RootBone");
		root->transform->Set_Parent(obj->transform);

		vector<shared_ptr<GameObject>> bone_list;
		const vector<Model_Data::Skeleton>& bones = model->bones;

		for (size_t i = 0; i < bones.size(); ++i)
		{
			shared_ptr<GameObject> bone = Create_GameObject(bones[i].name);
			bone_list.emplace_back(bone);
			bone->transform->Set_Parent(root->transform);
		}

		for (size_t i = 0; i < bones.size(); ++i)
		{
			if (bones[i].parentIndex != -1)
			{
				bone_list[i]->transform->Set_Parent(bone_list[bones[i].parentIndex]->transform);
			}
		}

		vector<Matrix> bone_matrixes;
		bone_matrixes.resize(model->bones.size());

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

		if (convert_animation)
		{
			Convert_Animation(bone_list);
		}

		if (convert_mesh)
		{
			Convert_Mesh();
		}

		if (convert_prefab)
		{
			Resources::Create_Prefab(obj);
		}

		if (convert_material)
		{
			for (auto& path : model->default_material_pathes)
			{
				int path_i = path.find_last_of("\\") + 1;
				int ext_i = path.find_last_of(".");
				string filename = path.substr(path_i, ext_i - path_i); //ファイル名
				shared_ptr<Material> mat = Material::Create("Shader\\Standard_Shader_VS.hlsl", "Shader\\Standard_Shader_PS.hlsl");
				mat->name = filename;
				mat->Set_Texture("diffuseMap", Texture::Load("Default_Resource\\Image\\Default_Texture.png"));
				mat->Set_Texture("normalMap", Texture::Load("Default_Resource\\Image\\Default_NormalMap.png"));
				mat->Save(path);
			}
		}

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

void FBX_Converter::Bone_Decompose(vector<Matrix>& matrixes, vector<shared_ptr<GameObject>>& bones, shared_ptr<Transform>& trans)
{
	{
		size_t i = 0;
		for (; i < bones.size(); ++i)
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

void FBX_Converter::Convert_Animation(vector<shared_ptr<GameObject>>& bones)
{
	for (size_t i = 0; i < model->animations.size(); ++i)
	{
		Model_Data::Animation& animation = model->animations[i];

		shared_ptr<Animation_Clip> clip = make_shared<Animation_Clip>();
		clip->name = animation.name;
		clip->length = animation.secondsLength;
		Engine::asset_manager->Registration_Asset(clip);

		clip->animations.resize(model->bones.size());

		for (size_t t = 0; t < clip->animations.size(); ++t)
		{
			Animation_Clip::Animation& anim = clip->animations[t];

			//アニメーション対象のボーン名を登録する
			string path;
			if (model->bones[t].parentIndex == -1)
			{
				path = "RootBone/" + model->bones[t].name;
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

void FBX_Converter::Convert_Mesh()
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
	Engine::asset_manager->Registration_Asset(obj);

	return obj;
}