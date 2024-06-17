#include "pch.h"
#include "TLS.h"
#include "JobQueue.h"

thread_local uint32 LThreadId = 0;
thread_local std::shared_ptr <SendBufferChunk> LSendBufferChunk;
thread_local JobQueue* LCurrentJobQueue = nullptr;
thread_local uint64 LEndTickCount = 0LL;