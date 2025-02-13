#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <iostream>
#include <vector>

class TriangleComponent 
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
    // ������ ������
    std::vector<DirectX::XMFLOAT4> points;
    // 
    std::vector<int> indexes;
    // ����� ������ 
    ID3D11Buffer* vertBuff;
    // ����� ��������
    ID3D11Buffer* indBuff;

    bool Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device);
    
    void Draw(ID3D11DeviceContext* context);
};
