#include "Descriptor.h"
#include "GPUMemManager.h"

namespace Graphic {
	/*void BufferDescriptor::copyData(const void* data, UINT size)
	{ GPU::MemoryManager::UploadData(*m_Buffer, data, size, m_Offset); }*/


	/*IndexBuffer::IndexBuffer(ptrGBuffer gpubuffer, const UINT bufferSize)
		: BufferDescriptor(gpubuffer, bufferSize), m_start(0)
	{
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}*/

	/*// TODO check whether buffersize & start is correct
	IndexBuffer::IndexBuffer(IndexBuffer& buffer, const UINT start, const UINT end)
		: Descriptor(buffer.m_Buffer, (end - start) * sizeof(UINT)), m_start(start) // bufferSize = (end - start) * sizeof(UINT)
	{
		assert(start + m_BufferSize <= buffer.m_BufferSize && "start position plus bufferSize too large");
		m_Offset = buffer.m_Offset + start;
		m_view.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_view.Format = DXGI_FORMAT_R32_UINT;
		m_view.SizeInBytes = m_BufferSize;
	}*/

	/*ConstantBuffer::ConstantBuffer(ptrGBuffer gpubuffer, const UINT bufferSize)
		: BufferDescriptor(gpubuffer, CalculateConstantBufferByteSize(bufferSize)),
		m_RootHeapIndex(-1)
	{
		assert(m_BufferSize % 256 == 0 && "Constant buffer size not aligned");
		m_Offset = m_Buffer->MemAlloc(m_BufferSize);
		m_cbvDesc.BufferLocation = m_Buffer->GetGPUAddr() + m_Offset;
		m_cbvDesc.SizeInBytes = m_BufferSize;
	}

	void ConstantBuffer::CreateView(DescriptorTable& table, UINT slot)
	{
		ID3D12Device* device = Engine::GetDevice();
		D3D12_CPU_DESCRIPTOR_HANDLE handle = table.GetSlot(slot);
		device->CreateConstantBufferView(&m_cbvDesc, handle);
	}

	void ConstantBuffer::CreateRootView()
	{
		assert(m_RootHeapIndex == -1 && "Root View should be created only once");
		ID3D12Device* device = Engine::GetDevice();
		DescriptorHeap* initheap = Engine::GetInitHeap();
		m_RootHeapIndex = initheap->MallocHeap();
		device->CreateConstantBufferView(&m_cbvDesc, initheap->GetCPUHandle(m_RootHeapIndex));
	}*/
}