#include "GameComponent.h"

void CameComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, std::vector<points_indexes> data)
{
    for (auto& item : data) 
    {
        TriangleComponent tc = TriangleComponent();
        tc.Initialize(device, item.points, item.indexes, item.stride, item.offset);
        triangleComps.push_back(tc);
    }
}

// ???
void CameComponent::Update()
{
    for (auto& tc : triangleComps)
    {
        tc.Update();
    }
}

void CameComponent::Draw(ID3D11DeviceContext* context, ID3D11InputLayout* layout, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader)
{
    for (auto& tc : triangleComps)
    {
        tc.Draw(context, layout, pixelShader, vertexShader);
    }
}

void CameComponent::DestroyResources()
{
    for (auto& tc : triangleComps)
    {
        tc.DestroyResources();
    }
}