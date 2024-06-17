#pragma once
#include "pch.h"

extern thread_local uint32 LThreadId;
extern thread_local std::shared_ptr <class SendBufferChunk> LSendBufferChunk;
thread_local class JobQueue* LCurrentJobQueue;
thread_local uint64 LEndTickCount = 0LL;
