#include "Shader.h"

void Shader::Initialization(Microsoft::WRL::ComPtr<ID3D11Device> device, DisplayWin32& display)
{
	ID3DBlob* errorCode = nullptr;

	// ���������� ���������� ������� (VSMain).
	HRESULT res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",					// ���� � HLSL-�����
		nullptr,											// ��� ��������
		nullptr,											// ��� �������������� include
		"VSMain",											// ��� �������-�������
		"vs_5_0",											// ������ HLSL (��������� ������ 5.0)
		// D3DCOMPILE_DEBUG � �������� ���������� ������. D3DCOMPILE_SKIP_OPTIMIZATION � ���������� �����������.
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShaderByteCode,
		&errorCode									// ������ ����������
	);

	if (FAILED(res)) 
	{
		// ���� ������ �� ���������������, ������� ������
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// ���� ������ ���, ������ ���� ������� �� ������
		else
		{
			MessageBox(display.hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return;
	}

	errorCode = nullptr;

	// ���������� �������� ��� �������.
	// TEST = 1 � ���������� ������ #define TEST 1 � HLSL.
	// TCOLOR = float4(0.0f, 1.0f, 0.0f, 1.0f) � ����� ������������ � ������� ��� TCOLOR.
	// ��������� nullptr, nullptr ����������, �� �������� ����� �������.
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	// ���������� ����������� �������
	res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros,										// �������� ��������
		nullptr,											// ��� include ������
		"PSMain",										    // ����� ����� (���������� ������)
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

	// �������� ���������� � ����������� �������
	// GetBufferPointer() � ��������� �� �������� ���.
	// GetBufferSize() � ������ ��������� ����.
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

	// �������� ������� ������
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
			// D3D11_APPEND_ALIGNED_ELEMENT ������������� ����������� ���� ����� �������.
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};

	// �������� Input Layout
	// ������ ������ layout, ������� ��������� ����� ������ � ��������.
	// ���� ������ �� inputElements.
	// ���������� ���������������� vertexBC (��������� ������).
	res = device->CreateInputLayout(
		// �������� ������� ������ � ���������� ��������� (POSITION, COLOR)
		inputElements, 2,
		vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &layout);

	if (FAILED(res))
	{
		if (errorCode) {
			char* compileErrors = (char*)(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
	}

	// �������� ��������� �������������
	CD3D11_RASTERIZER_DESC rastDesc = {};
	// CullMode = D3D11_CULL_NONE � ��������� ��������� ��������� ������, �� ���� ��� ������� ���������� ����� ����������.
	rastDesc.CullMode = D3D11_CULL_NONE;
	// FillMode = D3D11_FILL_SOLID � ��������� ������������� �������� �������� (� �� ��������).
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