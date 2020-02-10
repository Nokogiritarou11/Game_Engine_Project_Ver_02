#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

class Shader
{
protected:
	ComPtr<ID3D11VertexShader>		VS = nullptr; // ���_�V�F�[�_
	ComPtr<ID3D11PixelShader>		PS = nullptr; // �s�N�Z���V�F�[�_
	ComPtr<ID3D11GeometryShader>	GS = nullptr; // �W�I���g���V�F�[�_
	ComPtr<ID3D11HullShader>		HS = nullptr; //�@�n���V�F�[�_
	ComPtr<ID3D11DomainShader>		DS = nullptr;//�@�h���C���l�[���V�F�[�_


	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	struct set_of_vertex_shader_and_input_layout
	{
		set_of_vertex_shader_and_input_layout(ID3D11VertexShader* vertex_shader, ID3D11InputLayout* input_layout) : vertex_shader(vertex_shader), input_layout(input_layout) {}
		ID3D11VertexShader* vertex_shader;
		ID3D11InputLayout* input_layout;
	};

	static std::unordered_map<std::wstring, set_of_vertex_shader_and_input_layout> vertex_cache;
	static std::unordered_map<std::wstring, ID3D11PixelShader*> pixel_cache;
public:

	ComPtr<ID3D11InputLayout>		VertexLayout;

	Shader() { /*ZeroMemory(this, sizeof(Shader));*/ }
	virtual ~Shader();

	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, bool UI_Material);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);

	void Activate();
};