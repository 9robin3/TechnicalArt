
#define VSYNC
#define USECONSOLE

#include "stdafx.h"
#include "ShaderBuffers.h"
#include "InputHandler.h"
#include "Camera.h"
#include "Geometry.h"
#include "DDSTextureLoader.h"

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE				g_hInst					= nullptr;  
HWND					g_hWnd					= nullptr;

IDXGISwapChain*         g_SwapChain				= nullptr;
ID3D11RenderTargetView* g_RenderTargetView		= nullptr;
ID3D11Texture2D*        g_DepthStencil			= nullptr;
ID3D11DepthStencilView* g_DepthStencilView		= nullptr;
ID3D11Device*			g_Device				= nullptr;
ID3D11DeviceContext*	g_DeviceContext			= nullptr;
ID3D11RasterizerState*	g_RasterState			= nullptr;

ID3D11RasterizerState*	g_RasterStateSkyDome	= nullptr;

ID3D11InputLayout*		g_InputLayout			= nullptr;
ID3D11VertexShader*		g_VertexShader			= nullptr;
ID3D11PixelShader*		g_PixelShader = nullptr;

//For skydome
ID3D11VertexShader*		g_VertexShaderSkyDome	= nullptr;
ID3D11PixelShader*		g_PixelShaderSkyDome	= nullptr;
ID3D11ShaderResourceView* SkyTexSRV = nullptr;
ID3D11Resource* SkyTex = nullptr;

//For reflection
ID3D11VertexShader*		g_VertexShaderReflection = nullptr;
ID3D11PixelShader*		g_PixelShaderReflection = nullptr;

ID3D11Buffer*			g_MatrixBuffer			= nullptr;
InputHandler*			g_InputHandler			= nullptr;

ID3D11Buffer* camera_buffer						= nullptr;
ID3D11Buffer* material_buffer					= nullptr;

int width, height;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT				Render(float deltaTime);
HRESULT				Update(float deltaTime);
HRESULT				InitDirect3DAndSwapChain(int width, int height);
void				InitRasterizerState();
HRESULT				CreateRenderTargetView();
HRESULT				CreateDepthStencilView(int width, int height);
void				SetViewport(int width, int height);
HRESULT				CreateShadersAndInputLayout();
void				InitShaderBuffers();
void				Release();

//
// object declarations
//
camera_t* camera;
float camera_vel = 5.0f;	// world unit/s
Cube_t* cube;
Cube2_t* cube2;
Triangle_t* triangle;
OBJModel_t* obj;
OBJModel_t* skyDome;
// model-to-world matrices
mat4f MSkyDome;
mat4f Mcube;
mat4f Mcube2;
mat4f Mtriangle;
mat4f quad_matrix;
vec3f cube2_pos;
vec3f triangle_pos;
vec3f quad_pos;
material_t material;
Quad_t* quad;
float angle = 0;			// rad
float angle_vel = fPI / 16;	// rad/s
bool inverse = false;
// world-to-view matrix
mat4f Mview;
// projection matrix
mat4f Mproj;

