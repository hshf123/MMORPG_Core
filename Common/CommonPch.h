#pragma once

#ifdef DEV_TEST
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#pragma warning (disable : 26451)
#pragma warning (disable : 26495)

