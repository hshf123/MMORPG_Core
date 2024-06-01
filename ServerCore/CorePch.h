#pragma once

#pragma warning(disable : 26498)	  // SPD Log
#pragma warning(disable : 4819)		  // SPD Log

#include <iostream>
#include <shared_mutex>
#include <thread>
#include <functional>
#include <atomic>
#include <format>
#include <chrono>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <array>

#include "Macro.h"
#include "Types.h"
#include "TLS.h"
#include "MemoryPool.h"
#include "SendBuffer.h"
