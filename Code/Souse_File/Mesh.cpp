#include "Mesh.h"
#include "misc.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "DxSystem.h"
using namespace std;
using namespace fbxsdk;
using namespace DirectX;

shared_ptr<Mesh> Mesh::Load_Mesh(const char* file_pass, const char* fbx_filename)
{
	shared_ptr<Mesh> mesh_ptr = make_shared<Mesh>();
	mesh_ptr->name = (string)fbx_filename;

	static unordered_map<string, vector<mesh>> cache;
	const string fullpass = (string)file_pass + (string)fbx_filename;

	auto it = cache.find(fullpass);
	if (it != cache.end())
	{
		mesh_ptr->skin_meshes = it->second;
		return mesh_ptr;
	}
	else
		//ファイルから読み込み
	{
		vector<mesh> skin_meshes;
		/*
		ifstream in_f(fullpass);
		assert(in_f.is_open(), "File not found");

		string _s;
		getline(in_f, _s);
		int mesh_size = stoi(_s);
		skin_meshes.resize(mesh_size);
		for (int i = 0;i < mesh_size;i++)
		{
			vector<Mesh::vertex> vertices; // Vertex buffer
			vector<u_int> indices; // Index buffer

			getline(in_f, _s);
			int vertex_size = stoi(_s);
			vertices.resize(vertex_size);
			for (int j = 0;j < vertex_size;j++)
			{
				in_f >> vertices[j].position.x >> vertices[j].position.y >> vertices[j].position.z;
				in_f.ignore();

				in_f >> vertices[j].normal.x >> vertices[j].normal.y >> vertices[j].normal.z;
				in_f.ignore();

				in_f >> vertices[j].texcoord.x >> vertices[j].texcoord.y;
				in_f.ignore();

				for (int k = 0;k < MAX_BONE_INFLUENCES;k++)
				{
					if (k < MAX_BONE_INFLUENCES - 1)
					{
						getline(in_f, _s, ' ');
					}
					else
					{
						getline(in_f, _s);
					}
					vertices[j].bone_weights[k] = stof(_s);
				}

				for (int k = 0;k < MAX_BONE_INFLUENCES;k++)
				{
					if (k < MAX_BONE_INFLUENCES - 1)
					{
						getline(in_f, _s, ' ');
					}
					else
					{
						getline(in_f, _s);
					}
					vertices[j].bone_indices[k] = stoi(_s);
				}
			}

			getline(in_f, _s);
			int index_size = stoi(_s);
			indices.resize(index_size);
			for (int j = 0;j < index_size;j++)
			{
				if (j < index_size - 1)
				{
					getline(in_f, _s, ' ');
				}
				else
				{
					getline(in_f, _s);
				}
				indices[j] = stoi(_s);
			}

			getline(in_f, _s);
			int subset_size = stoi(_s);
			skin_meshes[i].subsets.resize(subset_size);
			for (int j = 0;j < subset_size;j++)
			{
				getline(in_f, _s);
				skin_meshes[i].subsets[j].index_start = stoi(_s);

				getline(in_f, _s);
				skin_meshes[i].subsets[j].index_count = stoi(_s);

				in_f >> skin_meshes[i].subsets[j].diffuse.color.x >> skin_meshes[i].subsets[j].diffuse.color.y >> skin_meshes[i].subsets[j].diffuse.color.z >> skin_meshes[i].subsets[j].diffuse.color.w;
				in_f.ignore();

				//シェーダー読んでるところ
				getline(in_f, _s);

				getline(in_f, _s);
				if (_s != "")
				{
					string Texpass = (string)file_pass + _s;
					skin_meshes[i].subsets[j].diffuse.TexPass = Texpass;
					skin_meshes[i].subsets[j].diffuse.TexName = _s;
				}
				else
				{
					string Texpass = "Default_Resource\\Image\\Default_Texture.png";
					skin_meshes[i].subsets[j].diffuse.TexPass = Texpass;
					skin_meshes[i].subsets[j].diffuse.TexName = _s;
				}
			}

			for (int row = 0; row < 4; row++)
			{
				for (int column = 0; column < 4; column++)
				{
					if (row == 3 && column == 3)
					{
						getline(in_f, _s);
					}
					else
					{
						getline(in_f, _s, ' ');
					}
					skin_meshes[i].global_transform.m[row][column] = stof(_s);
				}
			}

			getline(in_f, _s);
			int skeletal_animation_size = stoi(_s);
			skin_meshes[i].skeletal_animation.resize(skeletal_animation_size);
			for (int j = 0;j < skeletal_animation_size;j++)
			{
				getline(in_f, _s);
				int skeletal_size = stoi(_s);
				skin_meshes[i].skeletal_animation[j].bones.resize(skeletal_size);
				getline(in_f, _s);
				skin_meshes[i].skeletal_animation[j].sampling_time = stof(_s);
				for (int k = 0;k < skeletal_size;k++)
				{
					getline(in_f, _s);
					int bone_size = stoi(_s);
					skin_meshes[i].skeletal_animation[j].bones.at(k).resize(bone_size);
					for (int l = 0; l < bone_size; l++)
					{
						for (int row = 0; row < 4; row++)
						{
							for (int column = 0; column < 4; column++)
							{
								if (row == 3 && column == 3)
								{
									getline(in_f, _s);
								}
								else
								{
									getline(in_f, _s, ' ');
								}
								skin_meshes[i].skeletal_animation[j].bones.at(k).at(l).transform.m[row][column] = stof(_s);;
							}
						}
					}
				}
			}

			// 頂点バッファの生成
			{
				D3D11_BUFFER_DESC bd            = {};
				bd.Usage                        = D3D11_USAGE_DEFAULT;
				bd.ByteWidth                    = sizeof(Mesh::vertex) * vertices.size();
				bd.BindFlags                    = D3D11_BIND_VERTEX_BUFFER;
				bd.CPUAccessFlags               = 0;
				bd.MiscFlags                    = 0;
				bd.StructureByteStride          = 0;
				D3D11_SUBRESOURCE_DATA InitData = {};
				InitData.pSysMem                = &vertices[0];				// 頂点のアドレス
				InitData.SysMemPitch            = 0;
				InitData.SysMemSlicePitch       = 0;
				HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, skin_meshes[i].vertex_buffer.GetAddressOf());
				assert(SUCCEEDED(hr), hr_trace(hr));
			}

			// インデックスバッファの生成
			{
				D3D11_BUFFER_DESC bd            = {};
				//bd.Usage                      = D3D11_USAGE_DEFAULT;
				bd.Usage                        = D3D11_USAGE_IMMUTABLE;
				bd.ByteWidth                    = sizeof(u_int) * indices.size();
				bd.BindFlags                    = D3D11_BIND_INDEX_BUFFER;
				bd.CPUAccessFlags               = 0;
				bd.MiscFlags                    = 0;
				bd.StructureByteStride          = 0;
				D3D11_SUBRESOURCE_DATA InitData = {};
				InitData.pSysMem                = &indices[0];				// 頂点のアドレス
				InitData.SysMemPitch            = 0;
				InitData.SysMemSlicePitch       = 0;
				HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, skin_meshes[i].index_buffer.GetAddressOf());
				assert(SUCCEEDED(hr), hr_trace(hr));
			}
		}
		*/

		const string bin = fullpass + ".bin";
		ifstream in_bin(bin, ios::binary);
		if (in_bin.is_open())
		{
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(skin_meshes);

			for (int i = 0; i < skin_meshes.size(); i++)
			{
				// 頂点バッファの生成
				{
					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.ByteWidth = sizeof(Mesh::vertex) * skin_meshes[i].vertices.size();
					bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					D3D11_SUBRESOURCE_DATA InitData = {};
					InitData.pSysMem = &skin_meshes[i].vertices[0];				// 頂点のアドレス
					InitData.SysMemPitch = 0;
					InitData.SysMemSlicePitch = 0;
					HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, skin_meshes[i].vertex_buffer.GetAddressOf());
					assert(SUCCEEDED(hr), hr_trace(hr));
				}

				// インデックスバッファの生成
				{
					D3D11_BUFFER_DESC bd = {};
					//bd.Usage                      = D3D11_USAGE_DEFAULT;
					bd.Usage = D3D11_USAGE_IMMUTABLE;
					bd.ByteWidth = sizeof(u_int) * skin_meshes[i].indices.size();
					bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					D3D11_SUBRESOURCE_DATA InitData = {};
					InitData.pSysMem = &skin_meshes[i].indices[0];				// 頂点のアドレス
					InitData.SysMemPitch = 0;
					InitData.SysMemSlicePitch = 0;
					HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, skin_meshes[i].index_buffer.GetAddressOf());
					assert(SUCCEEDED(hr), hr_trace(hr));
				}

				//使わないので開放
				skin_meshes[i].vertices.clear();
				skin_meshes[i].indices.clear();
			}
		}
		else
		{
			const string fbx = fullpass + ".fbx";

			// Create the FBX SDK manager
			FbxManager* manager = FbxManager::Create();
			// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
			manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));
			// Create an importer.
			FbxImporter* importer = FbxImporter::Create(manager, "");
			// Initialize the importer.
			bool import_status = false;
			import_status = importer->Initialize(fbx.c_str(), -1, manager->GetIOSettings());
			_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());
			// Create a new scene so it can be populated by the imported file.
			FbxScene* scene = FbxScene::Create(manager, "");
			// Import the contents of the file into the scene.
			import_status = importer->Import(scene);
			_ASSERT_EXPR_A(import_status, importer->GetStatus().GetErrorString());
			// Convert mesh, NURBS and patch into triangle mesh
			fbxsdk::FbxGeometryConverter geometry_converter(manager);
			geometry_converter.Triangulate(scene, /*replace*/true);
			// Fetch node attributes and materials under this node recursively. Currently only mesh.
			vector<FbxNode*> fetched_meshes;
			function<void(FbxNode*)> traverse = [&](FbxNode* node) {
				if (node)
				{
					FbxNodeAttribute* fbx_node_attribute = node->GetNodeAttribute();
					if (fbx_node_attribute)
					{
						switch (fbx_node_attribute->GetAttributeType())
						{
							case FbxNodeAttribute::eMesh:
								fetched_meshes.push_back(node);
								break;
						}
					}
					for (int i = 0; i < node->GetChildCount(); i++)
						traverse(node->GetChild(i));
				}
			};
			traverse(scene->GetRootNode());

			//FbxMesh* fbx_mesh = fetched_meshes.at(0)->GetMesh(); // Currently only one mesh.
			skin_meshes.resize(fetched_meshes.size());
			for (size_t i = 0; i < fetched_meshes.size(); i++)
			{
				FbxMesh* fbx_mesh = fetched_meshes.at(i)->GetMesh();
				mesh& mesh = skin_meshes.at(i);
				const int number_of_materials = fbx_mesh->GetNode()->GetMaterialCount();
				mesh.subsets.resize(number_of_materials); // UNIT.18
				u_int vertex_count = 0;

				vector<bone_influences_per_control_point> bone_influences;
				fetch_bone_influences(fbx_mesh, bone_influences);

				FbxAMatrix global_transform = fbx_mesh->GetNode()->EvaluateGlobalTransform(0);
				for (int row = 0; row < 4; row++)
				{
					for (int column = 0; column < 4; column++)
					{
						mesh.global_transform.m[row][column] = static_cast<float>(global_transform[row][column]);
					}
				}

				fetch_animations(fbx_mesh, mesh.skeletal_animation, 0);

				// Count the polygon count of each material
				if (number_of_materials > 0)
				{
					// Count the faces of each material
					const int number_of_polygons = fbx_mesh->GetPolygonCount();
					for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; ++index_of_polygon)
					{
						const u_int material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
						mesh.subsets.at(material_index).index_count += 3;
					}
					// Record the offset (how many vertex)
					int offset = 0;
					for (subset& subset : mesh.subsets)
					{
						subset.index_start = offset;
						offset += subset.index_count;
						// This will be used as counter in the following procedures, reset to zero
						subset.index_count = 0;
					}
				}

				for (int index_of_material = 0; index_of_material < number_of_materials; ++index_of_material)
				{
					subset& subset = mesh.subsets.at(index_of_material); // UNIT.18
					const FbxSurfaceMaterial* surface_material = fbx_mesh->GetNode()->GetMaterial(index_of_material);
					const FbxProperty property = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
					const FbxProperty factor = surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);
					if (property.IsValid() && factor.IsValid())
					{
						FbxDouble3 color = property.Get<FbxDouble3>();
						double f = factor.Get<FbxDouble>();
						subset.diffuse.color.x = static_cast<float>(color[0] * f);
						subset.diffuse.color.y = static_cast<float>(color[1] * f);
						subset.diffuse.color.z = static_cast<float>(color[2] * f);
						subset.diffuse.color.w = 1.0f;
					}
					if (property.IsValid())
					{
						const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
						if (number_of_textures)
						{
							const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
							if (file_texture)
							{
								//画像読み込み
								D3D11_TEXTURE2D_DESC g_TEXTURE2D_DESC = {};
								const char* filename = file_texture->GetRelativeFileName();
								subset.diffuse.TexName = (string)filename;
								subset.diffuse.TexPass = (string)file_pass + (string)filename;
							}
						}
						else
						{
							string Texpass = "Default_Resource\\Image\\Default_Texture.png";
							subset.diffuse.TexName = "Default_Texture.png";
							subset.diffuse.TexPass = Texpass;
						}
					}
				}

				// Fetch mesh data
				FbxStringList uv_names;
				fbx_mesh->GetUVSetNames(uv_names);
				const FbxVector4* array_of_control_points = fbx_mesh->GetControlPoints();
				const int number_of_polygons = fbx_mesh->GetPolygonCount();
				if (mesh.subsets.size() > 0)
				{
					mesh.indices.resize(number_of_polygons * 3); // UNIT.18
				}
				for (int index_of_polygon = 0; index_of_polygon < number_of_polygons; index_of_polygon++)
				{
					// UNIT.18
					// The material for current face.
					int index_of_material = 0;
					if (number_of_materials > 0)
					{
						index_of_material = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(index_of_polygon);
						// Where should I save the vertex attribute index, according to the material
						subset& subset = mesh.subsets.at(index_of_material);
						const int index_offset = subset.index_start + subset.index_count;

						for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
						{
							vertex vertex;
							const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
							vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
							vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
							vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
							if (fbx_mesh->GetElementNormalCount())
							{
								FbxVector4 normal;
								fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
								vertex.normal.x = static_cast<float>(normal[0]);
								vertex.normal.y = static_cast<float>(normal[1]);
								vertex.normal.z = static_cast<float>(normal[2]);
							}
							if (fbx_mesh->GetElementUVCount())
							{
								FbxVector2 uv;
								bool unmapped_uv;
								fbx_mesh->GetPolygonVertexUV(index_of_polygon, index_of_vertex, uv_names[0], uv, unmapped_uv);
								vertex.texcoord.x = static_cast<float>(uv[0]);
								vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
							}
							if (fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) && bone_influences.size() > index_of_control_point)
							{
								for (int bone_influence_count = 0; bone_influence_count < bone_influences[index_of_control_point].size(); bone_influence_count++)
								{
									vertex.bone_weights[bone_influence_count] = bone_influences[index_of_control_point][bone_influence_count].weight;
									vertex.bone_indices[bone_influence_count] = bone_influences[index_of_control_point][bone_influence_count].index;
								}
							}
							mesh.vertices.push_back(vertex);
							// UNIT.18
							//indices.push_back(vertex_count);
							mesh.indices.at(index_offset + index_of_vertex) = static_cast<u_int>(vertex_count);
							vertex_count += 1;
						}
						subset.index_count += 3; // UNIT.18
					}
					else
					{
						u_int vertex_count_mat = 0;
						//create_ps_from_cso(device, "skinned_mesh_material_ps.cso", g_pPixelShader.GetAddressOf());
						for (int index_of_vertex = 0; index_of_vertex < 3; index_of_vertex++)
						{
							vertex vertex;
							const int index_of_control_point = fbx_mesh->GetPolygonVertex(index_of_polygon, index_of_vertex);
							vertex.position.x = static_cast<float>(array_of_control_points[index_of_control_point][0]);
							vertex.position.y = static_cast<float>(array_of_control_points[index_of_control_point][1]);
							vertex.position.z = static_cast<float>(array_of_control_points[index_of_control_point][2]);
							if (fbx_mesh->GetElementNormalCount())
							{
								FbxVector4 normal;
								fbx_mesh->GetPolygonVertexNormal(index_of_polygon, index_of_vertex, normal);
								vertex.normal.x = static_cast<float>(normal[0]);
								vertex.normal.y = static_cast<float>(normal[1]);
								vertex.normal.z = static_cast<float>(normal[2]);
							}
							if (fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) && bone_influences.size() > index_of_control_point)
							{
								for (int bone_influence_count = 0; bone_influence_count < bone_influences[index_of_control_point].size(); bone_influence_count++)
								{
									vertex.bone_weights[bone_influence_count] = bone_influences[index_of_control_point][bone_influence_count].weight;
									vertex.bone_indices[bone_influence_count] = bone_influences[index_of_control_point][bone_influence_count].index;
								}
							}
							mesh.vertices.push_back(vertex);
							mesh.indices.push_back(vertex_count_mat);
							vertex_count_mat += 1;
						}
					}
				}
				// 頂点バッファの生成
				{
					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.ByteWidth = sizeof(Mesh::vertex) * mesh.vertices.size();
					bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					D3D11_SUBRESOURCE_DATA InitData = {};
					InitData.pSysMem = &mesh.vertices[0];				// 頂点のアドレス
					InitData.SysMemPitch = 0;
					InitData.SysMemSlicePitch = 0;
					HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, mesh.vertex_buffer.GetAddressOf());
					assert(SUCCEEDED(hr), hr_trace(hr));
				}

				// インデックスバッファの生成
				{
					D3D11_BUFFER_DESC bd = {};
					//bd.Usage                      = D3D11_USAGE_DEFAULT;
					bd.Usage = D3D11_USAGE_IMMUTABLE;
					bd.ByteWidth = sizeof(u_int) * mesh.indices.size();
					bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					D3D11_SUBRESOURCE_DATA InitData = {};
					InitData.pSysMem = &mesh.indices[0];				// 頂点のアドレス
					InitData.SysMemPitch = 0;
					InitData.SysMemSlicePitch = 0;
					HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, mesh.index_buffer.GetAddressOf());
					assert(SUCCEEDED(hr), hr_trace(hr));
				}
			}

			ofstream ss(fullpass + ".bin", ios::binary);
			{
				cereal::BinaryOutputArchive o_archive(ss);
				o_archive(skin_meshes);
			}

			manager->Destroy();
		}

		for (int i = 0; i < skin_meshes.size(); i++)
		{
			//使わないので開放
			skin_meshes[i].vertices.clear();
			skin_meshes[i].indices.clear();
		}
		cache.insert(make_pair(fullpass, skin_meshes));
		mesh_ptr->skin_meshes = skin_meshes;
		return mesh_ptr;
	}
}

