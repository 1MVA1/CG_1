#include "GameComponent.h"

void CameComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device)
{
	triangleComp.Initialize(device);
}

void CameComponent::Draw(ID3D11DeviceContext* context)
{
	triangleComp.Draw(context);
}