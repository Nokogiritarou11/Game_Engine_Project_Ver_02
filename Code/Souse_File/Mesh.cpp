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

// FbxDouble2 → XMFLOAT2
inline DirectX::XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbxValue)
{
	return DirectX::XMFLOAT2(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1])
	);
}

// FbxDouble3 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2]),
		static_cast<float>(fbxValue[3])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbxValue)
{
	return DirectX::XMFLOAT4X4(
		static_cast<float>(fbxValue[0][0]),
		static_cast<float>(fbxValue[0][1]),
		static_cast<float>(fbxValue[0][2]),
		static_cast<float>(fbxValue[0][3]),
		static_cast<float>(fbxValue[1][0]),
		static_cast<float>(fbxValue[1][1]),
		static_cast<float>(fbxValue[1][2]),
		static_cast<float>(fbxValue[1][3]),
		static_cast<float>(fbxValue[2][0]),
		static_cast<float>(fbxValue[2][1]),
		static_cast<float>(fbxValue[2][2]),
		static_cast<float>(fbxValue[2][3]),
		static_cast<float>(fbxValue[3][0]),
		static_cast<float>(fbxValue[3][1]),
		static_cast<float>(fbxValue[3][2]),
		static_cast<float>(fbxValue[3][3])
	);
}

shared_ptr<Mesh> Mesh::Load_Mesh(const char* file_path, const char* fbx_filename, const char* ignoreRootMotionNodeName)
{
	shared_ptr<Mesh> mesh_ptr = make_shared<Mesh>();
	mesh_ptr->name = (string)fbx_filename;
	mesh_ptr->file_path = (string)file_path;

	static unordered_map<string, shared_ptr<Mesh>> cache;
	const string fullpath = (string)file_path + (string)fbx_filename;

	auto it = cache.find(fullpath);
	if (it != cache.end())
	{
		mesh_ptr = it->second;
		return mesh_ptr;
	}
	else //ファイルから読み込み
	{
		const string bin = fullpath + ".mesh";
		ifstream in_bin(bin, ios::binary);
		if (in_bin.is_open())
		{
			stringstream bin_s_stream;
			bin_s_stream << in_bin.rdbuf();
			cereal::BinaryInputArchive binaryInputArchive(bin_s_stream);
			binaryInputArchive(mesh_ptr);

			mesh_ptr->name = (string)fbx_filename;
			mesh_ptr->file_path = (string)file_path;

			for (u_int i = 0; i < mesh_ptr->meshes.size(); i++)
			{
				// 頂点バッファの生成
				{
					D3D11_BUFFER_DESC bd = {};
					bd.Usage = D3D11_USAGE_DEFAULT;
					bd.ByteWidth = sizeof(Mesh::vertex) * mesh_ptr->meshes[i].vertices.size();
					bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					D3D11_SUBRESOURCE_DATA InitData = {};
					InitData.pSysMem = &mesh_ptr->meshes[i].vertices[0];				// 頂点のアドレス
					InitData.SysMemPitch = 0;
					InitData.SysMemSlicePitch = 0;
					HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, mesh_ptr->meshes[i].vertex_buffer.GetAddressOf());
					assert(SUCCEEDED(hr));
				}

				// インデックスバッファの生成
				{
					D3D11_BUFFER_DESC bd = {};
					//bd.Usage                      = D3D11_USAGE_DEFAULT;
					bd.Usage = D3D11_USAGE_IMMUTABLE;
					bd.ByteWidth = sizeof(u_int) * mesh_ptr->meshes[i].indices.size();
					bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
					bd.CPUAccessFlags = 0;
					bd.MiscFlags = 0;
					bd.StructureByteStride = 0;
					D3D11_SUBRESOURCE_DATA InitData = {};
					InitData.pSysMem = &mesh_ptr->meshes[i].indices[0];				// 頂点のアドレス
					InitData.SysMemPitch = 0;
					InitData.SysMemSlicePitch = 0;
					HRESULT hr = DxSystem::Device->CreateBuffer(&bd, &InitData, mesh_ptr->meshes[i].index_buffer.GetAddressOf());
					assert(SUCCEEDED(hr));
				}

				//使わないので開放
				//mesh_ptr->meshes[i].vertices.clear();
				//mesh_ptr->meshes[i].indices.clear();
			}
			cache.insert(make_pair(fullpath, mesh_ptr));
		}
		else
		{
			const string fbx = fullpath + ".fbx";

			FbxManager* fbxManager = FbxManager::Create();

			// FBXに対する入出力を定義する
			FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbxManager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
			fbxManager->SetIOSettings(fbxIOS);

			// インポータを生成
			FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
			bool result = fbxImporter->Initialize(fbx.c_str(), -1, fbxManager->GetIOSettings());	// -1でファイルフォーマット自動判定
			_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

			// SceneオブジェクトにFBXファイル内の情報を流し込む
			FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
			fbxImporter->Import(fbxScene);
			fbxImporter->Destroy();	// シーンを流し込んだらImporterは解放してOK

									// ジオメトリを三角形化しておく
			FbxGeometryConverter fbxGeometryConverter(fbxManager);
			fbxGeometryConverter.Triangulate(fbxScene, true);
			fbxGeometryConverter.RemoveBadPolygonsFromMeshes(fbxScene);

			bool isRight_Hand = false;
			FbxAxisSystem::ECoordSystem lCoorSystem = fbxScene->GetGlobalSettings().GetAxisSystem().GetCoorSystem();
			if (lCoorSystem == FbxAxisSystem::eRightHanded)
			{
				//isRight_Hand = true;
			}

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
			// 単位系の統一。
			FbxSystemUnit SceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
			// センチメーター単位にコンバートする。
			if (SceneSystemUnit.GetScaleFactor() != 1.0f)
			{
				FbxSystemUnit::cm.ConvertScene(fbxScene);
			}

			// ディレクトリパス取得
			char dirname[256];
			::_splitpath_s(fbx.c_str(), nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

			// モデル構築
			FbxNode* fbxRootNode = fbxScene->GetRootNode();
			mesh_ptr->BuildNodes(fbxRootNode, -1, isRight_Hand);
			mesh_ptr->BuildMeshes(fbxRootNode, isRight_Hand);

			// 無視するルートモーションを検索
			if (ignoreRootMotionNodeName != nullptr)
			{
				mesh_ptr->rootMotionNodeIndex = -1;
				for (size_t i = 0; i < mesh_ptr->nodes.size(); ++i)
				{
					if (mesh_ptr->nodes.at(i).name == ignoreRootMotionNodeName)
					{
						mesh_ptr->rootMotionNodeIndex = static_cast<int>(i);
						break;
					}
				}
			}

			// アニメーション構築
			mesh_ptr->BuildAnimations(fbxScene, isRight_Hand);

			// マネージャ解放
			fbxManager->Destroy();		// 関連するすべてのオブジェクトが解放される

			cache.insert(make_pair(fullpath, mesh_ptr));

			ofstream ss(fullpath + ".mesh", ios::binary);
			{
				cereal::BinaryOutputArchive o_archive(ss);
				o_archive(mesh_ptr);
			}
		}
		return mesh_ptr;
	}
}