//
// object initialization
//
void initObjects()
{
	// create camera
	camera = new camera_t(fPI/4,				/*field-of-view*/
						(float)width / height,	/*aspect ratio*/
						0.2f,					/*z-near plane (everything closer will be clipped/removed)*/
						7000.0f                /*z-far plane (everything further will be clipped/removed)*/
						);				
	camera->moveTo({ 0, 2, 30 });
	// create objects
	cube = new Cube_t(g_Device);
	cube2 = new Cube2_t(g_Device);
	triangle = new Triangle_t(g_Device);
	cube2_pos = { 2, 0, 0 };
	triangle_pos = { 6, 0, 0 };
	Mcube = mat4f::rotation(5, 1.0f, 0.0f, 0.0f);
	Mcube2 = mat4f::rotation(5, 1.0f, 0.0f, 0.0f);
	Mtriangle = mat4f::rotation(5, 0.0f, 1.0f, 0.0f);
	quad = new Quad_t(g_Device);
	quad_pos = { 3, 0, 0 };
	
	obj = new OBJModel_t("../../assets/carbody/carbody.obj", g_Device);
	
	//Load skydome obj model
	skyDome = new OBJModel_t("../../assets/sphere/sphere.obj", g_Device);

	//Load the DDS texture for the skydome
	DirectX::CreateDDSTextureFromFile(g_Device, g_DeviceContext, L"../../assets/cubemaps/snowcube1024.dds", &SkyTex, &SkyTexSRV);
}
//
// per-frame, update object
//
void updateObjects(float dt)
{
	
	//D3D11_MAPPED_SUBRESOURCE resource;
	//g_DeviceContext->Map(camera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//camera_light* camera_light_buffer_ = (camera_light*)resource.pData;
	//camera_light_buffer_->camera_pos = { camera->GetPos() };
	//camera_light_buffer_->light_pos = { 5, 5, 5 };

	// basic camera control
	if (g_InputHandler->IsKeyPressed(Keys::W))
		camera->move({ 0.0f, 0.0f, -camera_vel *dt });

    if (g_InputHandler->IsKeyPressed(Keys::S))
	    camera->move({ 0.0f, 0.0f, camera_vel *dt });

	if (g_InputHandler->IsKeyPressed(Keys::D))
		camera->move({ camera_vel *dt, 0.0f, 0.0f });

	if (g_InputHandler->IsKeyPressed(Keys::A))
		camera->move({ -camera_vel *dt, 0.0f, 0.0f });
		
	if (g_InputHandler->IsKeyPressed(Keys::Left))
		camera->SetAngleX(0.0025f);

	if (g_InputHandler->IsKeyPressed(Keys::Right))
		camera->SetAngleX(-0.0025f);

	if (g_InputHandler->IsKeyPressed(Keys::Up))
		camera->SetAngleY(0.0025);

	if (g_InputHandler->IsKeyPressed(Keys::Down))
		camera->SetAngleY(-0.0025);

	angle += angle_vel * dt;
	Mcube = mat4f::rotation(angle, 0.0f, 1.0f, 0.0f);
	Mcube2 = mat4f::translation(cube2_pos);
	Mtriangle = mat4f::translation(triangle_pos);
	quad_matrix = mat4f::translation(quad_pos);
	
	//Set skydome scale
	MSkyDome = mat4f::scaling(100);
}

//
// per frame, render object
//
void renderObjects()
{
	Mview = camera->get_WorldToViewMatrix();
	Mproj = camera->get_ProjectionMatrix();

	g_DeviceContext->VSSetShader(g_VertexShaderSkyDome, nullptr, 0);
	g_DeviceContext->PSSetShader(g_PixelShaderSkyDome, nullptr, 0);
	g_DeviceContext->PSSetShaderResources(5, 1, &SkyTexSRV);
	g_DeviceContext->RSSetState(g_RasterStateSkyDome);

	skyDome->MapMatrixBuffers(g_DeviceContext, g_MatrixBuffer, MSkyDome, Mview, Mproj);
	skyDome->renderObj(g_DeviceContext, material_buffer);

	g_DeviceContext->VSSetShader(g_VertexShaderReflection, nullptr, 0);
	g_DeviceContext->PSSetShader(g_PixelShaderReflection, nullptr, 0);
	g_DeviceContext->RSSetState(g_RasterState);

	//Cube 1
	//cube->MapMatrixBuffers(g_DeviceContext, g_MatrixBuffer, Mcube, Mview, Mproj);
	//cube->MapMaterialBuffers(g_DeviceContext, material_buffer, { 0.5f, 0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0.5f, 0 });
	//cube->render(g_DeviceContext);
	////Cube 2
	//cube2->MapMatrixBuffers(g_DeviceContext, g_MatrixBuffer, Mcube * Mcube2, Mview, Mproj);
	//cube2->MapMaterialBuffers(g_DeviceContext, material_buffer, { 0.4f, 0.8f, 0.7f, 0.2f }, { 0.2f, 0.4f, 0.3f, 0.8f }, { 0.7f, 0.5f, 0.7f, 0 });
	//cube2->render(g_DeviceContext);
	//Triangle
	//triangle->MapMatrixBuffers(g_DeviceContext, g_MatrixBuffer, Mcube * Mcube2 * Mtriangle, Mview, Mproj);
	//triangle->MapMaterialBuffers(g_DeviceContext, material_buffer, { 1.0f, 0.8f, 0.7f, 0.2f }, { 1.0f, 0.4f, 0.3f, 0.8f }, { 0.7f, 0.5f, 0.7f, 0 });
	//triangle->render(g_DeviceContext);
	//Obj

	obj->MapMatrixBuffers(g_DeviceContext, g_MatrixBuffer, Mcube , Mview, Mproj);
	//obj->MapMaterialBuffers(g_DeviceContext, material_buffer, { 0.7f, 0.5f, 0.5f, 0 }, { 0.5f, 0.5f, 0.5f, 0.2f }, { 0.5f, 0.5f, 0.5f, 0 });
	obj->renderObj(g_DeviceContext, material_buffer);
}

