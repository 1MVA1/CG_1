#include "GameComponent.h"

void GameComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const std::vector<points_indexes>& data)
{
    for (const auto& item : data)
    {
        auto tc = std::make_unique<TriangleComponent>();
        tc->Initialize(device, item.points, item.indexes, item.stride, item.offset);
        triangleComps.push_back(std::move(tc));
    }
}

// ???
void GameComponent::Update()
{
    for (auto& tc : triangleComps)
    {
        tc->Update();
    }
}

void GameComponent::Draw(ID3D11DeviceContext* context, ID3D11InputLayout* layout, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader)
{
    for (auto& tc : triangleComps)
    {
        tc->Draw(context, layout, pixelShader, vertexShader);
    }
}

void GameComponent::DestroyResources()
{
    for (auto& tc : triangleComps)
    {
        tc->DestroyResources();
    }

    triangleComps.clear(); 
}
