#include "TriangleComponent.h"

bool TriangleComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
    // Компиляция вершинного шейдера (VSMain).
    ID3DBlob* errorVertexCode = nullptr;

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
        &errorVertexCode									// Ошибки компиляции
    );

	/*
	if (FAILED(res)) {
		// Если шейдер не скомпилировался, выводим ошибку
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// Если ошибок нет, значит файл шейдера не найден
		else
		{
			MessageBox(display.hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return 0;
	}*/

	// Добавление макросов для шейдера.
	// TEST = 1 — определяет макрос #define TEST 1 в HLSL.
	// TCOLOR = float4(0.0f, 1.0f, 0.0f, 1.0f) — можно использовать в шейдере как TCOLOR.
	// Последний nullptr, nullptr обязателен, он означает конец массива.
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	// Компиляция пиксельного шейдера
	ID3DBlob* errorPixelCode;

	res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros,										// Передача макросов
		nullptr,											// Без include файлов
		"PSMain",										    // Точка входа (пиксельный шейдер)
		"ps_5_0",											// HLSL 5.0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShaderByteCode,
		&errorPixelCode
	);

	// Проверка???

	// Создание вершинного и пиксельного шейдера
	// GetBufferPointer() — указатель на бинарный код.
	// GetBufferSize() — размер бинарного кода.
	res = device->CreateVertexShader( vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
	res = device->CreatePixelShader( pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

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
		inputElements,	2,								
		vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &layout);

	if (FAILED(res)) {
		//
	}

	//  Создание массива вершин
	points = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

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
	vertexData.pSysMem = &points.front();	
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Создание буфера вершин
	device->CreateBuffer(&vertexBufDesc, &vertexData, &vertBuff);

	// Массив indeces содержит индексы вершин, которые будут использоваться для отрисовки треугольников
	indexes = { 0,1,2, 1,0,3 };
	D3D11_BUFFER_DESC indexBufDesc = {};
	// D3D11_USAGE_DEFAULT — стандартный режим, данные будут использоваться GPU без прямого доступа CPU.
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	// D3D11_BIND_INDEX_BUFFER — указывает, что этот буфер содержит индексы для отрисовки.
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	// Размер буфера в байтах.
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indexes);

	// Структура D3D11_SUBRESOURCE_DATA содержит указатель pSysMem на массив индексов, который будет загружен в буфер.
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = &indexes.front();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Создаyние индексного буфера
	device->CreateBuffer(&indexBufDesc, &indexData, &indBuff);

	// Создание состояния растеризатора
	CD3D11_RASTERIZER_DESC rastDesc = {};
	// CullMode = D3D11_CULL_NONE — отключает отсечение невидимых граней, то есть все стороны примитивов будут нарисованы.
	rastDesc.CullMode = D3D11_CULL_NONE;
	// FillMode = D3D11_FILL_SOLID — отрисовка треугольников сплошной заливкой (а не каркасом).
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = device->CreateRasterizerState(&rastDesc, &rastState);

	// Проверка???

	return true;
}

void TriangleComponent::Draw(ID3D11DeviceContext* context)
{
	// Настройка вершинного буфера
	// Эти массивы будут использоваться при привязке вершинного буфера:
	//		strides — указывает размер одной вершины (в байтах). В данном случае, каждая вершина состоит из двух XMFLOAT4 (позиция + цвет), 
	//		то есть 32 байта (4 * 4 * 2).
	//		offsets — смещение (в байтах) от начала буфера (0, так как данные начинаются сразу).
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };

	context->RSSetState(rastState);

	// Шаг 8 - Настройка входных данных

	// Привязывается входной макет вершин (IASetInputLayout).
	context->IASetInputLayout(layout);
	// Устанавливается примитивная топология (D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), что означает, что каждые три вершины образуют один треугольник.
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Привязывается индексный буфер (IASetIndexBuffer).
	context->IASetIndexBuffer(indBuff, DXGI_FORMAT_R32_UINT, 0);
	// Привязывается вершинный буфер (IASetVertexBuffers).
	context->IASetVertexBuffers(0, 1, &vertBuff, strides, offsets);

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
}