void Mesh::fetch_bone_influences(const FbxMesh* fbx_mesh, vector<bone_influences_per_control_point>& influences)
{
	const int number_of_control_points = fbx_mesh->GetControlPointsCount();
	influences.resize(number_of_control_points);
	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
		const int number_of_clusters = skin->GetClusterCount();
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			FbxCluster* cluster = skin->GetCluster(index_of_cluster);
			const int number_of_control_point_indices = cluster->GetControlPointIndicesCount();
			const int* array_of_control_point_indices = cluster->GetControlPointIndices();
			const double* array_of_control_point_weights = cluster->GetControlPointWeights();
			for (int i = 0; i < number_of_control_point_indices; ++i)
			{
				bone_influences_per_control_point& influences_per_control_point
					= influences.at(array_of_control_point_indices[i]);
				bone_influence influence;
				influence.index = index_of_cluster;
				influence.weight = static_cast<float>(array_of_control_point_weights[i]);
				influences_per_control_point.push_back(influence);
			}
		}
	}
}

void Mesh::fetch_bone_matrices(FbxMesh* fbx_mesh, vector<bone>& skeletal, FbxTime Time)
{
	const int number_of_deformers = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int index_of_deformer = 0; index_of_deformer < number_of_deformers; ++index_of_deformer)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(index_of_deformer, FbxDeformer::eSkin));
		const int number_of_clusters = skin->GetClusterCount();
		skeletal.resize(number_of_clusters);
		for (int index_of_cluster = 0; index_of_cluster < number_of_clusters; ++index_of_cluster)
		{
			bone& bone = skeletal.at(index_of_cluster);
			FbxCluster* cluster = skin->GetCluster(index_of_cluster);
			// this matrix trnasforms coordinates of the initial pose from mesh space to global space
			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);
			// this matrix trnasforms coordinates of the initial pose from bone space to global space
			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);
			// this matrix trnasforms coordinates of the current pose from bone space to global space
			FbxAMatrix cluster_global_current_position;
			cluster_global_current_position = cluster->GetLink()->EvaluateGlobalTransform(Time);
			// this matrix trnasforms coordinates of the current pose from mesh space to global space
			FbxAMatrix reference_global_current_position;
			reference_global_current_position = fbx_mesh->GetNode()->EvaluateGlobalTransform(Time);
			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation
			// (translation, rotation and scale), the last row of the matrix represents the translation part of the
			// transformation.
			FbxAMatrix transform = reference_global_current_position.Inverse() * cluster_global_current_position
				* cluster_global_init_position.Inverse() * reference_global_init_position;
			// convert FbxAMatrix(transform) to XMDLOAT4X4(bone.transform)
			for (int row = 0; row < 4; row++)
			{
				for (int column = 0; column < 4; column++)
				{
					bone.transform.m[row][column] = static_cast<float>(transform[row][column]);
				}
			}
		}
	}
}

