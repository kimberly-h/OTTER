#include "IBuffer.h"

IBuffer::IBuffer(GLenum type, GLenum usage) :
	_elementCount(0),
	_elementSize(0),
	_handle(0)
{
	// TODO: implement
	_type = type;
	_usage = usage;
	glCreateBuffers(1, &_handle);
}

IBuffer::~IBuffer() {
	// TODO: implement
	if (_handle != 0)
	{ 
	glDeleteBuffers(1, &_handle); 
	_handle = 0; 
	}
}

void IBuffer::LoadData(const void* data, size_t elementSize, size_t elementCount) {
	// TODO: implement
	glNamedBufferData(_handle, elementSize * elementCount, data, _usage);
	_elementCount = elementCount;
	_elementSize = elementSize;
}

void IBuffer::Bind() {
	glBindBuffer(_type, _handle);
}

void IBuffer::UnBind(GLenum type) {
	glBindBuffer(type, 0);
}