// FBXノードを再帰的に辿ってデータを構築する
void Mesh::BuildNodes(FbxNode* fbxNode, int parentNodeIndex, bool isRight_Hand)
{
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
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
			BuildNode(fbxNode, parentNodeIndex, isRight_Hand);
			break;
	}

	// 再帰的に子ノードを処理する
	parentNodeIndex = static_cast<int>(nodes.size() - 1);
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildNodes(fbxNode->GetChild(i), parentNodeIndex, isRight_Hand);
	}
}

// FBXノードからノードデータを構築する
void Mesh::BuildNode(FbxNode* fbxNode, int parentNodeIndex, bool isRight_Hand)
{
	FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform();

	Node node;
	node.name = fbxNode->GetName();
	node.parentIndex = parentNodeIndex;
	node.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
	node.rotation = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
	node.position = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
	///*
	if (isRight_Hand)
	{
		//node.scale.z *= -1;
		//node.rotation.x *= -1;
		//node.rotation.y *= -1;
	}//*/

	nodes.emplace_back(node);
}

// FBXノードを再帰的に辿ってメッシュデータを構築する
void Mesh::BuildMeshes(FbxNode* fbxNode, bool isRight_Hand)
{
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
	FbxNodeAttribute::EType fbxNodeAttributeType = FbxNodeAttribute::EType::eUnknown;

	if (fbxNodeAttribute != nullptr)
	{
		fbxNodeAttributeType = fbxNodeAttribute->GetAttributeType();
	}

	switch (fbxNodeAttributeType)
	{
		case FbxNodeAttribute::eMesh:
			BuildMesh(fbxNode, static_cast<FbxMesh*>(fbxNodeAttribute), isRight_Hand);
			break;
	}

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildMeshes(fbxNode->GetChild(i), isRight_Hand);
	}
}

