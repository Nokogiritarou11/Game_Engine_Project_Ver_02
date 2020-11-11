#pragma once
#include "Renderer.h"

class Line_Renderer
{
public:

	static ComPtr <ID3D11Buffer> ConstantBuffer_CbMesh;  //�R���X�^���g�o�b�t�@(���b�V���f�[�^)
	static ComPtr <ID3D11Buffer> ConstantBuffer_CbColor; //�R���X�^���g�o�b�t�@(�J���[)

	Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

private:
	void Initialize(std::shared_ptr<GameObject> obj);
	void Render(Matrix V, Matrix P);
};