#include "Model_Data.h"
#include "Material.h"
#include <functional>
#include <cassert>
#include "DxSystem.h"
#include "Asset_Manager.h"
#include "Engine.h"
#include "Misc.h"
using namespace std;
using namespace fbxsdk;
using namespace BeastEngine;
using namespace DirectX;

// FbxDouble2 → XMFLOAT2
inline XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbx_value)
{
	return XMFLOAT2(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1])
	);
}

// FbxDouble3 → XMFLOAT3
inline XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbx_value)
{
	return XMFLOAT3(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	);
}

// FbxDouble4 → XMFLOAT3
inline XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbx_value)
{
	return XMFLOAT3(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	);
}

// FbxDouble4 → XMFLOAT4
inline XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbx_value)
{
	return XMFLOAT4(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2]),
		static_cast<float>(fbx_value[3])
	);
}

// FbxDouble4 → XMFLOAT4
inline XMFLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbx_value)
{
	return XMFLOAT4X4(
		static_cast<float>(fbx_value[0][0]),
		static_cast<float>(fbx_value[0][1]),
		static_cast<float>(fbx_value[0][2]),
		static_cast<float>(fbx_value[0][3]),
		static_cast<float>(fbx_value[1][0]),
		static_cast<float>(fbx_value[1][1]),
		static_cast<float>(fbx_value[1][2]),
		static_cast<float>(fbx_value[1][3]),
		static_cast<float>(fbx_value[2][0]),
		static_cast<float>(fbx_value[2][1]),
		static_cast<float>(fbx_value[2][2]),
		static_cast<float>(fbx_value[2][3]),
		static_cast<float>(fbx_value[3][0]),
		static_cast<float>(fbx_value[3][1]),
		static_cast<float>(fbx_value[3][2]),
		static_cast<float>(fbx_value[3][3])
	);
}

inline Vector3 Vector_Min(Vector3 a, Vector3 b)
{
	return Vector3(min(a.x, b.x), min(a.y, b.y), min(a.z, b.z));
}
inline Vector3 Vector_Max(Vector3 a, Vector3 b)
{
	return Vector3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z));
}

shared_ptr<Model_Data> Model_Data::Load_Model(const string& file_path, const string& fbx_filename)
{
	shared_ptr<Model_Data> model_ptr = make_shared<Model_Data>();
	model_ptr->name = fbx_filename;
	model_ptr->file_path = file_path;

	const string full_path = file_path + fbx_filename + ".fbx";

	FbxManager* fbx_manager = FbxManager::Create();

	// FBXに対する入出力を定義する
	// 特別な理由がない限りIOSROOTを指定
	FbxIOSettings* fbx_ios = FbxIOSettings::Create(fbx_manager, IOSROOT);
	fbx_manager->SetIOSettings(fbx_ios);

	// インポータを生成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	const bool result = fbx_importer->Initialize(full_path.c_str(), -1, fbx_manager->GetIOSettings()); // -1でファイルフォーマット自動判定
	_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

	// SceneオブジェクトにFBXファイル内の情報を流し込む
	FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "scene");
	fbx_importer->Import(fbx_scene);
	fbx_importer->Destroy(); // シーンを流し込んだらImporterは解放してOK

	// ジオメトリを三角形化しておく
	FbxGeometryConverter fbx_geometry_converter(fbx_manager);
	fbx_geometry_converter.Triangulate(fbx_scene, true);
	fbx_geometry_converter.RemoveBadPolygonsFromMeshes(fbx_scene);

#if 0
	// DirectX座標系へ変換
	FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != FbxAxisSystem::DirectX)
	{
		FbxAxisSystem::DirectX.ConvertScene(fbxScene);
	}
#elif 0
	// OpenGL座標系へ変換
	FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != FbxAxisSystem::OpenGL)
	{
		FbxAxisSystem::OpenGL.ConvertScene(fbxScene);
	}
