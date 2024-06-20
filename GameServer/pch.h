#pragma once

#include "CorePch.h"

#pragma comment(lib, "ServerCore.lib")
#ifdef DEV_TEST
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "Enum.pb.h"
#include "Struct.pb.h"
#include "Protocol.pb.h"