#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// Этот код определяет оконную процедуру WndProc, которая обрабатывает сообщения операционной системы для окна приложения
// CALLBACK — специальное ключевое слово, указывающее, что функция используется в качестве обработчика событий Windows API.
// hwnd — идентификатор окна, которому предназначено сообщение.
// umessage — тип полученного сообщения.
// wparam — дополнительный параметр, часто содержит код клавиши или флаг.
// lparam — дополнительный параметр, содержащий информацию о позиции мыши, размере окна и т.д.
LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	// WM_KEYDOWN вызывается, когда нажимается клавиша на клавиатуре.
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

		//  Если нажата клавиша Esc (код 27), вызывается PostQuitMessage(0), что завершает работу приложения.
		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	default:
		{
			// Передача остальных сообщений в стандартный обработчик
			return DefWindowProc(hwnd, umessage, wparam, lparam);
		}
	}
}


int main()
{
	// LPCWSTR — это указатель на строку в формате Unicode. (L перед строкой означает, что строка хранится в широких символах)
	LPCWSTR applicationName = L"My3DApp";
	// HINSTANCE — идентификатор экземпляра приложения.
	// GetModuleHandle(nullptr); — получает экземпляр текущего процесса, который нужен при регистрации окна.
	HINSTANCE hInstance = GetModuleHandle(nullptr);

#pragma region Window init
	// Шаг 1 - настройка окна
	// 
	// WNDCLASSEX — это структура, содержащая настройки класса окна.
	// Она нужна, чтобы зарегистрировать окно в Windows API.
    WNDCLASSEX wc;
    
	// wc.style определяет стиль поведения окна:
	// CS_HREDRAW — перерисовывать окно при изменении ширины.
	// CS_VREDRAW — перерисовывать окно при изменении высоты.
	// CS_OWNDC — выделять уникальный контекст устройства (DC) для каждого окна (важно для 3D-графики).
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	// lpfnWndProc — это указатель на оконную процедуру.
	// WndProc — функция, обрабатывающая сообщения окна (например, нажатия клавиш, закрытие и т. д.).
	wc.lpfnWndProc = WndProc;
	// cbClsExtra — дополнительные байты для класса окна (обычно 0).
	wc.cbClsExtra = 0;
	// cbWndExtra — дополнительные байты для экземпляра окна (обычно 0).
	wc.cbWndExtra = 0;
	// hInstance — экземпляр приложения
	wc.hInstance = hInstance;
	// hIcon — главная иконка окна.
	// LoadIcon(nullptr, IDI_WINLOGO) — загружает иконку Windows по умолчанию.
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	// hIconSm = wc.hIcon; — маленькая иконка окна (например, в заголовке).
	wc.hIconSm = wc.hIcon;
	// hCursor — курсор по умолчанию.
	// LoadCursor(nullptr, IDC_ARROW) — загружает стандартный указатель-стрелку.
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// hbrBackground — цвет фона окна.
	// GetStockObject(BLACK_BRUSH) — делает фон чёрным.
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	// lpszMenuName — указывает ресурс меню для окна.
	// nullptr означает, что меню нет.
	wc.lpszMenuName = nullptr;
	// lpszClassName — имя класса окна(используется при создании окна).
	wc.lpszClassName = applicationName;
	// cbSize — размер структуры (обязательно для WNDCLASSEX).
	wc.cbSize = sizeof(WNDCLASSEX);

	// RegisterClassEx(&wc); — регистрирует класс окна в системе.
	// После этого можно создавать окно на основе этого класса.
	RegisterClassEx(&wc);


	auto screenWidth = 800;
	auto screenHeight = 800;

	// RECT — структура, содержащая координаты границ окна:
	//		left = 0, top = 0 (левый верхний угол).
	//		right = screenWidth, bottom = screenHeight(правый нижний угол).
	//		static_cast<LONG>(...) — приведение типа int → LONG (так требует RECT).
	RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
	// AdjustWindowRect(...) изменяет RECT, чтобы учесть границы окна:
	//		WS_OVERLAPPEDWINDOW — стандартный стиль окна с рамкой, заголовком и кнопками управления.
	//		FALSE — у окна нет меню.
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// WS_SYSMENU — кнопка закрытия и системное меню (Alt + Space).
	// WS_CAPTION — заголовок окна.
	// WS_MINIMIZEBOX — кнопка "Свернуть".
	// WS_THICKFRAME — изменяемый размер окна.
	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	// GetSystemMetrics(SM_CXSCREEN) — ширина экрана.
	// GetSystemMetrics(SM_CYSCREEN) — высота экрана.
	// (ширина/высота экрана - ширина/высота окна) / 2 — позиция X/Y по центру.
	auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	// Возвращает HWND (хендл окна), который нужен для управления окном.
	HWND hWnd = CreateWindowEx(
		// WS_EX_APPWINDOW — окно будет отдельным приложением (в Alt+Tab).
		WS_EX_APPWINDOW,       				    // Дополнительный стиль окна
		applicationName,        				// Имя класса окна
		applicationName,        				// Заголовок окна
		dwStyle,								// Стиль окна
		posX, posY,								// Координаты (X, Y)
		windowRect.right - windowRect.left,		// Ширина
		windowRect.bottom - windowRect.top,		// Высота
		nullptr, nullptr, hInstance, nullptr	// Родитель, меню, экземпляр, параметры
	);

	// SW_SHOW делает окно видимым.
	ShowWindow(hWnd, SW_SHOW);
	// Переводит окно в фокус, чтобы оно стало активным.
	SetForegroundWindow(hWnd);
	// Гарантирует, что окно получает ввод с клавиатуры.
	SetFocus(hWnd);
	// Показывает курсор (если он скрыт).
	ShowCursor(true);

#pragma endregion Window init

	// Шаг 2 - Создание SwapChain

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	// swap chain — это двойной (или тройной) буферинг, который помогает избежать мерцания экрана.
	// Обнуляем структуру (чтобы избежать случайных мусорных данных).
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;														// Двойной буфер (Один буфер отрисовывается, другой отображается на экране.)
	swapDesc.BufferDesc.Width = screenWidth;										// Ширина окна
	swapDesc.BufferDesc.Height = screenHeight;										// Высота окна
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						// Формат пикселей (RGBA 8 бит)
	// Частота обновления экрана 
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	// Используется стандартный способ обновления экрана.
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// DXGI_USAGE_RENDER_TARGET_OUTPUT — буфер используется как цель рендеринга.
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// Окно, в котором будет отрисовываться сцена.
	swapDesc.OutputWindow = hWnd;
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

	// Устройство D3D11 (представляет GPU).
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	// Контекст рендеринга(управляет отрисовкой).
	ID3D11DeviceContext* context;
	// Механизм смены кадров.
	IDXGISwapChain* swapChain;

	auto res = D3D11CreateDeviceAndSwapChain(
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

	if(FAILED(res))
	{
		// Капец, а почему
	}
	
	// Шаг 3 - RTV и back buffer

	// Получение back buffer
	ID3D11Texture2D* backTex;
	ID3D11RenderTargetView* rtv;
	// swapChain->GetBuffer(0, ...) получает задний буфер, который используется как основная поверхность для рендеринга.
	// __uuidof(ID3D11Texture2D) говорит компилятору, что мы ожидаем 2D-текстуру.
	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
	// CreateRenderTargetView() создаёт RTV (цель рендеринга).
	// nullptr означает, что используется вся текстура.
	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);

	// Шаг 4 - Компиляция шейдеров

	// Компиляция вершинного шейдера (VSMain).
	ID3DBlob* vertexBC = nullptr;
	ID3DBlob* errorVertexCode = nullptr;
	res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",					// Путь к HLSL-файлу
		nullptr,											// Без макросов
		nullptr,											// Без дополнительных include
		"VSMain",											// Имя функции-шейдера
		"vs_5_0",											// Версия HLSL (вершинный шейдер 5.0)
		// D3DCOMPILE_DEBUG — включает отладочные данные. D3DCOMPILE_SKIP_OPTIMIZATION — пропускает оптимизацию.
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexBC,											// Скомпилированный шейдерный код
		&errorVertexCode									// Ошибки компиляции
	);

	if (FAILED(res)) {
		// Если шейдер не скомпилировался, выводим ошибку
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// Если ошибок нет, значит файл шейдера не найден
		else
		{
			MessageBox(hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return 0;
	}

	// Добавление макросов для шейдера.
	// TEST = 1 — определяет макрос #define TEST 1 в HLSL.
	// TCOLOR = float4(0.0f, 1.0f, 0.0f, 1.0f) — можно использовать в шейдере как TCOLOR.
	// Последний nullptr, nullptr обязателен, он означает конец массива.
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	// Компиляция пиксельного шейдера
	ID3DBlob* pixelBC;
	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros,										// Передача макросов
		nullptr,											// Без include файлов
		"PSMain",										    // Точка входа (пиксельный шейдер)
		"ps_5_0",											// HLSL 5.0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelBC,
		&errorPixelCode
	);

	// Создание вершинного и пиксельного шейдера
	// GetBufferPointer() — указатель на бинарный код.
	// GetBufferSize() — размер бинарного кода.
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &vertexShader);

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &pixelShader);

	// Шаг 5 - Создание Inputs

	// Описание формата вершин
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			// D3D11_APPEND_ALIGNED_ELEMENT автоматически выравнивает цвет после позиции.
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};

	// Создание Input Layout
	// Создаёт объект layout, который связывает буфер вершин с шейдером.
	// Берёт формат из inputElements.
	// Использует скомпилированный vertexBC (вершинный шейдер).
	ID3D11InputLayout* layout;
	device->CreateInputLayout(
		inputElements,					// Описание формата вершин
		2,								// Количество элементов (POSITION, COLOR)
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		&layout);
	
	// Шаг 6 - Создание точек

	//  Создание массива вершин
	DirectX::XMFLOAT4 points[8] = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	// Шаг 7 - Создание буферов

	// Описание буфера вершин
	D3D11_BUFFER_DESC vertexBufDesc = {};
	// D3D11_USAGE_DEFAULT — буфер в GPU, нельзя менять с CPU.
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	// D3D11_BIND_VERTEX_BUFFER — используется как буфер вершин.
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	// ByteWidth — размер буфера (всего 8 XMFLOAT4 по 16 байт).
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	// Заполнение буфера данными
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points;				// Указатель на вершины
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Создание буфера вершин
	ID3D11Buffer* vb;
	device->CreateBuffer(&vertexBufDesc, &vertexData, &vb);

	// Массив indeces содержит индексы вершин, которые будут использоваться для отрисовки треугольников
	int indeces[] = { 0,1,2, 1,0,3 };
	D3D11_BUFFER_DESC indexBufDesc = {};
	// D3D11_USAGE_DEFAULT — стандартный режим, данные будут использоваться GPU без прямого доступа CPU.
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	// D3D11_BIND_INDEX_BUFFER — указывает, что этот буфер содержит индексы для отрисовки.
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	// Размер буфера в байтах.
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indeces);

	// Структура D3D11_SUBRESOURCE_DATA содержит указатель pSysMem на массив индексов, который будет загружен в буфер.
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indeces;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Создаётся индексный буфер ib, который передаётся на GPU.
	ID3D11Buffer* ib;
	device->CreateBuffer(&indexBufDesc, &indexData, &ib);

	// Настройка вершинного буфера
	// Эти массивы будут использоваться при привязке вершинного буфера:
	//		strides — указывает размер одной вершины (в байтах). В данном случае, каждая вершина состоит из двух XMFLOAT4 (позиция + цвет), 
	//		то есть 32 байта (4 * 4 * 2).
	//		offsets — смещение (в байтах) от начала буфера (0, так как данные начинаются сразу).
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };


	// Создание состояния растеризатора
	CD3D11_RASTERIZER_DESC rastDesc = {};
	// CullMode = D3D11_CULL_NONE — отключает отсечение невидимых граней, то есть все стороны примитивов будут нарисованы.
	rastDesc.CullMode = D3D11_CULL_NONE;
	// FillMode = D3D11_FILL_SOLID — отрисовка треугольников сплошной заливкой (а не каркасом).
	rastDesc.FillMode = D3D11_FILL_SOLID;

	ID3D11RasterizerState* rastState;
	res = device->CreateRasterizerState(&rastDesc, &rastState);

	context->RSSetState(rastState);


	// Переменные для отслеживания времени и FPS
	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;

	// Шаг 12 - Создание сообщения в окне

	// Создаётся переменная MSG msg, которая будет хранить сообщения Windows (например, закрытие окна).
	// Цикл продолжается, пока не будет запрошен выход (isExitRequested == false).
	MSG msg = {};
	bool isExitRequested = false;

	while (!isExitRequested) 
	{
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

		// Очистка состояния контекста
		context->ClearState();

		context->RSSetState(rastState);

		// Шаг 10 - Установка сцены

		D3D11_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(screenWidth);
		viewport.Height = static_cast<float>(screenHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		// Диапазон глубины.
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;

		context->RSSetViewports(1, &viewport);

		// Шаг 8 - Настройка входных данных

		// Привязывается входной макет вершин (IASetInputLayout).
		context->IASetInputLayout(layout);
		// Устанавливается примитивная топология (D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), что означает, что каждые три вершины образуют один треугольник.
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// Привязывается индексный буфер (IASetIndexBuffer).
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
		// Привязывается вершинный буфер (IASetVertexBuffers).
		context->IASetVertexBuffers(0, 1, &vb, strides, offsets);

		// Шаг 9 - Установка шейдеров

		context->VSSetShader(vertexShader, nullptr, 0);
		context->PSSetShader(pixelShader, nullptr, 0);

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
			SetWindowText(hWnd, text);

			frameCount = 0;
		}

		// Шаг 11 - Установка BackBuffer

		// Привязывается рендер-таргет (куда рендерится изображение). В данном случае — основной back buffer (rtv).
		context->OMSetRenderTargets(1, &rtv, nullptr);

		float color[] = { totalTime, 0.1f, 0.1f, 1.0f };

		// Шаг 13 - Очищаем BackBuffer

		context->ClearRenderTargetView(rtv, color);

		// Шаг 14 - Рисуем треугольники

		// Отрисовываются треугольники с использованием индексного буфера.
		// 6 — количество индексов(два треугольника).
		// 0 — начальный индекс.
		// 0 — базовый индекс вершины.
		context->DrawIndexed(6, 0, 0);

		// Сбрасывает рендер-таргеты.
		context->OMSetRenderTargets(0, nullptr, nullptr);

		// Шаг 15 - Выводим результат

		// 1 — включает вертикальную синхронизацию (VSync).
		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}

    std::cout << "Hello World!\n";
}