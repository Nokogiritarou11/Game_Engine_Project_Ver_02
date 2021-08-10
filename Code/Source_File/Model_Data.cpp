#include "Model_Data.h"
#include "Material.h"
#include <locale.h>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <assert.h>
#include "DxSystem.h"
#include "Material.h"
using namespace std;
using namespace fbxsdk;
using namespace BeastEngine;
using namespace DirectX;

// FbxDouble2 �� XMFLOAT2
inline DirectX::XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbxValue)
{
	return DirectX::XMFLOAT2(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1])
	);
}

// FbxDouble3 �� XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 �� XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2])
	);
}

// FbxDouble4 �� XMFLOAT4
inline DirectX::XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbxValue)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(fbxValue[0]),
		static_cast<float>(fbxValue[1]),
		static_cast<float>(fbxValue[2]),
		static_cast<float>(fbxValue[3])
	);
}

// FbxDouble4 �� XMFLOAT4
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

inline Vector3 Vector_Min(Vector3 a, Vector3 b)
{
	return Vector3(min(a.x, b.x), min(a.y, b.y), min(a.y, b.y));
}
inline Vector3 Vector_Max(Vector3 a, Vector3 b)
{
	return Vector3(max(a.x, b.x), max(a.y, b.y), max(a.y, b.y));
}

shared_ptr<Model_Data> Model_Data::Load_Model(const string& file_path, const string& fbx_filename)
{
	shared_ptr<Model_Data> model_ptr = make_shared<Model_Data>();
	model_ptr->name = fbx_filename;
	model_ptr->file_path = file_path;

	const string fullpath = file_path + fbx_filename + ".fbx";

	FbxManager* fbxManager = FbxManager::Create();

	// FBX�ɑ΂�����o�͂��`����
	FbxIOSettings* fbxIOS = FbxIOSettings::Create(fbxManager, IOSROOT);	// ���ʂȗ��R���Ȃ�����IOSROOT���w��
	fbxManager->SetIOSettings(fbxIOS);

	// �C���|�[�^�𐶐�
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool result = fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings());	// -1�Ńt�@�C���t�H�[�}�b�g��������
	_ASSERT_EXPR_A(result, "FbxImporter::Initialize() : Failed!!\n");

	// Scene�I�u�W�F�N�g��FBX�t�@�C�����̏��𗬂�����
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "scene");
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();	// �V�[���𗬂����񂾂�Importer�͉������OK

							// �W�I���g�����O�p�`�����Ă���
	FbxGeometryConverter fbxGeometryConverter(fbxManager);
	fbxGeometryConverter.Triangulate(fbxScene, true);
	fbxGeometryConverter.RemoveBadPolygonsFromMeshes(fbxScene);

#if 0
	// DirectX���W�n�֕ϊ�
	FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != FbxAxisSystem::DirectX)
	{
		FbxAxisSystem::DirectX.ConvertScene(fbxScene);
	}
#elif 0
	// OpenGL���W�n�֕ϊ�
	FbxAxisSystem sceneAxisSystem = fbxScene->GetGlobalSettings().GetAxisSystem();
	if (sceneAxisSystem != FbxAxisSystem::OpenGL)
	{
		FbxAxisSystem::OpenGL.ConvertScene(fbxScene);
	}
#endif
	// �P�ʌn�̓���B
	FbxSystemUnit SceneSystemUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
	// �Z���`���[�^�[�P�ʂɃR���o�[�g����B
	if (SceneSystemUnit.GetScaleFactor() != 1.0f)
	{
		FbxSystemUnit::cm.ConvertScene(fbxScene);
	}

	// �f�B���N�g���p�X�擾
	char dirname[256];
	::_splitpath_s(fullpath.c_str(), nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

	// ���f���\�z
	FbxNode* fbxRootNode = fbxScene->GetRootNode();
	model_ptr->BuildNodes(fbxRootNode, -1);
	model_ptr->BuildMeshes(fbxRootNode);

	// �A�j���[�V�����\�z
	model_ptr->BuildAnimations(fbxScene);

	// �}�l�[�W�����
	fbxManager->Destroy();		// �֘A���邷�ׂẴI�u�W�F�N�g����������

	return model_ptr;
}

