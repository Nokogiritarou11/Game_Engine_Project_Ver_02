#include <fstream>
#include "Fbx_Converter.h"
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

void Fbx_Converter::Draw_ImGui()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	ImGui::Begin(u8"FBX�R���o�[�g", nullptr, window_flags);

	if (load_state == 0)
	{
		if (ImGui::Button(u8"FBX�ǂݍ���"))
		{
			if (const string path = System_Function::Get_Open_File_Name("fbx", "\\Assets\\Model"); !path.empty())
			{
				const int path_i = path.find_last_of("\\") + 1;
				const int ext_i = path.find_last_of(".");
				const string path_name = path.substr(0, path_i); //�t�@�C���܂ł̃f�B���N�g��
				const string ext_name = path.substr(ext_i, path.size() - ext_i); //�g���q
				const string file_name = path.substr(path_i, ext_i - path_i); //�t�@�C����
				if (ext_name == ".fbx")
				{
					model = Model_Data::Load_Model(path_name, file_name);
					convert_mesh = true;
					convert_material = true;
					convert_animation = true;
					convert_prefab = true;
					++load_state;
				}
				else
				{
					Debug::Log(u8"�t�@�C���`�����Ή����Ă��܂���");
				}
			}
		}
	}
	else if (load_state == 1)
	{
		ImGui::Text(model->name.c_str());
		ImGui::Checkbox(u8"���b�V���f�[�^�ɕϊ�����", &convert_mesh);
		ImGui::Checkbox(u8"�}�e���A���ɕϊ�����", &convert_material);
		if (!model->bones.empty())
		{
			ImGui::Checkbox(u8"�A�j���[�V�����f�[�^�ɕϊ�����", &convert_animation);
		}
		else
		{
			convert_animation = false;
		}
		ImGui::Checkbox(u8"�v���n�u�f�[�^�ɕϊ�����", &convert_prefab);

		if (convert_mesh || convert_material || convert_animation || convert_prefab)
		{
			if (ImGui::Button(u8"�ϊ��J�n"))
			{
				Load_From_Fbx(convert_mesh, convert_material, convert_animation, convert_prefab);
				Debug::Log(u8"FBX�̕ϊ����������܂���");
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
			if (ImGui::Button(u8"�L�����Z��"))
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

void Fbx_Converter::Load_From_Fbx(bool& convert_mesh, bool& convert_material, bool& convert_animation, bool& convert_prefab) const
{
	shared_ptr<GameObject> obj = Create_GameObject(model->name);

	if (model->bones.empty())
	{
		//�{�[���̂Ȃ��ꍇ

		//���b�V����`�悷�邽�߂�MeshRenderer���Z�b�g�A�b�v����
		vector<shared_ptr<GameObject>> rend_list;
		for (auto& mesh : model->meshes)
		{
			shared_ptr<GameObject> g = Create_GameObject(mesh->name);
			g->transform->Set_Parent(obj->transform);
			shared_ptr<Mesh_Renderer> renderer = g->Add_Component<Mesh_Renderer>();
			renderer->file_path = model->file_path + mesh->name + ".mesh";

			//���b�V���̋t�s�񂩂珉���p�����Z�o����
			Vector3 translate;
			Quaternion rotation;
			Vector3 scale;
			mesh->inverse_matrixes[0].Decompose(scale, rotation, translate);
			g->transform->Set_Position(translate);
			g->transform->Set_Rotation(rotation);
			g->transform->Set_Scale(scale);

			rend_list.push_back(g);
		}

		//�ǂݍ��ނ�cm�P�ʂŏo�Ă��܂��̂ŃX�P�[���𒲐�
		obj->transform->Set_Scale(0.01f, 0.01f, 0.01f);

		//���b�V���������o��
		if (convert_mesh)
		{
			Convert_Mesh();
		}

		//�v���n�u�������o��
		if (convert_prefab)
		{
			Resources::Create_Prefab(obj);
		}

		//�}�e���A���������o��
		if (convert_material)
		{
			for (auto& path : model->default_material_paths)
			{
				int path_i = path.find_last_of("\\") + 1;
				int ext_i = path.find_last_of(".");
				string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
				shared_ptr<Material> mat = Material::Create("Shader\\Standard_Shader_VS.hlsl", "Shader\\Standard_Shader_PS.hlsl");
				mat->name = filename;
				mat->Set_Texture("diffuseMap", Texture::Load("Default_Assets\\Image\\Default_Texture.png"));
				mat->Set_Texture("normalMap", Texture::Load("Default_Assets\\Image\\Default_NormalMap.png"));
				mat->Save(path);
			}
		}

		//�����������̂��폜����
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
		//�{�[�������ꍇ(�A�j���[�V�����܂�)

		obj->Add_Component<Animator>();

		//���b�V����`�悷�邽�߂�SkinMeshRenderer���Z�b�g�A�b�v����
		vector<shared_ptr<GameObject>> rend_list;
		for (auto& mesh : model->meshes)
		{
			shared_ptr<GameObject> g = Create_GameObject(mesh->name);
			g->transform->Set_Parent(obj->transform);
			shared_ptr<SkinMesh_Renderer> renderer = g->Add_Component<SkinMesh_Renderer>();
			renderer->file_path = model->file_path + mesh->name + ".mesh";
			rend_list.push_back(g);
		}

		//�{�[���p�̃_�~�[�e�I�u�W�F�N�g�𐶐�
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

		//�{�[���̐e�q�֌W��ݒ�
		for (size_t i = 0; i < bones.size(); ++i)
		{
			if (bones[i].parent_index != -1)
			{
				bone_list[i]->transform->Set_Parent(bone_list[bones[i].parent_index]->transform);
			}
		}

		//�{�[���t�s���ݒ�
		vector<Matrix> bone_matrixes;
		bone_matrixes.resize(model->bones.size());

		for (size_t i = 0; i < rend_list.size(); ++i)
		{
			shared_ptr<SkinMesh_Renderer> rend = rend_list[i]->Get_Component<SkinMesh_Renderer>();
			shared_ptr<Mesh>& mesh = model->meshes[i];

			for (size_t t = 0; t < mesh->node_indices.size(); ++t)
			{
				rend->bones.push_back(bone_list[mesh->node_indices[t]]->transform);
				bone_matrixes[mesh->node_indices[t]] = mesh->inverse_matrixes[t].Invert();
			}
		}

		for (int i = 0; i < root->transform->Get_Child_Count(); ++i)
		{
			Bone_Decompose(bone_matrixes, bone_list, root->transform->Get_Child(i).lock());
		}

		//�ǂݍ��ނ�cm�P�ʂŏo�Ă��܂��̂ŃX�P�[���𒲐�
		obj->transform->Set_Scale(0.01f, 0.01f, 0.01f);

		//�A�j���[�V�����������o��
		if (convert_animation)
		{
			Convert_Animation(bone_list);
		}

		//���b�V���������o��
		if (convert_mesh)
		{
			Convert_Mesh();
		}

		//�v���n�u�������o��
		if (convert_prefab)
		{
			Resources::Create_Prefab(obj);
		}

		//�}�e���A���������o��
		if (convert_material)
		{
			for (auto& path : model->default_material_paths)
			{
				int path_i = path.find_last_of("\\") + 1;
				int ext_i = path.find_last_of(".");
				string filename = path.substr(path_i, ext_i - path_i); //�t�@�C����
				shared_ptr<Material> mat = Material::Create("Shader\\Standard_Shader_VS.hlsl", "Shader\\Standard_Shader_PS.hlsl");
				mat->name = filename;
				mat->Set_Texture("diffuseMap", Texture::Load("Default_Assets\\Image\\Default_Texture.png"));
				mat->Set_Texture("normalMap", Texture::Load("Default_Assets\\Image\\Default_NormalMap.png"));
				mat->Save(path);
			}
		}

		//�����������̂��폜����
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

void Fbx_Converter::Bone_Decompose(vector<Matrix>& matrixes, vector<shared_ptr<GameObject>>& bones, const shared_ptr<Transform>& trans)
{
	{
		//�t�s��̃��X�g���玩�g�̎p�����Z�o
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

	//�q�ɑ΂��čċA
	for (int i = 0; i < trans->Get_Child_Count(); ++i)
	{
		Bone_Decompose(matrixes, bones, trans->Get_Child(i).lock());
	}
}

void Fbx_Converter::Convert_Animation(const vector<shared_ptr<GameObject>>& bones) const
{
	for (size_t i = 0; i < model->animations.size(); ++i)
	{
		auto& animation = model->animations[i];

		auto clip = make_shared<Animation_Clip>();
		clip->name = animation.name;
		clip->length = animation.seconds_length;
		clip->frame_count = animation.keyframes.size() - 1;
		Engine::asset_manager->Registration_Asset(clip);

		clip->animations.resize(model->bones.size());

		for (size_t t = 0; t < clip->animations.size(); ++t)
		{
			auto& anim = clip->animations[t];

			//�A�j���[�V�����Ώۂ̃{�[������o�^����
			string path;
			if (model->bones[t].parent_index == -1)
			{
				path = "RootBone/" + model->bones[t].name;
			}
			else
			{
				path = bones[t]->name;
				auto trans = bones[t]->transform->Get_Parent();
				while (const auto& target = trans.lock())
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
			anim.target_path = path;

			//�Ώۃ{�[���̃L�[��o�^����
			anim.keys.resize(animation.keyframes.size());
			for (size_t l = 0; l < anim.keys.size(); ++l)
			{
				anim.keys[l].time = animation.keyframes[l].seconds;
				anim.keys[l].position = animation.keyframes[l].node_keys[t].position;
				anim.keys[l].rotation = animation.keyframes[l].node_keys[t].rotation;
				anim.keys[l].scale = animation.keyframes[l].node_keys[t].scale;
			}
		}

		//�����o��
		string save_path = model->file_path + animation.name + ".anim";
		{
			ofstream ss(save_path.c_str(), ios::binary);
			cereal::BinaryOutputArchive o_archive(ss);
			o_archive(clip);
		}
	}
}

void Fbx_Converter::Convert_Mesh() const
{
	for (const auto& mesh : model->meshes)
	{
		//�����o��
		string save_path = model->file_path + mesh->name + ".mesh";
		{
			ofstream ss(save_path.c_str(), ios::binary);
			cereal::BinaryOutputArchive o_archive(ss);
			o_archive(mesh);
		}
	}
}

shared_ptr<GameObject> Fbx_Converter::Create_GameObject(const string& n) const
{
	//�R���o�[�^�[�����݂̂Ŏg�p����I�u�W�F�N�g�𐶐�����
	auto obj = make_shared<GameObject>();
	obj->Add_Component<Transform>();
	obj->name = n;
	Engine::asset_manager->Registration_Asset(obj);

	return obj;
}