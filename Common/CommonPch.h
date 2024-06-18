#pragma once

#ifdef DEV_TEST
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "Enum.pb.h"
#include "Struct.pb.h"
#include "Protocol.pb.h"

#pragma warning (disable : 26451)
#pragma warning (disable : 26495)

