#include "Game.h"

void Game::Initialization(LPCWSTR applicationName, int width, int height) {
	display = DisplayWin32(L"My3DApp", 800, 800); 
}

void Game::CreateBackBuff()
{
	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	// swap chain — это двойной (или тройной) буферинг, который помогает избежать мерцания экрана.
	// Обнуляем структуру (чтобы избежать случайных мусорных данных).
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;													// Двойной буфер (Один буфер отрисовывается, другой отображается на экране.)
	swapDesc.BufferDesc.Width = display.ClientWidth;							// Ширина окна
	swapDesc.BufferDesc.Height = display.ClientHeight;							// Высота окна
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;					// Формат пикселей (RGBA 8 бит)
	// Частота обновления экрана 
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// Используется стандартный способ обновления экрана.
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// DXGI_USAGE_RENDER_TARGET_OUTPUT — буфер используется как цель рендеринга.
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Окно, в котором будет отрисовываться сцена.
	swapDesc.OutputWindow = display.hWnd;
	swapDesc.Windowed = true;
	// DXGI_SWAP_EFFECT_FLIP_DISCARD — более быстрый метод смены кадров: при смене кадров старый буфер отбрасывается.
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// Доп флаги. DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH — позволяет переключаться между оконным и полноэкранным режимами.
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// Настройка сглаживания.
	// Count = 1 — без мультисемплинга (MSAA).
	swapDesc.SampleDesc.Count = 1;
	// Quality = 0 — стандартное качество.
	swapDesc.SampleDesc.Quality = 0;

	HRESULT res = D3D11CreateDeviceAndSwapChain(
		nullptr,					// GPU (nullptr = использовать основной)
		D3D_DRIVER_TYPE_HARDWARE,	// Аппаратное ускорение
		nullptr,					// Без программного рендера
		D3D11_CREATE_DEVICE_DEBUG,	// Включаем отладку (только для разработки)
		featureLevel,
		1,							// Количество уровней функциональности
		D3D11_SDK_VERSION,			// Версия SDK
		&swapDesc,
		&swapChain,
		&device,
		nullptr,					// Уровень функциональности (не нужен)
		&context
	);

	if (FAILED(res))
	{
		// Капец, а почему
	}

	// swapChain->GetBuffer(0, ...) получает задний буфер, который используется как основная поверхность для рендеринга.
	// __uuidof(ID3D11Texture2D) говорит компилятору, что мы ожидаем 2D-текстуру.
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuff);	// __uuidof(ID3D11Texture2D)
}

void Game::PrepareResources()
{
	HRESULT res = device->CreateRenderTargetView(backBuff, nullptr, &rendTargView);

	if (FAILED(res))
	{
		// Капец, а почему
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
	// Расчёт deltaTime и FPS
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
		// Каждую секунду обновляется заголовок окна (SetWindowText) с текущим значением FPS.
		SetWindowText(display.hWnd, text);

		frameCount = 0;
	}

	PrepareFrame();
	Draw();
}

void Game::MessageHandler()
{
	// Переменная MSG msg, которая будет хранить сообщения Windows (например, закрытие окна).
	msg = {};

	// Обработка сообщений Windows
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		// TranslateMessage(&msg) обрабатывает сообщения клавиатуры.
		TranslateMessage(&msg);
		// DispatchMessage(&msg) отправляет сообщения в оконную процедуру.
		DispatchMessage(&msg);
	}

	// Если пришло сообщение WM_QUIT, то программа завершает работу
	if (msg.message == WM_QUIT) {
		isExitRequested = true;
	}
}

void Game::PrepareFrame()
{
	// Очистка состояния контекста
	context->ClearState();

	// Установка сцены
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(display.ClientWidth);
	viewport.Height = static_cast<float>(display.ClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	// Диапазон глубины.
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);
}

void Game::Draw()
{
	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };

	// Очищаем BackBuffer
	context->ClearRenderTargetView(rendTargView, color);
	// Привязывается рендер-таргет (куда рендерится изображение). В данном случае — основной back buffer (rtv).
	context->OMSetRenderTargets(1, &rendTargView, nullptr);

	gameComp.Draw(context, shader.layout, shader.pixelShader, shader.vertexShader);

	// Сбрасывает рендер-таргеты.
	context->OMSetRenderTargets(0, nullptr, nullptr);

	// Выводим результат
	// 1 — включает вертикальную синхронизацию (VSync).
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Game::Run()
{
	isExitRequested = false;

	PrevTime = std::chrono::steady_clock::now();
	totalTime = 0;
	frameCount = 0;

	// Цикл продолжается, пока не будет запрошен выход (isExitRequested == false).
	while (!isExitRequested) 
	{
		MessageHandler();
		Update();
	}
}