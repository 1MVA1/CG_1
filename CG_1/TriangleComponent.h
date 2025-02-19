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
    // ������ ������
    std::vector<DirectX::XMFLOAT4> points;
    // ������ indeces �������� ������� ������, ������� ����� �������������� ��� ��������� �������������
    std::vector<int> indexes;
    // ��������� ���������� ������
    //	 stride � ��������� ������ ����� ������� (� ������).
    //	 offset � �������� (� ������) �� ������ ������.
    UINT stride;
    UINT offset;
    // ����� ������ 
    ID3D11Buffer* vertBuff;
    // ����� ��������
    ID3D11Buffer* indBuff;

    void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, std::vector<DirectX::XMFLOAT4> points, std::vector<int> indexes,
        UINT stride, UINT offset);

    void Update();
    
    void Draw(ID3D11DeviceContext* context, ID3D11InputLayout* layout, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader);

    void DestroyResources();
};
