#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <unordered_map>
#include <wrl.h>
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
using Microsoft::WRL::ComPtr;

class Shader
{
protected:
	ComPtr<ID3D11VertexShader>		VS = nullptr; // 頂点シェーダ
	ComPtr<ID3D11PixelShader>		PS = nullptr; // ピクセルシェーダ
	ComPtr<ID3D11GeometryShader>	GS = nullptr; // ジオメトリシェーダ
	ComPtr<ID3D11HullShader>		HS = nullptr; // ハルシェーダ
	ComPtr<ID3D11DomainShader>		DS = nullptr; // ドメインネームシェーダ

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

	enum Render_Type
	{
		Opaque,
		CutOut,
		Fade,
		Transparent,
		Overlay
	};

	ComPtr<ID3D11InputLayout>		VertexLayout;

	Shader() { /*ZeroMemory(this, sizeof(Shader));*/ }
	virtual ~Shader();

	bool Create_VS(WCHAR* filename, LPCSTR VSName);
	bool Create_PS(WCHAR* filename, LPCSTR PSName);

	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);

	void Activate();

	void Activate_VS();
	void Activate_PS();
	void Activate_GS();
	void Activate_HS();
	void Activate_DS();

};