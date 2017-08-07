#pragma once
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler")

#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <wrl\client.h>
#include <d3d11sdklayers.h>
#include <DDSTextureLoader.h>
#include "Camera.h"
#include "Geometrie.h"

using namespace Microsoft::WRL;


class GPU
{
private:

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	ComPtr<IDXGISwapChain> swapChain;
	ComPtr<ID3D11RenderTargetView> RTV;
	ComPtr<ID3D11DepthStencilView> DSV;
	ComPtr<ID3D11Texture2D> backBuffer;
	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11DepthStencilState> dSStateEnabled;
	ComPtr<ID3D11DepthStencilState> dSStateDisabled;

	ComPtr<ID3D11InputLayout> inputLayout;

	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ComPtr<ID3D11GeometryShader> geometryShader;

	ComPtr<ID3D11Buffer> vertexBuffer;


	ComPtr<ID3D11ShaderResourceView> SRV[2];
	ComPtr<ID3D11SamplerState> samplerState;
	ComPtr<ID3D11RasterizerState> rasterState;

	ComPtr<ID3D11Buffer> constantBuffer;
	ComPtr<ID3D11Buffer> gridVertexBuffer;
	ComPtr<ID3D11Buffer> gridIndexBuffer;
	
	ComPtr<ID3D11Buffer> lightBuffer;

	ComPtr<ID3D11Buffer> modelVertexBuffer;
	ComPtr<ID3D11Buffer> modelIndexBuffer;

	ComPtr<ID3D11Buffer> spriteVertexBuffer;
	ComPtr<ID3D11Buffer> spriteIndexBuffer;

	ComPtr<ID3D11VertexShader> uiVertexShader;
	ComPtr<ID3D11PixelShader> uiPixelShader;

	ComPtr<ID3D11InputLayout> uiInputLayout;
	ComPtr<ID3D11Buffer> uiConstantBuffer;

	int gridIndexCount, modelIndexCount, spriteIndexCount;

	HRESULT HR;
	HWND hwnd;
	int altezza, larghezza;
	float aspectRatio;

	XMMATRIX projection;
	XMMATRIX world;
	XMMATRIX ortho;

	Camera camera;
	Geometrie geo;

public:

	GPU(int,int,HWND &);
	~GPU();

	void Inizializza();
	void Renderizza();
	void Update();

	void Clear();
	void CaricaShader();
	void CaricaTexture();
	void Draw(int);
	void setPipeline();
	void setBuffer();
	void setBuffer2();
	void setBuffer3();
	void Carica3D();
	void Aggiorna3D();
	void CreaGriglia();
	void CaricaModello();
	void CaricaLuci();
	void ZBufferOn();
	void ZBufferOff();
	void CaricaSprite();
};

