#include "GPU.h"

struct Matrici
{
	XMMATRIX projection;
	XMMATRIX view;
	XMMATRIX world;

	XMFLOAT3 cameraPosition;
};
struct uiMatrici
{
	XMMATRIX ortho;
	XMMATRIX view;
	XMMATRIX world;
};
struct Luci
{
	XMFLOAT4 AmbientColor;
	XMFLOAT4 lightColor;
	XMFLOAT4 lightDirection;
	XMFLOAT4 SpecularColor;
	float SpecularPower;
};

GPU::GPU(int width,int height,HWND& hWnd):hwnd(hWnd),altezza(height),larghezza(width),aspectRatio(float(larghezza / altezza))
{

}


GPU::~GPU()
{
}

void GPU::Clear()
{
	float colore[4] = { 0.0f,0.2f,1.0f,1.0f };
	deviceContext->ClearRenderTargetView(RTV.Get(), colore);
	deviceContext->ClearDepthStencilView(DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void GPU::Draw(int n)
{
	deviceContext->DrawIndexed(n, 0, 0);
}
void GPU::setPipeline()
{
	
	
	UINT strides = sizeof(Geometrie::Vertici);
	UINT offsets = 0;
	
	

	deviceContext->IASetVertexBuffers(0,1, gridVertexBuffer.GetAddressOf(), &strides, &offsets);

	deviceContext->IASetIndexBuffer(gridIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetInputLayout(inputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->VSSetShader(vertexShader.Get(), 0, 0);

	ID3D11Buffer* buffers[] = { lightBuffer.Get(),constantBuffer.Get() };
	deviceContext->VSSetConstantBuffers(0, 2, buffers);
	
	//deviceContext->GSSetConstantBuffers(0, 2, buffers);
	//deviceContext->GSSetShader(geometryShader.Get(), 0, 0);

	deviceContext->PSSetConstantBuffers(0, 1, buffers);
	deviceContext->PSSetShaderResources(0, 1, SRV[0].GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, samplerState.GetAddressOf());
	deviceContext->PSSetShader(pixelShader.Get(), 0, 0);
	deviceContext->RSSetState(rasterState.Get());
}
void GPU::setBuffer()
{
	UINT strides = sizeof(Geometrie::Vertici);
	UINT offsets = 0;
	
	deviceContext->IASetInputLayout(inputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, gridVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(gridIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->VSSetShader(vertexShader.Get(), 0, 0);
	deviceContext->PSSetShader(pixelShader.Get(), 0, 0);
	deviceContext->PSSetShaderResources(0, 1, SRV[1].GetAddressOf());

	ID3D11Buffer* buffers[] = { lightBuffer.Get(),constantBuffer.Get() };
	deviceContext->VSSetConstantBuffers(0, 2, buffers);
}
void GPU::setBuffer2()
{
	UINT strides = sizeof(Geometrie::Vertici);
	UINT offsets = 0;
	deviceContext->IASetInputLayout(inputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, modelVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(modelIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->VSSetShader(vertexShader.Get(), 0, 0);
	deviceContext->PSSetShader(pixelShader.Get(), 0, 0);
	deviceContext->PSSetShaderResources(0, 1, SRV[0].GetAddressOf());

	ID3D11Buffer* buffers[] = { lightBuffer.Get(),constantBuffer.Get() };
	deviceContext->VSSetConstantBuffers(0, 2, buffers);
}
void GPU::setBuffer3()
{

	UINT strides = sizeof(Geometrie::uiVertici);
	UINT offsets = 0;
	
	deviceContext->IASetInputLayout(uiInputLayout.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetVertexBuffers(0, 1, spriteVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(spriteIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	deviceContext->VSSetShader(uiVertexShader.Get(), 0, 0);
	deviceContext->PSSetShader(uiPixelShader.Get(), 0, 0);
	deviceContext->PSSetShaderResources(0, 1, SRV[0].GetAddressOf());
	deviceContext->VSSetConstantBuffers(0, 1, uiConstantBuffer.GetAddressOf());
}
void GPU::Renderizza()
{
	ZBufferOn();
	Clear();

	setBuffer();
	Draw(gridIndexCount);

	setBuffer2();
	Draw(modelIndexCount);

	ZBufferOff();
	setBuffer3();
	Draw(spriteIndexCount);

	swapChain->Present(1, 0);
}
void GPU::Update()
{
	Aggiorna3D();
}
void GPU::Inizializza()
{
	UINT deviceFlag = 0;

#if defined (DEBUG) || (_DEBUG)

	deviceFlag |= D3D11_CREATE_DEVICE_DEBUG;

#endif // DEBUG || _DEBUG


	DXGI_SWAP_CHAIN_DESC scDesc;
	memset(&scDesc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

	scDesc.BufferCount = 1;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.Width = larghezza;
	scDesc.BufferDesc.Height = altezza;
	scDesc.BufferDesc.RefreshRate.Numerator = 1;
	scDesc.BufferDesc.RefreshRate.Denominator = 60;
	scDesc.BufferUsage = D3D11_BIND_RENDER_TARGET;
	scDesc.OutputWindow = hwnd;
	scDesc.SampleDesc = { 4,16 };
	scDesc.Windowed = TRUE;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featureLevels[2] = { D3D_FEATURE_LEVEL_11_1,D3D_FEATURE_LEVEL_11_0 };

	HR = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, deviceFlag, featureLevels, 2, D3D11_SDK_VERSION, &scDesc, swapChain.GetAddressOf(), device.GetAddressOf(), 0, deviceContext.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore inizializzazione Device o SwapChain", L"ERRORE", 0);
		return;
	}


	swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

	HR = device->CreateRenderTargetView(backBuffer.Get(), 0, RTV.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore inizializzazione Device o SwapChain", L"ERRORE", 0);
		return;
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilDesc.Width = larghezza;
	depthStencilDesc.Height = altezza;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT; // solo depth 
	depthStencilDesc.SampleDesc = { 4,16 };
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR = device->CreateTexture2D(&depthStencilDesc, 0, depthStencilBuffer.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore inizializzazione Device o SwapChain", L"ERRORE", 0);
		return;
	}

	D3D11_DEPTH_STENCIL_DESC depthEnabledStencilDesc;
	depthEnabledStencilDesc.DepthEnable = true;
	depthEnabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthEnabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthEnabledStencilDesc.StencilEnable = false;

	HR = device->CreateDepthStencilState(&depthEnabledStencilDesc, dSStateEnabled.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore inizializzazione Device o SwapChain", L"ERRORE", 0);
		return;
	}

	// Z buffer eliminato per il 2D (depth enable = false )
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthDisabledStencilDesc.StencilEnable = false;
	

	HR = device->CreateDepthStencilState(&depthDisabledStencilDesc, dSStateDisabled.GetAddressOf());
	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore inizializzazione Device o SwapChain", L"ERRORE", 0);
		return;
	}

	
	HR = device->CreateDepthStencilView(depthStencilBuffer.Get(), 0, DSV.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore inizializzazione Device o SwapChain", L"ERRORE", 0);
		return;
	}

	deviceContext->OMSetRenderTargets(1, RTV.GetAddressOf(), DSV.Get());

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(larghezza);
	vp.Height = static_cast<float>(altezza);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	deviceContext->RSSetViewports(1, &vp);

	
	CaricaShader();
	CreaGriglia();
	CaricaModello();
	CaricaSprite();
	CaricaTexture();
	Carica3D();
	CaricaLuci();
	setPipeline();
}
void GPU::Carica3D()
{
	camera.Render();
	projection = XMMatrixPerspectiveFovLH(3.14F / 3.0F, aspectRatio, 1.0f, 100000.0f);
	world = XMMatrixIdentity();
	ortho = XMMatrixOrthographicLH((float)larghezza, (float)altezza, 1.0f, 100000.0f);

	D3D11_BUFFER_DESC constBDesc;
	ZeroMemory(&constBDesc, sizeof(D3D11_BUFFER_DESC));
	constBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBDesc.ByteWidth = sizeof(Matrici);
	constBDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	
	Matrici data = { projection,camera.GetViewMatrix(),world,camera.GetPosition() };
	
	D3D11_SUBRESOURCE_DATA subRes;
	ZeroMemory(&subRes, sizeof(D3D11_SUBRESOURCE_DATA));
	subRes.pSysMem = &data;

	HR = device->CreateBuffer(&constBDesc, &subRes, constantBuffer.GetAddressOf());
	

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Caricamento Constant Buffer", L"ATTENZIONE", 0);
	}
	
	uiMatrici uiData = { ortho,camera.GetViewMatrix(),(XMMatrixScaling(100.f,100.f,100.f)*XMMatrixTranslation(-500,100,0)) };

	constBDesc.ByteWidth = sizeof(uiMatrici);

	D3D11_SUBRESOURCE_DATA ui_subRes;
	ZeroMemory(&ui_subRes, sizeof(D3D11_SUBRESOURCE_DATA));
	ui_subRes.pSysMem = &uiData;

	HR = device->CreateBuffer(&constBDesc, &ui_subRes, uiConstantBuffer.GetAddressOf());


	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Caricamento uiConstant Buffer", L"ATTENZIONE", 0);
	}
}
void GPU::CaricaLuci()
{
	D3D11_BUFFER_DESC constBDesc;
	ZeroMemory(&constBDesc, sizeof(D3D11_BUFFER_DESC));
	constBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBDesc.ByteWidth = 80;//sizeof(Luci); multiplo di 16 maggiore di 68  e' 80
	constBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	constBDesc.CPUAccessFlags = 0;

	Luci data;
	data.AmbientColor = { 0.0f, 1.0f, 1.0f, 1.0f };
	data.lightColor = { 1.0f,1.0f,0.0f,1.0f };
	data.lightDirection = { 0.0f,-1.0f,-1.0f,0 };
	data.SpecularColor = { 1.0f, 0.5f, 0.0f, 1.0f };
	data.SpecularPower =  25.0f;

	D3D11_SUBRESOURCE_DATA subRes;
	ZeroMemory(&subRes, sizeof(D3D11_SUBRESOURCE_DATA));
	subRes.pSysMem = &data;
	
	HR = device->CreateBuffer(&constBDesc, &subRes, lightBuffer.GetAddressOf());


	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Caricamento Light Buffer", L"ATTENZIONE", 0);
	}
}
float GetHeight(float x, float z)
{ 
	return 0.3f*(z*sinf(0.1f*x) + x*cosf(0.1f*z));
}
XMFLOAT3 GetGridNormal(float x, float z)
{
	// n = (-df/dx, 1, -df/dz)
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.3f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);

	return n;
}
void GPU::CreaGriglia()
{
	Geometrie::MeshData grid;

	//geo.CreateTriangleGrid(160.0f, 160.0f, 256, 256, grid);
	geo.CreateHeightMapGrid("Assets/setup.txt",grid);
	UINT size = grid.Vertices.size();
	gridIndexCount = size;
	UINT* indices = new UINT[gridIndexCount];

	std::vector<Geometrie::Vertici> vertici(size);

	for (UINT i = 0; i < size; i++)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;

		//p.y = 0;//GetHeight(p.x, p.z);

		vertici[i].posizione =  p;
		indices[i] = i;
		// Color the vertex based on its height.
		/*if( p.y < -10.0f )
		{
			// Sandy beach color.
			vertici[i].colore = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
		}
		else if(p.y < 5.0f)
		{
			// Light yellow-green.
			vertici[i].colore = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
		}
		else if(p.y < 12.0f)
		{
			// Dark yellow-green.
			vertici[i].colore = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
		}
		else if(p.y < 20.0f)
		{
			// Dark brown.
			vertici[i].colore = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
		}

		else
		{

			// White snow.
			vertici[i].colore = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		}*/
		vertici[i].normal = grid.Vertices[i].Normal;
		vertici[i].texcoord = grid.Vertices[i].TexC;
		vertici[i].color = grid.Vertices[i].Color;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Geometrie::Vertici)* size;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData;

	vInitData.pSysMem = &vertici[0];

	HR = device->CreateBuffer(&vbd, &vInitData, gridVertexBuffer.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore creazione vertex buffer griglia", L"ERRORE", 0);
		return;
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*gridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;



	D3D11_SUBRESOURCE_DATA iinitData;
	ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));

	iinitData.pSysMem = &indices[0];

	HR = device->CreateBuffer(&ibd, &iinitData, gridIndexBuffer.GetAddressOf());

	if (FAILED(HR))
	{

		MessageBox(hwnd, L"errore creazione index buffer griglia", L"ERRORE", 0);
		return;
	}
}	
void GPU::CaricaModello()
{
	UINT size = 0;
	modelIndexCount = 0;
	
	bool controllo = geo.CreaMesh("Assets/Sfera.dae", true);
	if (!controllo)
	{
		MessageBox(hwnd, L"errore caricamento", L"ERRORE", 0);
		return;
	}

	for (int i = 0; i < geo.meshes.size(); i++)
	{
		size += geo.meshes[i].Vertices.size();
		modelIndexCount += geo.meshes[i].Indices.size();
	}

	std::vector<Geometrie::Vertici> vertici_m(size);
	std::vector<UINT> indici_m(modelIndexCount);

	for (int j = 0; j < geo.meshes.size(); j++)
	{
		for (size_t i = 0; i < geo.meshes[j].Vertices.size(); i++)
		{
			
			vertici_m[i].posizione = geo.meshes[j].Vertices[i].Position;
			vertici_m[i].normal = geo.meshes[j].Vertices[i].Normal;
			vertici_m[i].texcoord = geo.meshes[j].Vertices[i].TexC;
			
		}
		for (size_t i = 0; i < geo.meshes[j].Indices.size(); i++)
		{
			indici_m[i] = geo.meshes[j].Indices[i];
		}
	}
	
	D3D11_BUFFER_DESC mvbd;
	mvbd.Usage = D3D11_USAGE_IMMUTABLE;
	mvbd.ByteWidth = sizeof(Geometrie::Vertici)*size;
	mvbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	mvbd.CPUAccessFlags = 0;
	mvbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA mvInitData;
	mvInitData.pSysMem = &vertici_m[0];

	HR = device->CreateBuffer(&mvbd, &mvInitData, modelVertexBuffer.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore creazione vertex buffer modello", L"ERRORE", 0);
		return;
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*modelIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	
	

	D3D11_SUBRESOURCE_DATA iinitData;
	ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));

	iinitData.pSysMem = &indici_m[0];

	HR = device->CreateBuffer(&ibd,&iinitData, modelIndexBuffer.GetAddressOf());

	if (FAILED(HR))
	{

		MessageBox(hwnd, L"errore creazione index buffer modello", L"ERRORE", 0);
		return;
	}
}
void GPU::Aggiorna3D()
{
	
	camera.setTime();

	//world = XMMatrixScaling(10.0f,10.0f,10.0f);
	
	
	camera.avanti(GetAsyncKeyState(0x57)); // W
	camera.destra(GetAsyncKeyState(0x44)); // D
	camera.sinistra(GetAsyncKeyState(0x41)); // A
	camera.indietro(GetAsyncKeyState(0x53)); // S
	camera.giu(GetAsyncKeyState(VK_DOWN));
	camera.su(GetAsyncKeyState(VK_UP));
	camera.ruotaSu(GetAsyncKeyState(VK_LEFT));
	camera.ruotaGiu(GetAsyncKeyState(VK_RIGHT));

	camera.Render();


	D3D11_MAPPED_SUBRESOURCE mappedResource;
    Matrici* bufPtr = 0;
    if (FAILED(deviceContext->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    { 
		 MessageBox(hwnd,L"errore aggiornamento constant buffer",L"ERRORE", 0);
	}    
	bufPtr = (Matrici*) mappedResource.pData;
	bufPtr->projection = projection;
	bufPtr->view = camera.GetViewMatrix();
	bufPtr->world = world;
	bufPtr->cameraPosition = camera.GetPosition();
    deviceContext->Unmap(constantBuffer.Get(), 0);
	
	//Matrici data = { projection,camera.GetViewMatrix(),world };
	//deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	//deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

}
void GPU::CaricaShader()
{

	ComPtr<ID3DBlob> vsData;
	ComPtr<ID3DBlob> psData;
	ComPtr<ID3DBlob> gsData;

#if defined(_DEBUG)

	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	HR = D3DCompileFromFile(L"LightShader.hlsl", nullptr, nullptr, "VSmain", "vs_5_0", compileFlags, 0, &vsData, nullptr);

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione Vertex", L"ATTENZIONE", 0);
		return;
	}
	HR = D3DCompileFromFile(L"LightShader.hlsl", nullptr, nullptr, "PSmain", "ps_5_0", compileFlags, 0, &psData, nullptr);

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione Pixel", L"ATTENZIONE", 0);
		return;
	}
	HR = D3DCompileFromFile(L"LightShader.hlsl", nullptr, nullptr, "GSmain", "gs_5_0", compileFlags, 0, &gsData,nullptr);
	
	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione Geometry", L"ATTENZIONE", 0);
		return;
	}

	HR = device->CreateVertexShader(vsData->GetBufferPointer(), vsData->GetBufferSize(), 0, &vertexShader);
	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione Vertex", L"ATTENZIONE", 0);
		return;
	}
	HR = device->CreatePixelShader(psData->GetBufferPointer(), psData->GetBufferSize(), 0, &pixelShader);
	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione Pixel", L"ATTENZIONE", 0);
		return;
	}
	HR = device->CreateGeometryShader(gsData->GetBufferPointer(), gsData->GetBufferSize(), 0, &geometryShader);
	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione Geometry", L"ATTENZIONE", 0);
		return;
	}

	D3D11_INPUT_ELEMENT_DESC layout[4] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12 + 8,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "COLOR",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12 + 8 + 12,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	HR = device->CreateInputLayout(layout, ARRAYSIZE(layout), vsData->GetBufferPointer(), vsData->GetBufferSize(), inputLayout.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore  Input layout", L"ATTENZIONE", 0);
	}

	////////////////////////////////////////// UI ///////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	vsData.Reset();
	psData.Reset();

	HR = D3DCompileFromFile(L"uiShader.hlsl", nullptr, nullptr, "VSmain", "vs_5_0", compileFlags, 0, &vsData, nullptr);

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione uiVertex", L"ATTENZIONE", 0);
		return;
	}
	HR = D3DCompileFromFile(L"uiShader.hlsl", nullptr, nullptr, "PSmain", "ps_5_0", compileFlags, 0, &psData, nullptr);

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione uiPixel", L"ATTENZIONE", 0);
		return;
	}
	HR = device->CreateVertexShader(vsData->GetBufferPointer(), vsData->GetBufferSize(), 0, &uiVertexShader);
	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione uiVertex", L"ATTENZIONE", 0);
		return;
	}
	HR = device->CreatePixelShader(psData->GetBufferPointer(), psData->GetBufferSize(), 0, &uiPixelShader);
	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Creazione uiPixel", L"ATTENZIONE", 0);
		return;
	}

	D3D11_INPUT_ELEMENT_DESC ui_layout[2] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	HR = device->CreateInputLayout(ui_layout, ARRAYSIZE(ui_layout), vsData->GetBufferPointer(), vsData->GetBufferSize(), uiInputLayout.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore  Input layout", L"ATTENZIONE", 0);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	

}
void GPU::CaricaTexture()
{
	HR = CreateDDSTextureFromFile(device.Get(), L"Assets/sassolini.dds", 0, SRV[0].GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Texture", L"ATTENZIONE", 0);
	}

	HR = CreateDDSTextureFromFile(device.Get(), L"Assets/grass.dds", 0, SRV[1].GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Texture", L"ATTENZIONE", 0);
	}

	

	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof(D3D11_SAMPLER_DESC));
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MaxAnisotropy = 16;
	sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	
	HR = device->CreateSamplerState(&sd, samplerState.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Caricamento SamplerState", L"ATTENZIONE", 0);
	}

	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC));
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.MultisampleEnable = true;
	HR = device->CreateRasterizerState(&rastDesc, rasterState.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"Errore Caricamento RasterState", L"ATTENZIONE", 0);
	}

}
void GPU::CaricaSprite()
{
	Geometrie::uiVertici ui[4];

	ui[0].posizione = XMFLOAT2(-1, 1); ui[0].texcoord = XMFLOAT2(0, 0);
	ui[1].posizione = XMFLOAT2(-1, 0); ui[1].texcoord = XMFLOAT2(0, 1);
	ui[2].posizione = XMFLOAT2(0, 1); ui[2].texcoord = XMFLOAT2(1, 0);
	ui[3].posizione = XMFLOAT2(0, 0); ui[3].texcoord = XMFLOAT2(1, 1);

	D3D11_BUFFER_DESC bDesc;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(Geometrie::uiVertici)* ARRAYSIZE(ui);
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vInitData;

	vInitData.pSysMem = ui;

	device->CreateBuffer(&bDesc, &vInitData, spriteVertexBuffer.GetAddressOf());

	if (FAILED(HR))
	{
		MessageBox(hwnd, L"errore creazione vertex buffer sprite", L"ERRORE", 0);
		return;
	}

	UINT indici[6];

	indici[0] = 1;
	indici[1] = 0;
	indici[2] = 2;
	indici[3] = 2;
	indici[4] = 3;
	indici[5] = 1;

	spriteIndexCount = ARRAYSIZE(indici);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT)*spriteIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	ZeroMemory(&iinitData, sizeof(D3D11_SUBRESOURCE_DATA));

	iinitData.pSysMem = indici;

	HR = device->CreateBuffer(&ibd, &iinitData, spriteIndexBuffer.GetAddressOf());

	if (FAILED(HR))
	{

		MessageBox(hwnd, L"errore creazione index buffer sprite", L"ERRORE", 0);
		return;
	}
}
void GPU::ZBufferOff()
{
	deviceContext->OMSetDepthStencilState(dSStateDisabled.Get(), 1);
}
void GPU::ZBufferOn()
{
	deviceContext->OMSetDepthStencilState(dSStateEnabled.Get(), 1);
}