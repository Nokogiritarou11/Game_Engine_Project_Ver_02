#pragma once
#include "Material.h"
#include "Mesh.h"
#include <vector>
#include "cereal/cereal.hpp"
#include "cereal/access.hpp"
#include "cereal/archives/binary.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/memory.hpp"

class SkyBox
{
public:
	SkyBox();

	void Render(Matrix V, Matrix P);

private:
	std::shared_ptr<Material> material;
	std::shared_ptr<Mesh> mesh_data;

	struct CbSkyBox
	{
		Matrix world;
		Vector4 color;
	};

	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer_CbSkyBox;		//コンスタントバッファ
};