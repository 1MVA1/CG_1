#pragma once

#include <wrl.h>
#include <d3d11.h>

#include "TriangleComponent.h"

class CameComponent
{
public:
	TriangleComponent triangleComp = TriangleComponent();

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device);

	void Draw(ID3D11DeviceContext* context);
};