// FBX�m�[�h���ċA�I�ɒH���ăf�[�^���\�z����
void Model_Data::BuildNodes(FbxNode* fbxNode, int parentNodeIndex)
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
			BuildNode(fbxNode, parentNodeIndex);
			break;
	}

	// �ċA�I�Ɏq�m�[�h����������
	parentNodeIndex = static_cast<int>(bones.size() - 1);
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildNodes(fbxNode->GetChild(i), parentNodeIndex);
	}
}

// FBX�m�[�h����m�[�h�f�[�^���\�z����
void Model_Data::BuildNode(FbxNode* fbxNode, int parentNodeIndex)
{
	Skeleton node;
	node.name = fbxNode->GetName();
	node.parentIndex = parentNodeIndex;

	bones.emplace_back(node);
}

// FBX�m�[�h���ċA�I�ɒH���ă��b�V���f�[�^���\�z����
void Model_Data::BuildMeshes(FbxNode* fbxNode)
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
			BuildMesh(fbxNode, static_cast<FbxMesh*>(fbxNodeAttribute));
			break;
	}

	// �ċA�I�Ɏq�m�[�h����������
	for (int i = 0; i < fbxNode->GetChildCount(); ++i)
	{
		BuildMeshes(fbxNode->GetChild(i));
	}
}

