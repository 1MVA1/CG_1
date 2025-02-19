#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <iostream>

#include "DisplayWin32.h"

class Shader
{
public:
    // ������ layout ��������� ����� ������ � ��������
    ID3D11InputLayout* layout;
    ID3D11PixelShader* pixelShader;
    ID3DBlob* pixelShaderByteCode;
    ID3D11VertexShader* vertexShader;
    ID3DBlob* vertexShaderByteCode;
    // ��������� �������������
    ID3D11RasterizerState* rastState;

    void Initialization(Microsoft::WRL::ComPtr<ID3D11Device> device);
};