#endif
	// センチメーター単位にコンバートする
	if (const FbxSystemUnit scene_system_unit = fbx_scene->GetGlobalSettings().GetSystemUnit(); scene_system_unit.GetScaleFactor() != 1.0f)
	{
		FbxSystemUnit::cm.ConvertScene(fbx_scene);
	}

	// ディレクトリパス取得
	char dirname[256];
	::_splitpath_s(full_path.c_str(), nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

	// モデル構築
	FbxNode* fbx_root_node = fbx_scene->GetRootNode();
	model_ptr->BuildNodes(fbx_root_node, -1);
	model_ptr->BuildMeshes(fbx_root_node);

	// アニメーション構築
	model_ptr->BuildAnimations(fbx_scene);

	// マネージャ解放
	fbx_manager->Destroy();	// 関連するすべてのオブジェクトが解放される

	return model_ptr;
}

void Model_Data::BuildNodes(FbxNode* fbx_node, int parent_node_index)
{
	// FBXノードを再帰的に辿ってデータを構築する
	FbxNodeAttribute* fbxNodeAttribute = fbx_node->GetNodeAttribute();
	FbxNodeAttribute::EType fbxNodeAttributeType = FbxNodeAttribute::EType::eUnknown;

	if (fbxNodeAttribute != nullptr)
	{
		fbxNodeAttributeType = fbxNodeAttribute->GetAttributeType();
	}

	switch (fbxNodeAttributeType)
	{
		case FbxNodeAttribute::eUnknown:
			break;
		case FbxNodeAttribute::eNull:
			break;
		case FbxNodeAttribute::eMarker:
			break;
		case FbxNodeAttribute::eMesh:
			break;
		case FbxNodeAttribute::eSkeleton:
			BuildNode(fbx_node, parent_node_index);
			break;
		default:
			break;
	}

	// 再帰的に子ノードを処理する
	parent_node_index = static_cast<int>(bones.size() - 1);
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		BuildNodes(fbx_node->GetChild(i), parent_node_index);
	}
}

void Model_Data::BuildNode(const FbxNode* fbx_node, const int parent_node_index)
{
	// FBXノードからノードデータを構築する
	Skeleton node;
	node.name = fbx_node->GetName();
	node.parent_index = parent_node_index;

	bones.emplace_back(node);
}

void Model_Data::BuildMeshes(FbxNode* fbx_node)
{
	// FBXノードを再帰的に辿ってメッシュデータを構築する
	FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
	FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;

	if (fbx_node_attribute != nullptr)
	{
		fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
	}

	switch (fbx_node_attribute_type)
	{
		case FbxNodeAttribute::eMesh:
			BuildMesh(fbx_node, static_cast<FbxMesh*>(fbx_node_attribute));
			break;
		default:
			break;
	}

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		BuildMeshes(fbx_node->GetChild(i));
	}
}

