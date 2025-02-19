#include "TriangleComponent.h"

void TriangleComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, std::vector<DirectX::XMFLOAT4> points, 
	std::vector<int> indexes, UINT stride, UINT offset)
{
	this->stride = stride;
	this->offset = offset;



	this->points = points;

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



	this->indexes = indexes;

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
}

// ???
void TriangleComponent::Update()
{

}

void TriangleComponent::Draw(ID3D11DeviceContext* context, ID3D11InputLayout* layout, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader)
{
	// ������������� ������� ����� ������ (IASetInputLayout).
	context->IASetInputLayout(layout);
	// ��������������� ����������� ��������� (D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), ��� ��������, ��� ������ ��� ������� �������� ���� �����������.
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// ������������� ��������� ����� (IASetIndexBuffer).
	context->IASetIndexBuffer(indBuff, DXGI_FORMAT_R32_UINT, 0);
	// ������������� ��������� ����� (IASetVertexBuffers).
	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	//������ ������������
	// �������������� ������������ � �������������� ���������� ������.
	// ���������� �������� (��� ������������).
	// ��������� ������
	// ������� ������ �������
	context->DrawIndexed(size(indexes), 0, 0);
}

void TriangleComponent::DestroyResources()
{
	points.clear();
	indexes.clear();

	// ������������ �������� DirectX
	if (vertBuff) {
		vertBuff->Release();
		vertBuff = nullptr;
	}
	if (indBuff) {
		indBuff->Release();
		indBuff = nullptr;
	}
}