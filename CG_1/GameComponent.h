#pragma once

#include <wrl.h>
#include <d3d11.h>

#include "TriangleComponent.h"

struct points_indexes {
	std::vector<DirectX::XMFLOAT4> points;
	std::vector<int> indexes;
	UINT stride;
	UINT offset;
};

class CameComponent
{
public:
	std::vector<TriangleComponent> triangleComps;

	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, std::vector<points_indexes> data);

	void Update();

	void Draw(ID3D11DeviceContext* context, ID3D11InputLayout* layout, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader);

	void DestroyResources();
};