// FBX���b�V�����烁�b�V���f�[�^���\�z����
void Model_Data::BuildMesh(FbxNode* fbxNode, FbxMesh* fbxMesh)
{
	int fbxControlPointsCount = fbxMesh->GetControlPointsCount();
	//int fbxPolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	//const int* fbxPolygonVertices = fbxMesh->GetPolygonVertices();

	int fbxMaterialCount = fbxNode->GetMaterialCount();
	int fbxPolygonCount = fbxMesh->GetPolygonCount();

	meshes.emplace_back(make_shared<Mesh>());
	shared_ptr<Mesh> mesh = meshes.back();
	mesh->name = fbxNode->GetName();
	mesh->subsets.resize(fbxMaterialCount > 0 ? fbxMaterialCount : 1);

	// �T�u�Z�b�g�̃}�e���A���ݒ�
	for (int index_of_material = 0; index_of_material < fbxMaterialCount; ++index_of_material)
	{
		Mesh::subset& subset = mesh->subsets.at(index_of_material); // UNIT.18
		const FbxSurfaceMaterial* surface_material = fbxNode->GetMaterial(index_of_material);
		string material_name = surface_material->GetName();

		string new_mat_path = file_path + material_name + ".mat";
		mesh->default_material_passes.push_back(new_mat_path);

		subset.material_ID = mesh->default_material_passes.size() - 1;

		bool cashed = false;
		for (u_int i = 0; i < default_material_passes.size(); i++)
		{
			if (default_material_passes[i] == new_mat_path)
			{
				cashed = true;
				break;
			}
		}
		if (cashed) continue;

		shared_ptr<Material> mat = Material::Create("Shader\\Standard_Shader_VS.hlsl", "Shader\\Standard_Shader_PS.hlsl");
		mat->name = material_name;

		//Main(Diffuse)Texture
		GetTexture(surface_material, FbxSurfaceMaterial::sDiffuse, mat);
		//SpecularTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sSpecular, mat);
		//NormalTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sNormalMap, mat);
		//HeightTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sBump, mat);
		//EmissionTexture
		GetTexture(surface_material, FbxSurfaceMaterial::sEmissive, mat);

		mat->Save(file_path);
		default_material_passes.push_back(new_mat_path);
	}

	// �T�u�Z�b�g�̒��_�C���f�b�N�X�͈͐ݒ�
	if (fbxMaterialCount > 0)
	{
		for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
		{
			int fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
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

	// ���_�e���̓f�[�^
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
	// ���_�e���̓f�[�^�𒊏o����
	vector<BoneInfluence> boneInfluences;
	{
		boneInfluences.resize(fbxControlPointsCount);

		FbxAMatrix fbxGeometricTransform(
			fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricScaling(FbxNode::eSourcePivot)
		);
		// �X�L�j���O�ɕK�v�ȏ����擾����
		int fbxDeformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbxDeformerIndex = 0; fbxDeformerIndex < fbxDeformerCount; ++fbxDeformerIndex)
		{
			FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(fbxDeformerIndex, FbxDeformer::eSkin));

			int fbxClusterCount = fbxSkin->GetClusterCount();
			for (int fbxClusterIndex = 0; fbxClusterIndex < fbxClusterCount; ++fbxClusterIndex)
			{
				FbxCluster* fbxCluster = fbxSkin->GetCluster(fbxClusterIndex);

				// ���_�e���̓f�[�^�𒊏o����
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

				// �{�[���ϊ��s��p�̋t�s��̌v�Z������
				{
					// ���b�V����Ԃ��烏�[���h��Ԃւ̕ϊ��s��
					FbxAMatrix fbxMeshSpaceTransform;
					fbxCluster->GetTransformMatrix(fbxMeshSpaceTransform);

					// �{�[����Ԃ��烏�[���h��Ԃւ̕ϊ��s��
					FbxAMatrix fbxBoneSpaceTransform;
					fbxCluster->GetTransformLinkMatrix(fbxBoneSpaceTransform);

					// �{�[���t�s����v�Z����
					FbxAMatrix fbxInverseTransform = fbxBoneSpaceTransform.Inverse() * fbxMeshSpaceTransform * fbxGeometricTransform;

					Matrix inverseTransform = FbxAMatrixToFloat4x4(fbxInverseTransform);
					mesh->inverse_matrixes.emplace_back(inverseTransform);

					int nodeIndex = FindNodeIndex(fbxCluster->GetLink()->GetName());
					mesh->nodeIndices.emplace_back(nodeIndex);
				}
			}
		}
		if (fbxDeformerCount == 0)
		{
			FbxAMatrix localMatrix = fbxNode->EvaluateLocalTransform();
			Matrix inverseTransform = FbxAMatrixToFloat4x4(localMatrix);
			mesh->inverse_matrixes.emplace_back(inverseTransform);
		}
	}

	// UV�Z�b�g��
	FbxStringList fbxUVSetNames;
	fbxMesh->GetUVSetNames(fbxUVSetNames);

	// ���_�f�[�^
	vector<Mesh::vertex>& vertices = mesh->vertices;
	vector<u_int>& indices = mesh->indices;
	vertices.resize(fbxPolygonCount * 3);
	indices.resize(fbxPolygonCount * 3);

	int vertexCount = 0;
	const FbxVector4* fbxControlPoints = fbxMesh->GetControlPoints();
	bool No_Tanjent = false;
	Vector3 MinV = { FLT_MAX ,FLT_MAX ,FLT_MAX };
	Vector3 MaxV = { -FLT_MAX ,-FLT_MAX ,-FLT_MAX };
	for (int fbxPolygonIndex = 0; fbxPolygonIndex < fbxPolygonCount; ++fbxPolygonIndex)
	{
		// �|���S���ɓK�p����Ă���}�e���A���C���f�b�N�X���擾����
		int fbxMaterialIndex = 0;
		if (fbxMaterialCount > 0)
		{
			fbxMaterialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(fbxPolygonIndex);
		}

		Mesh::subset& subset = mesh->subsets.at(fbxMaterialIndex);
		const int indexOffset = subset.index_start + subset.index_count;

		for (int fbxVertexIndex = 0; fbxVertexIndex < 3; ++fbxVertexIndex)
		{
			Mesh::vertex vertex;

			int fbxControlPointIndex = fbxMesh->GetPolygonVertex(fbxPolygonIndex, fbxVertexIndex);
			// Position
			{
				vertex.position = FbxDouble4ToFloat3(fbxControlPoints[fbxControlPointIndex]);
				MinV = Vector_Min(vertex.position, MinV);
				MaxV = Vector_Max(vertex.position, MaxV);
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
							index_of_vertex = fbxPolygonIndex * 3 + fbxVertexIndex;
							break;
						case FbxGeometryElement::EReferenceMode::eIndexToDirect:
							index_of_vertex = geometry_element_tangent->GetIndexArray().GetAt(fbxControlPointIndex);
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

			DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r));
			DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r));

			for (Mesh::vertex* p : v)
			{
				DirectX::XMVECTOR N = DirectX::XMLoadFloat3(&p->normal);
				DirectX::XMStoreFloat3(&p->tangent, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(T, DirectX::XMVectorMultiply(N, DirectX::XMVector3Dot(T, N)))));

				if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Cross(N, T), B)) < 0)
				{
					p->tangent.x *= -1;
					p->tangent.y *= -1;
					p->tangent.z *= -1;
				}
			}
		}
	}

	mesh->boundingbox.Set_size(MaxV - MinV);

	// ���_�o�b�t�@
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Mesh::vertex) * vertices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
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

	// �C���f�b�N�X�o�b�t�@
	{
		/*
		vector<u_int> inverse_indices;
		for (int i = 0; i < indices.size() / 3; ++i)
		{
			inverse_indices.push_back(i * 3 + 2);
			inverse_indices.push_back(i * 3 + 1);
			inverse_indices.push_back(i * 3);
		}
		*/
		D3D11_BUFFER_DESC bufferDesc = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * indices.size());
		//bufferDesc.ByteWidth = static_cast<UINT>(sizeof(u_int) * inverse_indices.size());
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = &indices[0];
		//subresourceData.pSysMem = &inverse_indices[0];
		subresourceData.SysMemPitch = 0; //Not use for index buffers.
		subresourceData.SysMemSlicePitch = 0; //Not use for index buffers.
		HRESULT hr = DxSystem::device->CreateBuffer(&bufferDesc, &subresourceData, mesh->index_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

}