// FBXメッシュからメッシュデータを構築する
void Mesh::BuildMesh(FbxNode* fbxNode, FbxMesh* fbxMesh, bool isRight_Hand)
{
	int fbxControlPointsCount = fbxMesh->GetControlPointsCount();
	//int fbxPolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbxMesh->GetPolygonVertices();

	int fbxMaterialCount = fbxNode->GetMaterialCount();
	int fbxPolygonCount = fbxMesh->GetPolygonCount();

	meshes.emplace_back(mesh());
	mesh& mesh = meshes.back();
	mesh.subsets.resize(fbxMaterialCount > 0 ? fbxMaterialCount : 1);
	mesh.nodeIndex = FindNodeIndex(fbxNode->GetName());

	// サブセットのマテリアル設定
	for (int index_of_material = 0; index_of_material < fbxMaterialCount; ++index_of_material)
	{
		subset& subset = mesh.subsets.at(index_of_material); // UNIT.18
		const FbxSurfaceMaterial* surface_material = fbxNode->GetMaterial(index_of_material);
		string material_name = surface_material->GetName();

		string new_mat_path = file_path + material_name + ".mat";
		bool cashed = false;
		for (u_int i = 0; i < Default_Material_Passes.size(); i++)
		{
			if (Default_Material_Passes[i] == new_mat_path)
			{
				subset.material_ID = i;
				cashed = true;
			}
		}
		if (cashed) continue;

		shared_ptr<Material> mat = Material::Create(file_path, material_name, L"Shader/Standard_Shader_PS.hlsl");

		//Main(Diffuse)Texture
		GetTexture(surface_material, FbxSurfaceMaterial::sDiffuse, mat, Texture::Texture_Type::Main);
		//SpecularTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sSpecular, mat, Texture::Texture_Type::Specular);
		//NormalTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sNormalMap, mat, Texture::Texture_Type::Normal);
		//HeightTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sBump, mat, Texture::Texture_Type::Height);
		//EmissionTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sEmissive, mat, Texture::Texture_Type::Emission);

		subset.material_ID = Default_Material_Passes.size();

		mat->Save();
		Default_Material_Passes.push_back(new_mat_path);
	}

	// サブセットの頂点インデックス範囲設定
	if (fbxMaterialCount > 0)
	{
		for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
		{
			int fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
			mesh.subsets.at(fbxMaterialIndex).index_count += 3;
		}

		int offset = 0;
		for (subset& subset : mesh.subsets)
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
	std::vector<BoneInfluence> boneInfluences;
	{
		boneInfluences.resize(fbxControlPointsCount);

		FbxAMatrix fbxGeometricTransform(
			fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricScaling(FbxNode::eSourcePivot)
		);

		// スキニングに必要な情報を取得する
		int fbxDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbxDeformerIndex = 0; fbxDeformerIndex < fbxDeformerCount; ++fbxDeformerIndex)
		{
			FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(fbxDeformerIndex, FbxDeformer::eSkin));

			int fbxClusterCount = fbxSkin->GetClusterCount();
			for (int fbxClusterIndex = 0; fbxClusterIndex < fbxClusterCount; ++fbxClusterIndex)
			{
				FbxCluster* fbxCluster = fbxSkin->GetCluster(fbxClusterIndex);

				// 頂点影響力データを抽出する
				{
					int fbxClusterControlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
					const int* fbxControlPointIndices = fbxCluster->GetControlPointIndices();
					const double* fbxControlPointWeights = fbxCluster->GetControlPointWeights();

					for (int i = 0; i < fbxClusterControlPointIndicesCount; ++i)
					{
						BoneInfluence& boneInfluence = boneInfluences.at(fbxControlPointIndices[i]);

						boneInfluence.Add(fbxClusterIndex, static_cast<float>(fbxControlPointWeights[i]));
					}
				}

				// ボーン変換行列用の逆行列の計算をする
				{
					// メッシュ空間からワールド空間への変換行列
					FbxAMatrix fbxMeshSpaceTransform;
					fbxCluster->GetTransformMatrix(fbxMeshSpaceTransform);

					// ボーン空間からワールド空間への変換行列
					FbxAMatrix fbxBoneSpaceTransform;
					fbxCluster->GetTransformLinkMatrix(fbxBoneSpaceTransform);

					// ボーン逆行列を計算する
					FbxAMatrix fbxInverseTransform = fbxBoneSpaceTransform.Inverse() * fbxMeshSpaceTransform * fbxGeometricTransform;

					DirectX::XMFLOAT4X4 inverseTransform = FbxAMatrixToFloat4x4(fbxInverseTransform);
					mesh.inverseTransforms.emplace_back(inverseTransform);

					int nodeIndex = FindNodeIndex(fbxCluster->GetLink()->GetName());
					mesh.nodeIndices.emplace_back(nodeIndex);
				}
			}
		}
	}

	// UVセット名
	FbxStringList fbxUVSetNames;
	fbxMesh->GetUVSetNames(fbxUVSetNames);

	// 頂点データ
	std::vector<vertex>& vertices = mesh.vertices;
	std::vector<u_int>& indices = mesh.indices;
	vertices.resize(fbxPolygonCount * 3);
	indices.resize(fbxPolygonCount * 3);

	int vertexCount = 0;
	const FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();
	bool No_Tanjent = false;
	for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する
		int fbxMaterialIndex = 0;
		if (fbxMaterialCount > 0)
		{
			fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
		}

		subset& subset = mesh.subsets.at(fbxMaterialIndex);
		const int indexOffset = subset.index_start + subset.index_count;

		for (int fbxVertexIndex = 0; fbxVertexIndex < 3; ++fbxVertexIndex)
		{
			vertex vertex;

			int fbxControlPointIndex = fbxMesh->GetPolygonVertex(fbxPolygonIndex, fbxVertexIndex);
			// Position
			{
				vertex.position = FbxDouble4ToFloat3(fbxControlPoints[fbxControlPointIndex]);
				///*
				if (isRight_Hand)
				{
					vertex.position.z *= -1;
				}//*/
			}

			// Weight
			{
				BoneInfluence& boneInfluence = boneInfluences.at(fbxControlPointIndex);
				vertex.bone_indices[0] = boneInfluence.indices[0];
				vertex.bone_indices[1] = boneInfluence.indices[1];
				vertex.bone_indices[2] = boneInfluence.indices[2];
				vertex.bone_indices[3] = boneInfluence.indices[3];
				vertex.bone_weights[0] = boneInfluence.weights[0];
				vertex.bone_weights[1] = boneInfluence.weights[1];
				vertex.bone_weights[2] = boneInfluence.weights[2];
				vertex.bone_weights[3] = 1.0f - (vertex.bone_weights[0] + vertex.bone_weights[1] + vertex.bone_weights[2]);
				//vertex.boneWeight.w = boneInfluence.weights[3];
			}

			// Normal
			if (fbxMesh->GetElementNormalCount() > 0)
			{
				FbxVector4 fbxNormal;
				fbxMesh->GetPolygonVertexNormal(fbxPolygonIndex, fbxVertexIndex, fbxNormal);
				vertex.normal = FbxDouble4ToFloat3(fbxNormal);
				///*
				if (isRight_Hand)
				{
					vertex.normal.z *= -1;
				}//*/
			}
			else
			{
				vertex.normal = DirectX::XMFLOAT3(0, 0, 0);
			}

			// Tangent
			if (fbxMesh->GetElementTangentCount() > 0)
			{
				const FbxGeometryElementTangent* geometry_element_tangent = fbxMesh->GetElementTangent(0);
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
							index_of_vertex = fbxControlPointIndex;
							break;
						case FbxGeometryElement::EReferenceMode::eIndexToDirect:
							index_of_vertex = geometry_element_tangent->GetIndexArray().GetAt(fbxControlPointIndex);
							break;
						case FbxGeometryElement::EReferenceMode::eIndex:
						default:
							_ASSERT_EXPR(false, L"Invalid Reference Mode");
							throw std::exception("Invalid Reference Mode");
					}
					vertex.tangent.x = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[0]);
					vertex.tangent.y = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[1]);
					vertex.tangent.z = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[2]);
					///*
					if (isRight_Hand)
					{
						vertex.tangent.z *= -1;
					}//*/
				}
				else if (mapping_mode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
				{
					int index_of_vertex = 0;
					switch (reference_mode)
					{
						case FbxGeometryElement::EReferenceMode::eDirect:
							index_of_vertex = fbxPolygonIndex * 3 + fbxVertexIndex;
							break;
						case FbxGeometryElement::EReferenceMode::eIndexToDirect:
							index_of_vertex = geometry_element_tangent->GetIndexArray().GetAt(fbxControlPointIndex);
							break;
						case FbxGeometryElement::EReferenceMode::eIndex:
						default:
							_ASSERT_EXPR(false, L"Invalid Reference Mode");
							throw std::exception("Invalid Reference Mode");
					}
					vertex.tangent.x = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[0]);
					vertex.tangent.y = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[1]);
					vertex.tangent.z = static_cast<float>(geometry_element_tangent->GetDirectArray().GetAt(index_of_vertex)[2]);
					///*
					if (isRight_Hand)
					{
						vertex.tangent.z *= -1;
					}//*/
				}
				else
				{
					_ASSERT_EXPR(false, L"Invalid Mapping Mode");
					throw std::exception("Invalid Mapping Mode");
				}
			}
			else
			{
				No_Tanjent = true;
			}

			// Texcoord
			if (fbxMesh->GetElementUVCount() > 0)
			{
				bool fbxUnmappedUV;
				FbxVector2 fbxUV;
				fbxMesh->GetPolygonVertexUV(fbxPolygonIndex, fbxVertexIndex, fbxUVSetNames[0], fbxUV, fbxUnmappedUV);
				fbxUV[1] = 1.0 - fbxUV[1];
				vertex.texcoord = FbxDouble2ToFloat2(fbxUV);
			}
			else
			{
				vertex.texcoord = DirectX::XMFLOAT2(0, 0);
			}

			indices.at(indexOffset + fbxVertexIndex) = vertexCount;
			vertices.at(vertexCount) = vertex;
			vertexCount++;
		}

		subset.index_count += 3;
	}

	if (No_Tanjent)
	{
		// TODO:This code should be optimized.
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			vertex* v[3] = { &vertices.at(indices.at(i + 0)), &vertices.at(indices.at(i + 1)), &vertices.at(indices.at(i + 2)) };

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

			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r));
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r));

			for (vertex* p : v)
			{
				DirectX::XMVECTOR N = DirectX::XMLoadFloat3(&p->normal);
				DirectX::XMStoreFloat3(&p->tangent, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(T, DirectX::XMVectorMultiply(N, DirectX::XMVector3Dot(T, N)))));

				if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Cross(N, T), B)) < 0)
				{
					p->tangent.x *= -1;
					p->tangent.y *= -1;
					p->tangent.z *= -1;
					///*
					if (isRight_Hand)
					{
						p->tangent.z *= -1;
					}//*/
				}
			}
		}
	}

	// 頂点バッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = vertices.data();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;

		HRESULT hr = DxSystem::Device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertex_buffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	// インデックスバッファ
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * indices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = indices.data();
		subresourceData.SysMemPitch = 0; //Not use for index buffers.
		subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
		HRESULT hr = DxSystem::Device->CreateBuffer(&bufferDesc, &subresourceData, mesh.index_buffer.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

}

