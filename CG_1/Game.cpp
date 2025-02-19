#include "Game.h"

void Game::Initialization(LPCWSTR applicationName, int width, int height) {
	display = DisplayWin32(L"My3DApp", 800, 800); 
}

void Game::CreateBackBuff()
{
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	// swap chain � ��� ������� (��� �������) ��������, ������� �������� �������� �������� ������.
	// �������� ��������� (����� �������� ��������� �������� ������).
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;													// ������� ����� (���� ����� ��������������, ������ ������������ �� ������.)
	swapDesc.BufferDesc.Width = display.ClientWidth;							// ������ ����
	swapDesc.BufferDesc.Height = display.ClientHeight;							// ������ ����
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;					// ������ �������� (RGBA 8 ���)
	// ������� ���������� ������ 
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// ������������ ����������� ������ ���������� ������.
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// DXGI_USAGE_RENDER_TARGET_OUTPUT � ����� ������������ ��� ���� ����������.
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// ����, � ������� ����� �������������� �����.
	swapDesc.OutputWindow = display.hWnd;
	swapDesc.Windowed = true;
	// DXGI_SWAP_EFFECT_FLIP_DISCARD � ����� ������� ����� ����� ������: ��� ����� ������ ������ ����� �������������.
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// ��� �����. DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH � ��������� ������������� ����� ������� � ������������� ��������.
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// ��������� �����������.
	// Count = 1 � ��� ��������������� (MSAA).
	swapDesc.SampleDesc.Count = 1;
	// Quality = 0 � ����������� ��������.
	swapDesc.SampleDesc.Quality = 0;

	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr,					// GPU (nullptr = ������������ ��������)
		D3D_DRIVER_TYPE_HARDWARE,	// ���������� ���������
		nullptr,					// ��� ������������ �������
		D3D11_CREATE_DEVICE_DEBUG,	// �������� ������� (������ ��� ����������)
		featureLevel,
		1,							// ���������� ������� ����������������
		D3D11_SDK_VERSION,			// ������ SDK
		&swapDesc,
		&swapChain,
		&device,
		nullptr,					// ������� ���������������� (�� �����)
		&context
	);

	if (FAILED(res))
	{
		// �����, � ������
	}

	// swapChain->GetBuffer(0, ...) �������� ������ �����, ������� ������������ ��� �������� ����������� ��� ����������.
	// __uuidof(ID3D11Texture2D) ������� �����������, ��� �� ������� 2D-��������.
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuff);	// __uuidof(ID3D11Texture2D)
}

void Game::PrepareResources()
{
	HRESULT res = device->CreateRenderTargetView(backBuff, nullptr, &rendTargView);

	if (FAILED(res))
	{
		// �����, � ������
	}

	shader.Initialization(device);

	context->RSSetState(shader.rastState);

	std::vector<points_indexes> data;
	data.push_back({
	{
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),
		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	},
	{ 0, 1, 2, 1, 0, 3 },
	32, 0
	});

	gameComp.Initialize(device, data);
}

void Game::Update()
{
	// ������ deltaTime � FPS
	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;

	totalTime += deltaTime;
	frameCount++;

	if (totalTime > 1.0f) {
		float fps = frameCount / totalTime;

		totalTime -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		// ������ ������� ����������� ��������� ���� (SetWindowText) � ������� ��������� FPS.
		SetWindowText(display.hWnd, text);

		frameCount = 0;
	}

	PrepareFrame();
	Draw();
}

void Game::MessageHandler()
{
	// ���������� MSG msg, ������� ����� ������� ��������� Windows (��������, �������� ����).
	msg = {};

	// ��������� ��������� Windows
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// TranslateMessage(&msg) ������������ ��������� ����������.
		TranslateMessage(&msg);
		// DispatchMessage(&msg) ���������� ��������� � ������� ���������.
		DispatchMessage(&msg);
	}

	// ���� ������ ��������� WM_QUIT, �� ��������� ��������� ������
	if (msg.message == WM_QUIT) {
		isExitRequested = true;
	}
}

void Game::PrepareFrame()
{
	// ������� ��������� ���������
	context->ClearState();

	// ��������� �����
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(display.ClientWidth);
	viewport.Height = static_cast<float>(display.ClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	// �������� �������.
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}

void Game::Draw()
{
	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };

	// ������� BackBuffer
	context->ClearRenderTargetView(rendTargView, color);
	// ������������� ������-������ (���� ���������� �����������). � ������ ������ � �������� back buffer (rtv).
	context->OMSetRenderTargets(1, &rendTargView, nullptr);

	gameComp.Draw(context, shader.layout, shader.pixelShader, shader.vertexShader);

	// ���������� ������-�������.
	context->OMSetRenderTargets(0, nullptr, nullptr);

	// ������� ���������
	// 1 � �������� ������������ ������������� (VSync).
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Game::Run()
{
	isExitRequested = false;

	PrevTime = std::chrono::steady_clock::now();
	totalTime = 0;
	frameCount = 0;

	// ���� ������������, ���� �� ����� �������� ����� (isExitRequested == false).
	while (!isExitRequested) 
	{
		MessageHandler();
		Update();
	}
}