void Mesh::fetch_animations(FbxMesh* fbx_mesh, vector<skeletal_animation>& skeletal_animation, u_int sampling_rate)
{
	// Get the list of all the animation stack.
	FbxArray<FbxString*> array_of_animation_stack_names;
	fbx_mesh->GetScene()->FillAnimStackNameArray(array_of_animation_stack_names);
	// Get the number of animations.
	int number_of_animations = array_of_animation_stack_names.Size();
	skeletal_animation.resize(number_of_animations);
	if (number_of_animations > 0)
	{
		// Get the FbxTime per animation's frame.
		FbxTime::EMode Time_mode = fbx_mesh->GetScene()->GetGlobalSettings().GetTimeMode();
		FbxTime frame_Time;
		frame_Time.SetTime(0, 0, 0, 1, 0, Time_mode);
		sampling_rate = sampling_rate > 0 ? sampling_rate : frame_Time.GetFrameRate(Time_mode);
		float sampling_Time = 1.0f / sampling_rate;
		for (int i = 0; i < number_of_animations; i++)
		{
			skeletal_animation[i].sampling_time = sampling_Time;
			skeletal_animation[i].animation_tick = 0.0f;
			FbxString* animation_stack_name = array_of_animation_stack_names.GetAt(i);
			FbxAnimStack* current_animation_stack
				= fbx_mesh->GetScene()->FindMember<FbxAnimStack>(animation_stack_name->Buffer());
			fbx_mesh->GetScene()->SetCurrentAnimationStack(current_animation_stack);
			FbxTakeInfo* take_info = fbx_mesh->GetScene()->GetTakeInfo(animation_stack_name->Buffer());
			FbxTime start_Time = take_info->mLocalTimeSpan.GetStart();
			FbxTime end_Time = take_info->mLocalTimeSpan.GetStop();
			FbxTime sampling_step;
			sampling_step.SetTime(0, 0, 1, 0, 0, Time_mode);
			sampling_step = static_cast<FbxLongLong>(sampling_step.Get() * sampling_Time);
			for (FbxTime current_Time = start_Time; current_Time < end_Time; current_Time += sampling_step)
			{
				vector<bone> skeletal;
				fetch_bone_matrices(fbx_mesh, skeletal, current_Time);
				skeletal_animation[i].bones.push_back(skeletal);
			}
		}
	}
	for (int i = 0; i < number_of_animations; i++)
	{
		delete array_of_animation_stack_names[i];
	}
}