// アニメーションデータを構築
void Mesh::BuildAnimations(FbxScene* fbxScene, bool isRight_Hand)
{
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbxAnimStackNames;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNames);

	int fbxAnimationCount = fbxAnimStackNames.Size();
	for (int fbxAnimationIndex = 0; fbxAnimationIndex < fbxAnimationCount; ++fbxAnimationIndex)
	{
		animations.emplace_back(Animation());
		Animation& animation = animations.back();

		// アニメーションデータのサンプリング設定
		FbxTime::EMode fbxTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime fbxFrameTime;
		fbxFrameTime.SetTime(0, 0, 0, 1, 0, fbxTimeMode);

		float samplingRate = static_cast<float>(fbxFrameTime.GetFrameRate(fbxTimeMode));
		float samplingTime = 1.0f / samplingRate;

		FbxString* fbxAnimStackName = fbxAnimStackNames.GetAt(fbxAnimationIndex);
		FbxAnimStack* fbxAnimStack = fbxScene->FindMember<FbxAnimStack>(fbxAnimStackName->Buffer());
		animation.name = fbxAnimStackName->Buffer();

		// 再生するアニメーションを指定する。
		fbxScene->SetCurrentAnimationStack(fbxAnimStack);

		// アニメーションの再生開始時間と再生終了時間を取得する
		FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName->Buffer());
		FbxTime fbxStartTime = fbxTakeInfo->mLocalTimeSpan.GetStart();
		FbxTime fbxEndTime = fbxTakeInfo->mLocalTimeSpan.GetStop();

		// 抽出するデータは60フレーム基準でサンプリングする
		FbxTime fbxSamplingStep;
		fbxSamplingStep.SetTime(0, 0, 1, 0, 0, fbxTimeMode);
		fbxSamplingStep = static_cast<FbxLongLong>(fbxSamplingStep.Get() * samplingTime);

		int startFrame = static_cast<int>(fbxStartTime.Get() / fbxSamplingStep.Get());
		int endFrame = static_cast<int>(fbxEndTime.Get() / fbxSamplingStep.Get());
		int frameCount = static_cast<int>((fbxEndTime.Get() - fbxStartTime.Get()) / fbxSamplingStep.Get());

		// アニメーションの対象となるノードを列挙する
		std::vector<FbxNode*> fbxNodes;
		FbxNode* fbxRootNode = fbxScene->GetRootNode();
		for (Node& node : nodes)
		{
			FbxNode* fbxNode = fbxRootNode->FindChild(node.name.c_str());
			fbxNodes.emplace_back(fbxNode);
		}

		// アニメーションデータを抽出する
		animation.secondsLength = frameCount * samplingTime;
		animation.keyframes.resize(frameCount + 1);

		float seconds = 0.0f;
		Keyframe* keyframe = animation.keyframes.data();
		size_t fbxNodeCount = fbxNodes.size();
		FbxTime fbxCurrentTime = fbxStartTime;
		for (FbxTime fbxCurrentTime = fbxStartTime; fbxCurrentTime < fbxEndTime; fbxCurrentTime += fbxSamplingStep, ++keyframe)
		{
			// キーフレーム毎の姿勢データを取り出す。
			keyframe->seconds = seconds;
			keyframe->nodeKeys.resize(fbxNodeCount);
			for (size_t fbxNodeIndex = 0; fbxNodeIndex < fbxNodeCount; ++fbxNodeIndex)
			{
				NodeKeyData& keyData = keyframe->nodeKeys.at(fbxNodeIndex);
				FbxNode* fbxNode = fbxNodes.at(fbxNodeIndex);
				if (fbxNode == nullptr)
				{
					// アニメーション対象のノードがなかったのでダミーデータを設定
					Node& node = nodes.at(fbxNodeIndex);
					keyData.scale = node.scale;
					keyData.rotation = node.rotation;
					keyData.position = node.position;
				}
				else if (fbxNodeIndex == rootMotionNodeIndex)
				{
					// ルートモーションは無視する
					Node& node = nodes.at(fbxNodeIndex);
					keyData.scale = DirectX::XMFLOAT3(1, 1, 1);
					keyData.rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
					keyData.position = DirectX::XMFLOAT3(0, 0, 0);
				}
				else
				{
					// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
					const FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform(fbxCurrentTime);

					keyData.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
					keyData.rotation = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
					keyData.position = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
					///*
					if (isRight_Hand)
					{
						//keyData.scale.z *= -1;
						//keyData.position.z *= -1;
						//keyData.rotation.x *= -1;
						//keyData.rotation.y *= -1;
					}//*/
				}
			}
			seconds += samplingTime;
		}
	}

	// 後始末
	for (int i = 0; i < fbxAnimationCount; i++)
	{
		delete fbxAnimStackNames[i];
	}
}

// ノードインデックスを取得する
int Mesh::FindNodeIndex(const char* name)
{
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

void Mesh::GetTexture(const FbxSurfaceMaterial* fbx_mat, const char* fbx_tex_type, shared_ptr<Material> mat, Texture::Texture_Type tex_type)
{
	const FbxProperty property = fbx_mat->FindProperty(fbx_tex_type);
	if (property.IsValid())
	{
		const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();
		if (number_of_textures)
		{
			const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
			if (file_texture)
			{
				//画像読み込み
				const char* filename = file_texture->GetRelativeFileName();
				mat->Set_Texture(tex_type, file_path, filename);
			}
		}
		else
		{
			if (tex_type == Texture::Texture_Type::Main)
			{
				mat->Set_Texture(tex_type, "Default_Resource\\Image\\", "Default_Texture.png");
			}
			else if (tex_type == Texture::Texture_Type::Normal)
			{
				mat->Set_Texture(tex_type, "Default_Resource\\Image\\", "Default_NormalMap.png");
			}
		}
	}
}