// �A�j���[�V�����f�[�^���\�z
void Model_Data::BuildAnimations(FbxScene* fbxScene)
{
	// ���ׂẴA�j���[�V���������擾
	FbxArray<FbxString*> fbxAnimStackNames;
	fbxScene->FillAnimStackNameArray(fbxAnimStackNames);

	int fbxAnimationCount = fbxAnimStackNames.Size();
	for (int fbxAnimationIndex = 0; fbxAnimationIndex < fbxAnimationCount; ++fbxAnimationIndex)
	{
		animations.emplace_back(Animation());
		Animation& animation = animations.back();

		// �A�j���[�V�����f�[�^�̃T���v�����O�ݒ�
		FbxTime::EMode fbxTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime fbxFrameTime;
		fbxFrameTime.SetTime(0, 0, 0, 1, 0, fbxTimeMode);

		float samplingRate = static_cast<float>(fbxFrameTime.GetFrameRate(fbxTimeMode));
		float samplingTime = 1.0f / samplingRate;

		FbxString* fbxAnimStackName = fbxAnimStackNames.GetAt(fbxAnimationIndex);
		FbxAnimStack* fbxAnimStack = fbxScene->FindMember<FbxAnimStack>(fbxAnimStackName->Buffer());
		animation.name = fbxAnimStackName->Buffer();

		// �Đ�����A�j���[�V�������w�肷��B
		fbxScene->SetCurrentAnimationStack(fbxAnimStack);

		// �A�j���[�V�����̍Đ��J�n���ԂƍĐ��I�����Ԃ��擾����
		FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName->Buffer());
		FbxTime fbxStartTime = fbxTakeInfo->mLocalTimeSpan.GetStart();
		FbxTime fbxEndTime = fbxTakeInfo->mLocalTimeSpan.GetStop();

		// ���o����f�[�^��60�t���[����ŃT���v�����O����
		FbxTime fbxSamplingStep;
		fbxSamplingStep.SetTime(0, 0, 1, 0, 0, fbxTimeMode);
		fbxSamplingStep = static_cast<FbxLongLong>(fbxSamplingStep.Get() * samplingTime);

		int startFrame = static_cast<int>(fbxStartTime.Get() / fbxSamplingStep.Get());
		int endFrame = static_cast<int>(fbxEndTime.Get() / fbxSamplingStep.Get());
		int frameCount = static_cast<int>((fbxEndTime.Get() - fbxStartTime.Get()) / fbxSamplingStep.Get());

		// �A�j���[�V�����̑ΏۂƂȂ�m�[�h(�{�[��)��񋓂���
		vector<FbxNode*> fbxNodes;
		FbxNode* fbxRootNode = fbxScene->GetRootNode();
		for (Skeleton& node : bones)
		{
			FbxNode* fbxNode = fbxRootNode->FindChild(node.name.c_str());
			fbxNodes.emplace_back(fbxNode);
		}

		// �A�j���[�V�����f�[�^�𒊏o����
		animation.secondsLength = frameCount * samplingTime;
		animation.keyframes.resize(frameCount + 1);

		float seconds = 0.0f;
		Keyframe* keyframe = animation.keyframes.data();
		size_t fbxNodeCount = fbxNodes.size();
		FbxTime fbxCurrentTime = fbxStartTime;
		for (FbxTime fbxCurrentTime = fbxStartTime; fbxCurrentTime <= fbxEndTime; fbxCurrentTime += fbxSamplingStep, ++keyframe)
		{
			// �L�[�t���[�����̎p���f�[�^�����o���B
			keyframe->seconds = seconds;
			if (keyframe == &animation.keyframes.back()) keyframe->seconds = animation.secondsLength;
			keyframe->nodeKeys.resize(fbxNodeCount);
			for (size_t fbxNodeIndex = 0; fbxNodeIndex < fbxNodeCount; ++fbxNodeIndex)
			{
				Key_Pose& keyData = keyframe->nodeKeys.at(fbxNodeIndex);
				FbxNode* fbxNode = fbxNodes.at(fbxNodeIndex);
				if (fbxNode == nullptr)
				{
					// �A�j���[�V�����Ώۂ̃m�[�h���Ȃ������̂Ń_�~�[�f�[�^��ݒ�
					Skeleton& node = bones.at(fbxNodeIndex);
					keyData.scale = node.scale;
					keyData.rotation = node.rotation;
					keyData.position = node.position;
				}
				else if (fbxNodeIndex == rootMotionNodeIndex)
				{
					// ���[�g���[�V�����͖�������
					Skeleton& node = bones.at(fbxNodeIndex);
					keyData.scale = DirectX::XMFLOAT3(1, 1, 1);
					keyData.rotation = DirectX::XMFLOAT4(0, 0, 0, 1);
					keyData.position = DirectX::XMFLOAT3(0, 0, 0);
				}
				else
				{
					// �w�莞�Ԃ̃��[�J���s�񂩂�X�P�[���l�A��]�l�A�ړ��l�����o���B
					const FbxAMatrix& fbxLocalTransform = fbxNode->EvaluateLocalTransform(fbxCurrentTime);

					keyData.scale = FbxDouble4ToFloat3(fbxLocalTransform.GetS());
					keyData.rotation = FbxDouble4ToFloat4(fbxLocalTransform.GetQ());
					keyData.position = FbxDouble4ToFloat3(fbxLocalTransform.GetT());
				}
			}
			seconds += samplingTime;
		}
	}

	// ��n��
	for (int i = 0; i < fbxAnimationCount; i++)
	{
		delete fbxAnimStackNames[i];
	}
}

