#pragma once
#include "DxSystem.h"
#include "Component.h"
#include "Texture.h"
#include "Material.h"
#include "Camera.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Original_Math.h"
#include <string>
#include <wrl.h>
#include <memory>
#include <vector>
#include <stdio.h>
#include <locale.h>

class Renderer : public Component
{
public:
	void SetEnabled(bool value); //表示するか
	bool enableSelf();			 //現在アクティブか

	std::vector<std::shared_ptr<Material>> material; //使用するマテリアル

protected:
	friend class View_Texture;
	friend class Render_Manager;
	bool IsCalled = false;
	bool Disable_flg = false;
	static const Matrix CorrectionMatrix;

private:
	bool CanMultiple() override { return false; };

	bool enabled = true;
	bool enabled_old = false;

	static int Set_BlendState;
	static int Set_RasterizerState;
	static int Set_DepthStencilState;

	friend class View_Texture;
	virtual void Render(Matrix V, Matrix P) {};
	virtual void Render_Shadow(Matrix V, Matrix P) {};

	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(cereal::base_class<Component>(this), enabled);
	}
};

CEREAL_REGISTER_TYPE(Renderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Component, Renderer)