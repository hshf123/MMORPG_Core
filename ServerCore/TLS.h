#pragma once
#include "pch.h"

extern thread_local uint32 LThreadId;
extern thread_local std::shared_ptr <class SendBufferChunk> LSendBufferChunk;
extern thread_local class JobQueue* LCurrentJobQueue;
extern thread_local uint64 LEndTickCount;