// �m�[�h�C���f�b�N�X���擾����
int Model_Data::FindNodeIndex(const char* name)
{
	for (size_t i = 0; i < bones.size(); ++i)
	{
		if (bones[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

void Model_Data::GetTexture(const FbxSurfaceMaterial* fbx_mat, const char* fbx_tex_type, shared_ptr<Material> mat)
{
	const FbxProperty property = fbx_mat->FindProperty(fbx_tex_type);
	if (property.IsValid())
	{
		const int number_of_textures = property.GetSrcObjectCount<FbxFileTexture>();

		Material::Texture_Type type;
		if (fbx_tex_type == FbxSurfaceMaterial::sDiffuse)
		{
			type = Material::Texture_Type::Main;
		}
		else if (fbx_tex_type == FbxSurfaceMaterial::sSpecular)
		{
			type = Material::Texture_Type::Specular;
		}
		else if (fbx_tex_type == FbxSurfaceMaterial::sNormalMap)
		{
			type = Material::Texture_Type::Normal;
		}
		else if (fbx_tex_type == FbxSurfaceMaterial::sBump)
		{
			type = Material::Texture_Type::Height;
		}
		else if (fbx_tex_type == FbxSurfaceMaterial::sEmissive)
		{
			type = Material::Texture_Type::Emission;
		}

		if (number_of_textures)
		{
			const FbxFileTexture* file_texture = property.GetSrcObject<FbxFileTexture>();
			if (file_texture)
			{
				//�摜�ǂݍ���
				const char* filename = file_texture->GetRelativeFileName();
				mat->Set_Texture(type, file_path, filename);
			}
		}
		else
		{
			if (type == Material::Texture_Type::Main)
			{
				mat->Set_Texture(type, "Default_Resource\\Image\\", "Default_Texture.png");
			}
			else if (type == Material::Texture_Type::Normal)
			{
				mat->Set_Texture(type, "Default_Resource\\Image\\", "Default_NormalMap.png");
			}
		}
	}
}