//
// object deallocation, at program termination
//
void releaseObjects()
{
	SAFE_DELETE(cube);
	SAFE_DELETE(cube2);
	SAFE_DELETE(triangle);
	SAFE_DELETE(obj);
	SAFE_DELETE(skyDome);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	// load console and redirect some I/O to it
	// note: this has to be done before the win32 window is initialized, or DirectInput will fail miserably
#ifdef USECONSOLE
	AllocConsole();
	freopen("conin$", "r", stdin);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);
#endif

	// init the win32 window
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

#ifdef USECONSOLE
	printf("Win32-window created...\n");
#ifdef _DEBUG
	printf("Running in DEBUG mode\n");
#else
	printf("Running in RELEASE mode\n");
#endif
#endif

	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect( g_hWnd, &rc );
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	if(SUCCEEDED(hr = InitDirect3DAndSwapChain(width, height)))
	{
		InitRasterizerState();

		if (SUCCEEDED(hr = CreateRenderTargetView()) &&
			SUCCEEDED(hr = CreateDepthStencilView(width, height)))
		{
			SetViewport(width, height);

			g_DeviceContext->OMSetRenderTargets( 1, &g_RenderTargetView, g_DepthStencilView );

			if(SUCCEEDED(hr = CreateShadersAndInputLayout()))
			{
				InitShaderBuffers();
				initObjects();
			}
		}
	}

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	g_InputHandler = new InputHandler();
	g_InputHandler->Initialize(hInstance, g_hWnd, width, height);

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			g_InputHandler->Update();
			Update(dt);
			Render(dt);

			prevTimeStamp = currTimeStamp;
		}
	}

	Release();
#ifdef USECONSOLE
	FreeConsole();
#endif
	return (int) msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = nullptr;
	wcex.lpszClassName  = "DA307A_eduRend";
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Adjust and create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, 768, 768 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if(!(g_hWnd = CreateWindow(
							"DA307A_eduRend",
							"DA307A - eduRend",
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							nullptr,
							nullptr,
							hInstance,
							nullptr)))
	{
		return E_FAIL;
	}

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}

HRESULT CompileShader(char* shaderFile, char* pEntrypoint, char* pTarget, D3D10_SHADER_MACRO* pDefines, ID3DBlob** pCompiledShader)
{
	DWORD dwShaderFlags =	D3DCOMPILE_ENABLE_STRICTNESS |
							D3DCOMPILE_IEEE_STRICTNESS;

	std::string shader_code;
	std::ifstream in(shaderFile, std::ios::in | std::ios::binary);
	if(in)
	{
		in.seekg(0, std::ios::end);
		shader_code.resize((UINT)in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&shader_code[0], shader_code.size());
		in.close();
	}

	ID3DBlob* pErrorBlob = nullptr;
	HRESULT hr = D3DCompile(
		shader_code.data(),
		shader_code.size(),
		nullptr,
		pDefines,
		nullptr,
		pEntrypoint,
		pTarget, 
		dwShaderFlags,
		0,
		pCompiledShader,
		&pErrorBlob);

	if (pErrorBlob)
	{
		// output error message
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
#ifdef USECONSOLE
		printf("%s\n", (char*)pErrorBlob->GetBufferPointer());
#endif
		SAFE_RELEASE(pErrorBlob);
	}

    return hr;
}

