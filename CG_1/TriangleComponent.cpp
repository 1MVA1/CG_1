#include "TriangleComponent.h"

bool TriangleComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
    // ���������� ���������� ������� (VSMain).
    ID3DBlob* errorVertexCode = nullptr;

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
        &errorVertexCode									// ������ ����������
    );

	/*
	if (FAILED(res)) {
		// ���� ������ �� ���������������, ������� ������
		if (errorVertexCode) {
			char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// ���� ������ ���, ������ ���� ������� �� ������
		else
		{
			MessageBox(display.hWnd, L"MyVeryFirstShader.hlsl", L"Missing Shader File", MB_OK);
		}

		return 0;
	}*/

	// ���������� �������� ��� �������.
	// TEST = 1 � ���������� ������ #define TEST 1 � HLSL.
	// TCOLOR = float4(0.0f, 1.0f, 0.0f, 1.0f) � ����� ������������ � ������� ��� TCOLOR.
	// ��������� nullptr, nullptr ����������, �� �������� ����� �������.
	D3D_SHADER_MACRO Shader_Macros[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	// ���������� ����������� �������
	ID3DBlob* errorPixelCode;

	res = D3DCompileFromFile(
		L"Shaders/MyVeryFirstShader.hlsl",
		Shader_Macros,										// �������� ��������
		nullptr,											// ��� include ������
		"PSMain",										    // ����� ����� (���������� ������)
		"ps_5_0",											// HLSL 5.0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShaderByteCode,
		&errorPixelCode
	);

	// ��������???

	// �������� ���������� � ����������� �������
	// GetBufferPointer() � ��������� �� �������� ���.
	// GetBufferSize() � ������ ��������� ����.
	res = device->CreateVertexShader( vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), nullptr, &vertexShader);
	res = device->CreatePixelShader( pixelShaderByteCode->GetBufferPointer(), pixelShaderByteCode->GetBufferSize(), nullptr, &pixelShader);

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
		inputElements,	2,								
		vertexShaderByteCode->GetBufferPointer(), vertexShaderByteCode->GetBufferSize(), &layout);

	if (FAILED(res)) {
		//
	}

	//  �������� ������� ������
	points = {
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	// �������� ������ ������
	D3D11_BUFFER_DESC vertexBufDesc = {};
	// D3D11_USAGE_DEFAULT � ����� � GPU, ������ ������ � CPU.
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	// D3D11_BIND_VERTEX_BUFFER � ������������ ��� ����� ������.
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	// ByteWidth � ������ ������ (����� 8 XMFLOAT4 �� 16 ����).
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	// ���������� ������ �������
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = &points.front();	
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// �������� ������ ������
	device->CreateBuffer(&vertexBufDesc, &vertexData, &vertBuff);

	// ������ indeces �������� ������� ������, ������� ����� �������������� ��� ��������� �������������
	indexes = { 0,1,2, 1,0,3 };
	D3D11_BUFFER_DESC indexBufDesc = {};
	// D3D11_USAGE_DEFAULT � ����������� �����, ������ ����� �������������� GPU ��� ������� ������� CPU.
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	// D3D11_BIND_INDEX_BUFFER � ���������, ��� ���� ����� �������� ������� ��� ���������.
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	// ������ ������ � ������.
	indexBufDesc.ByteWidth = sizeof(int) * std::size(indexes);

	// ��������� D3D11_SUBRESOURCE_DATA �������� ��������� pSysMem �� ������ ��������, ������� ����� �������� � �����.
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = &indexes.front();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �����y��� ���������� ������
	device->CreateBuffer(&indexBufDesc, &indexData, &indBuff);

	// �������� ��������� �������������
	CD3D11_RASTERIZER_DESC rastDesc = {};
	// CullMode = D3D11_CULL_NONE � ��������� ��������� ��������� ������, �� ���� ��� ������� ���������� ����� ����������.
	rastDesc.CullMode = D3D11_CULL_NONE;
	// FillMode = D3D11_FILL_SOLID � ��������� ������������� �������� �������� (� �� ��������).
	rastDesc.FillMode = D3D11_FILL_SOLID;

	res = device->CreateRasterizerState(&rastDesc, &rastState);

	// ��������???

	return true;
}

void TriangleComponent::Draw(ID3D11DeviceContext* context)
{
	// ��������� ���������� ������
	// ��� ������� ����� �������������� ��� �������� ���������� ������:
	//		strides � ��������� ������ ����� ������� (� ������). � ������ ������, ������ ������� ������� �� ���� XMFLOAT4 (������� + ����), 
	//		�� ���� 32 ����� (4 * 4 * 2).
	//		offsets � �������� (� ������) �� ������ ������ (0, ��� ��� ������ ���������� �����).
	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };

	context->RSSetState(rastState);

	// ��� 8 - ��������� ������� ������

	// ������������� ������� ����� ������ (IASetInputLayout).
	context->IASetInputLayout(layout);
	// ��������������� ����������� ��������� (D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), ��� ��������, ��� ������ ��� ������� �������� ���� �����������.
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ������������� ��������� ����� (IASetIndexBuffer).
	context->IASetIndexBuffer(indBuff, DXGI_FORMAT_R32_UINT, 0);
	// ������������� ��������� ����� (IASetVertexBuffers).
	context->IASetVertexBuffers(0, 1, &vertBuff, strides, offsets);

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
}