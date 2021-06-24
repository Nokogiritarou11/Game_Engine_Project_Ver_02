#pragma once
#include "Bounds.h"
#include "Object.h"
#include <unordered_map>
#include <vector>
#include <wrl.h>

namespace BeastEngine
{
	class Mesh : public Object
	{
	public:
		struct vertex
		{
			BeastEngine::Vector3 position;
			BeastEngine::Vector3 normal;
			BeastEngine::Vector3 tangent;
			BeastEngine::Vector2 texcoord;
			float   bone_weights[4] = { 1, 0, 0, 0 };
			int     bone_indices[4] = {};
		};

		struct subset
		{
			u_int index_start = 0; // start number of index buffer
			u_int index_count = 0; // number of vertices (indices)
			BeastEngine::Vector4 color;
			u_int material_ID;
		};

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		std::vector<vertex> vertices;
		std::vector<u_int> indices;

		std::vector<subset>	subsets;
		std::vector<std::string> default_material_passes;

		BeastEngine::Bounds boundingbox;
		BeastEngine::Matrix inverseTransform;

		std::string name;
		std::string file_path;

	private:
	};
}