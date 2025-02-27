#include "Shader.h"

void Shader::Initialization(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin32& display)
{
	ID3DBlob* errorCode = nullptr;

	// Компиляция вершинного шейдера (VSMain).
	HRESULT res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",					// Путь к HLSL-файлу
		nullptr,											// Без макросов
		nullptr,											// Без дополнительных include
		"VSMain",											// Имя функции-шейдера
		"vs_5_0",											// Версия HLSL (вершинный шейдер 5.0)
		// D3DCOMPILE_DEBUG — включает отладочные данные. D3DCOMPILE_SKIP_OPTIMIZATION — пропускает оптимизацию.
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShaderByteCode,
		&errorCode									// Ошибки компиляции
	);

	if (FAILED(res)) 
	{
		// Если шейдер не скомпилировался, выводим ошибку
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// Если ошибок нет, значит файл шейдера не найден
		else
		{
			MessageBox(display.hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return;
	}

	errorCode = nullptr;

	// Добавление макросов для шейдера.
	// TEST = 1 — определяет макрос #define TEST 1 в HLSL.
	// TCOLOR = float4(0.0f, 1.0f, 0.0f, 1.0f) — можно использовать в шейдере как TCOLOR.
	// Последний nullptr, nullptr обязателен, он означает конец массива.
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	// Компиляция пиксельного шейдера
	res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros,										// Передача макросов
		nullptr,											// Без include файлов
		"PSMain",										    // Точка входа (пиксельный шейдер)
		"ps_5_0",											// HLSL 5.0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShaderByteCode,
		&errorCode
	);

	if (FAILED(res))
	{
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
	}

	// Создание вершинного и пиксельного шейдера
	// GetBufferPointer() — указатель на бинарный код.
	// GetBufferSize() — размер бинарного кода.
	res = device->CreateVertexShader(vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);

	if (FAILED(res))
	{
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
	}

	res = device->CreatePixelShader(pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

	if (FAILED(res))
	{
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
	}

	// Описание формата вершин
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION", 0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0, 0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR", 0,
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
	res = device->CreateInputLayout(
		// Описание формата вершин и Количество элементов (POSITION, COLOR)
		inputElements, 2,
		vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &layout);

	if (FAILED(res))
	{
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
	}

	// Создание состояния растеризатора
	CD3D11_RASTERIZER_DESC rastDesc = {};
	// CullMode = D3D11_CULL_NONE — отключает отсечение невидимых граней, то есть все стороны примитивов будут нарисованы.
	rastDesc.CullMode = D3D11_CULL_NONE;
	// FillMode = D3D11_FILL_SOLID — отрисовка треугольников сплошной заливкой (а не каркасом).
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = device->CreateRasterizerState(&rastDesc, &rastState);

	if (FAILED(res))
	{
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
	}
}

void Shader::DestroyResources()
{
	if (pixelShader) 
	{
		pixelShader->Release();
		pixelShader = nullptr;
	}

	if (vertexShader) 
	{
		vertexShader->Release();
		vertexShader = nullptr;
	}

	if (pixelShaderByteCode) 
	{
		pixelShaderByteCode->Release();
		pixelShaderByteCode = nullptr;
	}

	if (vertexShaderByteCode) 
	{
		vertexShaderByteCode->Release();
		vertexShaderByteCode = nullptr;
	}

	if (layout) 
	{
		layout->Release();
		layout = nullptr;
	}

	if (rastState) 
	{
		rastState->Release();
		rastState = nullptr;
	}
}