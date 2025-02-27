#include "TriangleComponent.h"

void TriangleComponent::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, const std::vector<DirectX::XMFLOAT4> points, 
	const std::vector<int> indexes, const UINT stride, const UINT offset)
{
	if (vertBuff) vertBuff->Release();
	if (indBuff) indBuff->Release();

	this->stride = stride;
	this->offset = offset;



	this->points = points;

	// Описание буфера вершин
	D3D11_BUFFER_DESC vertexBufDesc = {};
	// D3D11_USAGE_DEFAULT — буфер в GPU, нельзя менять с CPU.
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	// D3D11_BIND_VERTEX_BUFFER — используется как буфер вершин.
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	// ByteWidth — размер буфера
	vertexBufDesc.ByteWidth = static_cast<UINT>(sizeof(DirectX::XMFLOAT4) * points.size());

	// Заполнение буфера данными
	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = &points.front();	
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Создание буфера вершин
	device->CreateBuffer(&vertexBufDesc, &vertexData, &vertBuff);



	this->indexes = indexes;

	D3D11_BUFFER_DESC indexBufDesc = {};
	// D3D11_USAGE_DEFAULT — стандартный режим, данные будут использоваться GPU без прямого доступа CPU.
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	// D3D11_BIND_INDEX_BUFFER — указывает, что этот буфер содержит индексы для отрисовки.
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	// Размер буфера в байтах.
	indexBufDesc.ByteWidth = static_cast<UINT>(sizeof(int) * indexes.size());

	// Структура D3D11_SUBRESOURCE_DATA содержит указатель pSysMem на массив индексов, который будет загружен в буфер.
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = &indexes.front();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Создаyние индексного буфера
	device->CreateBuffer(&indexBufDesc, &indexData, &indBuff);
}

// ???
void TriangleComponent::Update()
{

}

void TriangleComponent::Draw(ID3D11DeviceContext* context, ID3D11InputLayout* layout, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader)
{
	// Привязывается входной макет вершин (IASetInputLayout).
	context->IASetInputLayout(layout);
	// Устанавливается примитивная топология (D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), что означает, что каждые три вершины образуют один треугольник.
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Привязывается индексный буфер (IASetIndexBuffer).
	context->IASetIndexBuffer(indBuff, DXGI_FORMAT_R32_UINT, 0);
	// Привязывается вершинный буфер (IASetVertexBuffers).
	context->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	//Рисуем треугольники
	// Отрисовываются треугольники с использованием индексного буфера.
	// количество индексов (два треугольника).
	// начальный индекс
	// базовый индекс вершины
	context->DrawIndexed(static_cast<UINT>(indexes.size()), 0, 0);
}

void TriangleComponent::DestroyResources()
{
	points.clear();
	indexes.clear();

	// Освобождение ресурсов DirectX
	if (vertBuff) {
		vertBuff->Release();
		vertBuff = nullptr;
	}
	if (indBuff) {
		indBuff->Release();
		indBuff = nullptr;
	}
}