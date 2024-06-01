#include "pch.h"
#include "TLS.h"

thread_local uint32 LThreadId = 0;
thread_local std::shared_ptr <SendBufferChunk> LSendBufferChunk;