void Model_Data::BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh)
{
	// FBXメッシュからメッシュデータを構築する
	int fbx_control_points_count = fbx_mesh->GetControlPointsCount();

	int fbx_material_count = fbx_node->GetMaterialCount();
	int fbx_polygon_count = fbx_mesh->GetPolygonCount();

	meshes.emplace_back(make_shared<Mesh>());
	shared_ptr<Mesh> mesh = meshes.back();
	Engine::asset_manager->Registration_Asset(mesh);
	mesh->name = fbx_node->GetName();
	mesh->subsets.resize(fbx_material_count > 0 ? fbx_material_count : 1);

	// サブセットのマテリアル設定
	if (fbx_material_count > 0)
	{
		for (int index_of_material = 0; index_of_material < fbx_material_count; ++index_of_material)
		{
			Mesh::subset& subset = mesh->subsets.at(index_of_material); // UNIT.18
			const FbxSurfaceMaterial* surface_material = fbx_node->GetMaterial(index_of_material);
			string material_name = surface_material->GetName();

			string new_mat_path = file_path + material_name + ".mat";

			{
				bool cashed = false;
				size_t mat_size = mesh->default_material_paths.size();
				for (size_t i = 0; i < mat_size; ++i)
				{
					if (mesh->default_material_paths[i] == new_mat_path)
					{
						cashed = true;
						subset.material_ID = i;
						break;
					}
				}
				if (cashed) continue;
			}

			mesh->default_material_paths.push_back(new_mat_path);
			subset.material_ID = mesh->default_material_paths.size() - 1;

			{
				bool cashed = false;
				for (auto& path : default_material_paths)
				{
					if (path == new_mat_path)
					{
						cashed = true;
						break;
					}
				}
				if (cashed) continue;
			}

			default_material_paths.push_back(new_mat_path);
		}
	}
	else
	{
		Mesh::subset& subset = mesh->subsets.front();
		string new_mat_path = file_path + mesh->name + ".mat";
		mesh->default_material_paths.push_back(new_mat_path);
		subset.material_ID = mesh->default_material_paths.size() - 1;
		default_material_paths.push_back(new_mat_path);
	}

	// サブセットの頂点インデックス範囲設定
	if (fbx_material_count > 0)
	{
		for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbx_polygon_count; ++fbxPolygonIndex)
		{
			int fbxMaterialIndex = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
			mesh->subsets.at(fbxMaterialIndex).index_count += 3;
		}

		int offset = 0;
		for (Mesh::subset& subset : mesh->subsets)
		{
			subset.index_start = offset;
			offset += subset.index_count;

			subset.index_count = 0;
		}
	}

	// 頂点影響力データ
	struct BoneInfluence
	{
		int useCount = 0;
		int indices[4] = { 0, 0, 0, 0 };
		float weights[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

		void Add(int index, float weight)
		{
			if (useCount < 4)
			{
				indices[useCount] = index;
				weights[useCount] = weight;
				useCount++;
			}
		}
	};

	// 頂点影響力データを抽出する
	vector<BoneInfluence> bone_influences;
	{
		bone_influences.resize(fbx_control_points_count);

		FbxAMatrix fbx_geometric_transform(
			fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricRotation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricScaling(FbxNode::eSourcePivot)
		);

		// スキニングに必要な情報を取得する
		int fbx_deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbx_deformer_index = 0; fbx_deformer_index < fbx_deformer_count; ++fbx_deformer_index)
		{
			auto fbx_skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(fbx_deformer_index, FbxDeformer::eSkin));

			int fbx_cluster_count = fbx_skin->GetClusterCount();
			for (int fbx_cluster_index = 0; fbx_cluster_index < fbx_cluster_count; ++fbx_cluster_index)
			{
				FbxCluster* fbx_cluster = fbx_skin->GetCluster(fbx_cluster_index);

				// 頂点影響力データを抽出する
				{
					int fbx_cluster_control_point_indices_count = fbx_cluster->GetControlPointIndicesCount();
					const int* fbx_control_point_indices = fbx_cluster->GetControlPointIndices();
					const double* fbx_control_point_weights = fbx_cluster->GetControlPointWeights();

					for (int i = 0; i < fbx_cluster_control_point_indices_count; ++i)
					{
						BoneInfluence& bone_influence = bone_influences.at(fbx_control_point_indices[i]);

						bone_influence.Add(fbx_cluster_index, static_cast<float>(fbx_control_point_weights[i]));
					}
				}

				// ボーン変換行列用の逆行列の計算をする
				{
					// メッシュ空間からワールド空間への変換行列
					FbxAMatrix fbx_mesh_space_transform;
					fbx_cluster->GetTransformMatrix(fbx_mesh_space_transform);

					// ボーン空間からワールド空間への変換行列
					FbxAMatrix fbx_bone_space_transform;
					fbx_cluster->GetTransformLinkMatrix(fbx_bone_space_transform);

					// ボーン逆行列を計算する
					FbxAMatrix fbx_inverse_transform = fbx_bone_space_transform.Inverse() * fbx_mesh_space_transform * fbx_geometric_transform;

					Matrix inverse_transform = FbxAMatrixToFloat4x4(fbx_inverse_transform);
					mesh->inverse_matrixes.emplace_back(inverse_transform);

					int node_index = FindNodeIndex(fbx_cluster->GetLink()->GetName());
					mesh->node_indices.emplace_back(node_index);
				}
			}
		}
		if (fbx_deformer_count == 0)
		{
			FbxAMatrix local_matrix = fbx_node->EvaluateLocalTransform();
			Matrix inverse_transform = FbxAMatrixToFloat4x4(local_matrix);
			mesh->inverse_matrixes.emplace_back(inverse_transform);
		}
	}

	// UVセット名
	FbxStringList fbx_uv_set_names;
	fbx_mesh->GetUVSetNames(fbx_uv_set_names);

	// 頂点データ
	vector<Mesh::vertex>& vertices = mesh->vertices;
	vector<u_int>& indices = mesh->indices;
	vertices.resize(fbx_polygon_count * 3);
	indices.resize(fbx_polygon_count * 3);

	int vertexCount = 0;
	const FbxVector4* fbx_control_points = fbx_mesh->GetControlPoints();
	bool no_tangent = false;
	Vector3 min_v = { FLT_MAX ,FLT_MAX ,FLT_MAX };
	Vector3 max_v = { -FLT_MAX ,-FLT_MAX ,-FLT_MAX };
	for (int fbx_polygon_index = 0; fbx_polygon_index < fbx_polygon_count; ++fbx_polygon_index)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する
		int fbx_material_index = 0;
		if (fbx_material_count > 0)
		{
			fbx_material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbx_polygon_index);
		}

		Mesh::subset& subset = mesh->subsets.at(fbx_material_index);
		const u_int index_offset = subset.index_start + subset.index_count;

		for (int fbx_vertex_index = 0; fbx_vertex_index < 3; ++fbx_vertex_index)
		{
			Mesh::vertex vertex;

			int fbx_control_point_index = fbx_mesh->GetPolygonVertex(fbx_polygon_index, fbx_vertex_index);
			// Position
			{
				vertex.position = FbxDouble4ToFloat3(fbx_control_points[fbx_control_point_index]);
				min_v = Vector_Min(vertex.position, min_v);
				max_v = Vector_Max(vertex.position, max_v);
			}

			// Weight
			{
				BoneInfluence& bone_influence = bone_influences.at(fbx_control_point_index);
				vertex.bone_indices[0] = bone_influence.indices[0];
				vertex.bone_indices[1] = bone_influence.indices[1];
				vertex.bone_indices[2] = bone_influence.indices[2];
				vertex.bone_indices[3] = bone_influence.indices[3];
				vertex.bone_weights[0] = bone_influence.weights[0];
				vertex.bone_weights[1] = bone_influence.weights[1];
				vertex.bone_weights[2] = bone_influence.weights[2];
				vertex.bone_weights[3] = 1.0f - (vertex.bone_weights[0] + vertex.bone_weights[1] + vertex.bone_weights[2]);
				//vertex.boneWeight.w = boneInfluence.weights[3];
			}

			// Normal
			if (fbx_mesh->GetElementNormalCount() > 0)
			{
				FbxVector4 fbx_normal;
				fbx_mesh->GetPolygonVertexNormal(fbx_polygon_index, fbx_vertex_index, fbx_normal);
				vertex.normal = FbxDouble4ToFloat3(fbx_normal);
			}
			else
			{
				vertex.normal = Vector3(0, 0, 0);
			}

			// Tangent
			if (fbx_mesh->GetElementTangentCount() > 0)
			{
				const FbxGeometryElementTangent* geometry_element_tangent = fbx_mesh->GetElementTangent(0);
				//以下コメントはサンプルからのコピペ
				/**	\enum EMappingMode		Determines how the element is mapped to a surface.
				* - \e eNone                The mapping is undetermined.
				* - \e eByControlPoint      There will be one mapping coordinate for each surface control point/vertex.
				* - \e eByPolygonVertex     There will be one mapping coordinate for each vertex, for every polygon of which it is a part.
				*							This means that a vertex will have as many mapping coordinates as polygons of which it is a part.
				* - \e eByPolygon           There can be only one mapping coordinate for the whole polygon.
				* - \e eByEdge              There will be one mapping coordinate for each unique edge in the mesh.
				*							This is meant to be used with smoothing layer elements.
				* - \e eAllSame             There can be only one mapping coordinate for the whole surface.
				*/
				FbxGeometryElement::EMappingMode mapping_mode = geometry_element_tangent->GetMappingMode();
				/** \enum EReferenceMode    Determines how the mapping information is stored in the array of coordinates.
				* - \e eDirect              This indicates that the mapping information for the n'th element is found in the n'th place of
				*							FbxLayerElementTemplate::mDirectArray.
				* - \e eIndex,              This symbol is kept for backward compatibility with FBX v5.0 files. In FBX v6.0 and higher,
				*							this symbol is replaced with eIndexToDirect.
				* - \e eIndexToDirect		This indicates that the FbxLayerElementTemplate::mIndexArray
				*							contains, for the n'th element, an index in the FbxLayerElementTemplate::mDirectArray
				*							array of mapping elements. eIndexToDirect is usually useful for storing eByPolygonVertex mapping
				*							mode elements coordinates. Since the same coordinates are usually
				*							repeated many times, this saves spaces by storing the coordinate only one time
				*							and then referring to them with an index. Materials and Textures are also referenced with this
				*							mode and the actual Material/Texture can be accessed via the FbxLayerElementTemplate::mDirectArray
				*/
				FbxGeometryElement::EReferenceMode reference_mode = geometry_element_tangent->GetReferenceMode();

				if (mapping_mode == FbxGeometryElement::EMappingMode::eByControlPoint)
				{
					int index_of_vertex = 0;
					switch (reference_mode)
					{
						case FbxGeometryElement::EReferenceMode::eDirect:
							index_of_vertex = fbx_control_point_index;
							break;
						case FbxGeometryElement::EReferenceMode::eIndexToDirect:
							index_of_vertex = geometry_element_tangent->GetIndexArray().GetAt(fbx_control_point_index);
							break;
						case FbxGeometryElement::EReferenceMode::eIndex:
						default:
							_ASSERT_EXPR(false, L"Invalid Reference Mode");
							throw exception("Invalid Reference Mode");
					}
					vertex.tangent.x = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[0]);
					vertex.tangent.y = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[1]);
					vertex.tangent.z = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[2]);
				}
				else if (mapping_mode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
				{
					int index_of_vertex = 0;
					switch (reference_mode)
					{
						case FbxGeometryElement::EReferenceMode::eDirect:
							index_of_vertex = fbx_polygon_index * 3 + fbx_vertex_index;
							break;
						case FbxGeometryElement::EReferenceMode::eIndexToDirect:
							index_of_vertex = geometry_element_tangent->GetIndexArray().GetAt(fbx_control_point_index);
							break;
						case FbxGeometryElement::EReferenceMode::eIndex:
						default:
							_ASSERT_EXPR(false, L"Invalid Reference Mode");
							throw exception("Invalid Reference Mode");
					}
					vertex.tangent.x = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[0]);
					vertex.tangent.y = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[1]);
					vertex.tangent.z = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[2]);
				}
				else
				{
					_ASSERT_EXPR(false, L"Invalid Mapping Mode");
					throw exception("Invalid Mapping Mode");
				}
			}
			else
			{
				no_tangent = true;
			}

			// Texcoord
			if (fbx_mesh->GetElementUVCount() > 0)
			{
				bool fbx_unmapped_uv;
				FbxVector2 fbx_uv;
				fbx_mesh->GetPolygonVertexUV(fbx_polygon_index, fbx_vertex_index, fbx_uv_set_names[0], fbx_uv, fbx_unmapped_uv);
				fbx_uv[1] = 1.0 - fbx_uv[1];
				vertex.texcoord = FbxDouble2ToFloat2(fbx_uv);
			}
			else
			{
				vertex.texcoord = Vector2(0, 0);
			}

			indices.at(index_offset + fbx_vertex_index) = vertexCount;
			vertices.at(vertexCount) = vertex;
			vertexCount++;
		}

		subset.index_count += 3;
	}

	if (no_tangent)
	{
		//タンジェントのデータが存在しない場合、頂点情報から新規に算出する
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			Mesh::vertex* v[3] = { &vertices.at(indices.at(i + 0)), &vertices.at(indices.at(i + 1)), &vertices.at(indices.at(i + 2)) };

			float x1 = v[1]->position.x - v[0]->position.x;
			float y1 = v[1]->position.y - v[0]->position.y;
			float z1 = v[1]->position.z - v[0]->position.z;
			float x2 = v[2]->position.x - v[0]->position.x;
			float y2 = v[2]->position.y - v[0]->position.y;
			float z2 = v[2]->position.z - v[0]->position.z;

			float s1 = v[1]->texcoord.x - v[0]->texcoord.x;
			float t1 = v[1]->texcoord.y - v[0]->texcoord.y;
			float s2 = v[2]->texcoord.x - v[0]->texcoord.x;
			float t2 = v[2]->texcoord.y - v[0]->texcoord.y;
			float r = 1.0f / (s1 * t2 - s2 * t1);

			XMVECTOR t = XMLoadFloat3(&XMFLOAT3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r));
			XMVECTOR b = XMLoadFloat3(&XMFLOAT3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r));

			for (Mesh::vertex* p : v)
			{
				XMVECTOR n = XMLoadFloat3(&p->normal);
				XMStoreFloat3(&p->tangent, XMVector3Normalize(XMVectorSubtract(t, XMVectorMultiply(n, XMVector3Dot(t, n)))));

				if (XMVectorGetX(XMVector3Dot(XMVector3Cross(n, t), b)) < 0)
				{
					p->tangent.x *= -1;
					p->tangent.y *= -1;
					p->tangent.z *= -1;
				}
			}
		}
	}

	mesh->boundingbox.Set_Size(max_v - min_v);

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Mesh::vertex) * vertices.size());
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.data();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = DxSystem::device->CreateBuffer(&bufferDesc, &subresourceData, mesh->vertex_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	// インデックスバッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * indices.size());
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = &indices[0];
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
		HRESULT hr = DxSystem::device->CreateBuffer(&bufferDesc, &subresourceData, mesh->index_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

void Model_Data::BuildAnimations(FbxScene* fbx_scene)
{
	// アニメーションデータを構築
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbx_anim_stack_names;
	fbx_scene->FillAnimStackNameArray(fbx_anim_stack_names);

	const int fbx_animation_count = fbx_anim_stack_names.Size();
	for (int fbx_animation_index = 0; fbx_animation_index < fbx_animation_count; ++fbx_animation_index)
	{
		animations.emplace_back(Animation());
		Animation& animation = animations.back();

		// アニメーションデータのサンプリング設定
		const FbxTime::EMode fbx_time_mode = fbx_scene->GetGlobalSettings().GetTimeMode();
		FbxTime fbx_frame_time;
		fbx_frame_time.SetTime(0, 0, 0, 1, 0, fbx_time_mode);

		const float sampling_rate = static_cast<float>(fbx_frame_time.GetFrameRate(fbx_time_mode));
		const float sampling_time = 1.0f / sampling_rate;

		FbxString* fbx_anim_stack_name = fbx_anim_stack_names.GetAt(fbx_animation_index);
		FbxAnimStack* fbx_anim_stack = fbx_scene->FindMember<FbxAnimStack>(fbx_anim_stack_name->Buffer());
		animation.name = fbx_anim_stack_name->Buffer();

		// 再生するアニメーションを指定する
		fbx_scene->SetCurrentAnimationStack(fbx_anim_stack);

		// アニメーションの再生開始時間と再生終了時間を取得する
		const FbxTakeInfo* fbx_take_info = fbx_scene->GetTakeInfo(fbx_anim_stack_name->Buffer());
		FbxTime fbx_start_time = fbx_take_info->mLocalTimeSpan.GetStart();
		FbxTime fbx_end_time = fbx_take_info->mLocalTimeSpan.GetStop();

		// 抽出するデータは60フレーム基準でサンプリングする
		FbxTime fbx_sampling_step;
		fbx_sampling_step.SetTime(0, 0, 1, 0, 0, fbx_time_mode);
		fbx_sampling_step = static_cast<FbxLongLong>(fbx_sampling_step.Get() * sampling_time);

		const int frame_count = static_cast<int>((fbx_end_time.Get() - fbx_start_time.Get()) / fbx_sampling_step.Get());

		// アニメーションの対象となるノード(ボーン)を列挙する
		vector<FbxNode*> fbx_nodes;
		FbxNode* fbx_root_node = fbx_scene->GetRootNode();
		for (Skeleton& node : bones)
		{
			FbxNode* fbx_node = fbx_root_node->FindChild(node.name.c_str());
			fbx_nodes.emplace_back(fbx_node);
		}

		// アニメーションデータを抽出する
		animation.seconds_length = static_cast<float>(frame_count) * sampling_time;
		animation.keyframes.resize(frame_count + 1);

		float seconds = 0.0f;
		Keyframe* keyframe = animation.keyframes.data();
		const size_t fbx_node_count = fbx_nodes.size();
		for (FbxTime fbx_current_time = fbx_start_time; fbx_current_time <= fbx_end_time; fbx_current_time += fbx_sampling_step, ++keyframe)
		{
			// キーフレーム毎の姿勢データを取り出す
			keyframe->seconds = seconds;
			if (keyframe == &animation.keyframes.back()) keyframe->seconds = animation.seconds_length;
			keyframe->node_keys.resize(fbx_node_count);
			for (size_t fbx_node_index = 0; fbx_node_index < fbx_node_count; ++fbx_node_index)
			{
				Key_Pose& key_data = keyframe->node_keys.at(fbx_node_index);
				FbxNode* fbx_node = fbx_nodes.at(fbx_node_index);
				if (fbx_node == nullptr)
				{
					// アニメーション対象のノードがなかったのでダミーデータを設定
					const Skeleton& node = bones.at(fbx_node_index);
					key_data.scale = node.scale;
					key_data.rotation = node.rotation;
					key_data.position = node.position;
				}
				else if (fbx_node_index == root_motion_node_index)
				{
					// ルートモーションは無視する
					key_data.scale = XMFLOAT3(1, 1, 1);
					key_data.rotation = XMFLOAT4(0, 0, 0, 1);
					key_data.position = XMFLOAT3(0, 0, 0);
				}
				else
				{
					// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す
					const FbxAMatrix& fbx_local_transform = fbx_node->EvaluateLocalTransform(fbx_current_time);
					key_data.scale = FbxDouble4ToFloat3(fbx_local_transform.GetS());
					key_data.rotation = FbxDouble4ToFloat4(fbx_local_transform.GetQ());
					key_data.position = FbxDouble4ToFloat3(fbx_local_transform.GetT());
				}
			}
			seconds += sampling_time;
		}
	}

	// 後始末
	for (int i = 0; i < fbx_animation_count; i++)
	{
		delete fbx_anim_stack_names[i];
	}
}

int Model_Data::FindNodeIndex(const char* name)
{
	// ノードインデックスを取得する
	for (size_t i = 0; i < bones.size(); ++i)
	{
		if (bones[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}