HRESULT CreateShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	ID3DBlob* pVertexShader = nullptr;
	if (SUCCEEDED(hr = D3DReadFileToBlob(L"VertexShaderDrawTri.cso", &pVertexShader)))
	{
		
		if(SUCCEEDED(hr = g_Device->CreateVertexShader(
			pVertexShader->GetBufferPointer(),
			pVertexShader->GetBufferSize(),
			nullptr,
			&g_VertexShader)))
		{
			D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			//D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
			//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//	{ "TEX", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//};

			hr = g_Device->CreateInputLayout(
						inputDesc,
						ARRAYSIZE(inputDesc),
						pVertexShader->GetBufferPointer(),
						pVertexShader->GetBufferSize(),
						&g_InputLayout);
		}

		SAFE_RELEASE(pVertexShader);
	}
	else
	{
		MessageBoxA(nullptr, "Failed to create vertex shader (check Output window for more info)", 0, 0);
	}

	ID3DBlob* pPixelShader = nullptr;
	if (SUCCEEDED(hr = D3DReadFileToBlob(L"PixelShaderDrawTri.cso", &pPixelShader)))
	{
		hr = g_Device->CreatePixelShader(
			pPixelShader->GetBufferPointer(),
			pPixelShader->GetBufferSize(),
			nullptr,
			&g_PixelShader);

		SAFE_RELEASE(pPixelShader);
	}
	else
	{
		MessageBoxA(nullptr, "Failed to create pixel shader (check Output window for more info)", 0, 0);
	}
	


	//////My own vertex & pixel shaders

	if (SUCCEEDED(hr = D3DReadFileToBlob(L"VertexShaderSkydome.cso", &pVertexShader)))
	{
		hr = g_Device->CreateVertexShader(
			pVertexShader->GetBufferPointer(),
			pVertexShader->GetBufferSize(),
			nullptr,
			&g_VertexShaderSkyDome);

		SAFE_RELEASE(pVertexShader);
	}

	if (SUCCEEDED(hr = D3DReadFileToBlob(L"PixelShaderSkydome.cso", &pPixelShader)))
	{
		hr = g_Device->CreatePixelShader(
			pPixelShader->GetBufferPointer(),
			pPixelShader->GetBufferSize(),
			nullptr,
			&g_PixelShaderSkyDome);

		SAFE_RELEASE(pPixelShader);
	}

	if (SUCCEEDED(hr = D3DReadFileToBlob(L"VertexShaderReflection.cso", &pVertexShader)))
	{
		hr = g_Device->CreateVertexShader(
			pVertexShader->GetBufferPointer(),
			pVertexShader->GetBufferSize(),
			nullptr,
			&g_VertexShaderReflection);

		SAFE_RELEASE(pVertexShader);
	}

	if (SUCCEEDED(hr = D3DReadFileToBlob(L"PixelShaderReflection.cso", &pPixelShader)))
	{
		hr = g_Device->CreatePixelShader(
			pPixelShader->GetBufferPointer(),
			pPixelShader->GetBufferSize(),
			nullptr,
			&g_PixelShaderReflection);

		SAFE_RELEASE(pPixelShader);
	}

	return hr;
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDirect3DAndSwapChain(int width, int height)
{
	D3D_DRIVER_TYPE driverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_REFERENCE };

	DXGI_SWAP_CHAIN_DESC sd;
	memset(&sd, 0, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// If the project is in a debug build, enable the debug layer.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = E_FAIL;
	for( UINT driverTypeIndex = 0; driverTypeIndex < ARRAYSIZE(driverTypes) && FAILED(hr); driverTypeIndex++ )
	{
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			driverTypes[driverTypeIndex],
			nullptr,
			creationFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&g_SwapChain,
			&g_Device,
			&initiatedFeatureLevel,
			&g_DeviceContext);
	}
#ifdef VSYNC
	g_SwapChain-> Present(1, 0);
#endif

	return hr;
}

void InitRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = true;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;

	g_Device->CreateRasterizerState(&rasterizerState, &g_RasterState);

	rasterizerState.FrontCounterClockwise = false;
	g_Device->CreateRasterizerState(&rasterizerState, &g_RasterStateSkyDome);

}

void InitShaderBuffers()
{
	HRESULT hr;

	// Matrix buffer
	D3D11_BUFFER_DESC MatrixBuffer_desc = { 0 };
	MatrixBuffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	MatrixBuffer_desc.ByteWidth = sizeof(MatrixBuffer_t);
	MatrixBuffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MatrixBuffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MatrixBuffer_desc.MiscFlags = 0;
	MatrixBuffer_desc.StructureByteStride = 0;
	ASSERT(hr = g_Device->CreateBuffer(&MatrixBuffer_desc, nullptr, &g_MatrixBuffer));

	// Material buffer
	D3D11_BUFFER_DESC material_buffer_desc = { 0 };
	material_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	material_buffer_desc.ByteWidth = sizeof(material_own);
	material_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	material_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	material_buffer_desc.MiscFlags = 0;
	material_buffer_desc.StructureByteStride = 0;
	ASSERT(hr = g_Device->CreateBuffer(&material_buffer_desc, nullptr, &material_buffer));

	//Camera buffer
	D3D11_BUFFER_DESC camera_buffer_desc = { 0 };
	camera_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	camera_buffer_desc.ByteWidth = sizeof(camera_light);
	camera_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camera_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	camera_buffer_desc.MiscFlags = 0;
	camera_buffer_desc.StructureByteStride = 0;
	ASSERT(hr = g_Device->CreateBuffer(&camera_buffer_desc, nullptr, &camera_buffer));
}

