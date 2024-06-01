#pragma once
#include "Singleton.h"

class SendBuffer : std::enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(std::shared_ptr<SendBufferChunk> owner, BYTE* buffer, int32 allocSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer; }
	uint32 AllocSize() { return _allocSize; }
	int32 WriteSize() { return _writeSize; }
	void Close(uint32 writeSize);

private:
	BYTE* _buffer;
	uint32 _allocSize = 0;
	uint32 _writeSize = 0;
	std::shared_ptr<SendBufferChunk> _owner;
};

class SendBufferChunk : public std::enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000,
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void Reset();
	std::shared_ptr<SendBuffer> Open(uint32 allocSize);
	void Close(uint32 writeSize);

	bool IsOpen() { return _open; }
	BYTE* Buffer() { return &_buffer[_usedSize]; }
	uint32 FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	std::array<BYTE, SEND_BUFFER_CHUNK_SIZE> _buffer = {};
	bool _open = false;
	uint32 _usedSize = 0;
};

class SendBufferManager : public RefSingleton<SendBufferManager>
{
public:
	std::shared_ptr<SendBuffer> Open(uint32 size);

private:
	std::shared_ptr<SendBufferChunk> Pop();
	void Push(std::shared_ptr<SendBufferChunk> buffer);

	static void PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	std::vector<std::shared_ptr<SendBufferChunk>> _sendBufferChunks;
};