HRESULT CreateRenderTargetView()
{
	HRESULT hr = S_OK;
	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	if(SUCCEEDED(hr = g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&pBackBuffer)))
	{
		hr = g_Device->CreateRenderTargetView( pBackBuffer, nullptr, &g_RenderTargetView );
		SAFE_RELEASE(pBackBuffer);
	}
	return hr;
}

HRESULT CreateDepthStencilView(int width, int height)
{
	HRESULT hr = S_OK;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = g_Device->CreateTexture2D( &descDepth, nullptr, &g_DepthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_Device->CreateDepthStencilView( g_DepthStencil, &descDSV, &g_DepthStencilView );
	
	return hr;
}

void SetViewport(int width, int height)
{
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_DeviceContext->RSSetViewports( 1, &vp );
}

HRESULT Update(float deltaTime)
{
	updateObjects(deltaTime);

	return S_OK;
}

HRESULT Render(float deltaTime)
{
	//clear back buffer, black color
	static float ClearColor[4] = { 0, 0, 0, 1 };
	g_DeviceContext->ClearRenderTargetView( g_RenderTargetView, ClearColor );
	
	//clear depth buffer
	g_DeviceContext->ClearDepthStencilView( g_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );
	
	//set topology
	g_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); /// D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	
	//set vertex description
	g_DeviceContext->IASetInputLayout(g_InputLayout);
	
	//set shaders
	g_DeviceContext->VSSetShader(g_VertexShader, nullptr, 0);
	g_DeviceContext->HSSetShader(nullptr, nullptr, 0);
	g_DeviceContext->DSSetShader(nullptr, nullptr, 0);
	g_DeviceContext->GSSetShader(nullptr, nullptr, 0);
	g_DeviceContext->PSSetShader(g_PixelShader, nullptr, 0);

	//g_DeviceContext->Map(material_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//material_own* material_buffer_ = (material_own*)resource.pData;
	//material_buffer_->Ka = { 1, 1, 0.5, 0 };
	//material_buffer_->Kd = { 1, 1, 1, 0 };
	//material_buffer_->Ks = { 1, 1, 1, 0 };
	//g_DeviceContext->Unmap(material_buffer, 0);

	// set buffers
	g_DeviceContext->VSSetConstantBuffers(0, 1, &g_MatrixBuffer);
	g_DeviceContext->PSSetConstantBuffers(0, 1, &material_buffer);
	g_DeviceContext->PSSetConstantBuffers(1, 1, &camera_buffer);

	D3D11_MAPPED_SUBRESOURCE resource;
	g_DeviceContext->Map(camera_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	camera_light* camera_light_buffer_ = (camera_light*)resource.pData;
	camera_light_buffer_->light_pos = { 0, 0, 5, 0 };
	camera_light_buffer_->camera_pos = camera->GetPos().xyz0();
	g_DeviceContext->Unmap(camera_buffer, 0);

	// time to render our objects
	renderObjects();

	//swap front and back buffer
	return g_SwapChain->Present( 0, 0 );
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:

		switch(wParam)
		{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void Release()
{
	// deallocate objects
	releaseObjects();

	// free D3D stuff
	SAFE_RELEASE(g_SwapChain);
	SAFE_RELEASE(g_RenderTargetView);
	SAFE_RELEASE(g_DepthStencil);
	SAFE_RELEASE(g_DepthStencilView);
	SAFE_RELEASE(g_RasterState);
	SAFE_RELEASE(g_RasterStateSkyDome);

	SAFE_RELEASE(g_InputLayout);
	SAFE_RELEASE(g_VertexShader);
	SAFE_RELEASE(g_VertexShaderSkyDome);
	SAFE_RELEASE(g_VertexShaderReflection);
	SAFE_RELEASE(g_PixelShader);
	SAFE_RELEASE(g_PixelShaderSkyDome);
	SAFE_RELEASE(g_PixelShaderReflection);

	SAFE_RELEASE(g_VertexShader);
	SAFE_RELEASE(g_DeviceContext);
	SAFE_RELEASE(g_Device);

	SAFE_RELEASE(g_MatrixBuffer);
	SAFE_RELEASE(camera_buffer);
	SAFE_RELEASE(material_buffer);
	SAFE_RELEASE(SkyTexSRV);
	SAFE_RELEASE(SkyTex);

}