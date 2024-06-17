#pragma once

#pragma warning(disable : 26498)	  // SPD Log
#pragma warning(disable : 4819)		  // SPD Log
#pragma warning (push)
#pragma warning (disable: 4244 4245 4100 4127)
#pragma warning (disable: 4634 4635)	// XML 주석 관련
#pragma warning (disable: 4389)			// Visual Studio 2022 (vc 143) , signed, unsigned 불일치
#pragma warning (disable: 26439 26498 26451 26444 26451 26812)	// VC++ Code Guidelines warning. POCO 라이브러리가 문제...

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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#pragma comment(lib, "Poco/PocoDatamd.lib")
#pragma comment(lib, "Poco/PocoDataODBCmd.lib")
#pragma comment(lib, "Poco/PocoFoundationmd.lib")

#include <vector>
#include <stack>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <atomic>

#include "Macro.h"
#include "Types.h"
#include "TLS.h"
#include "MemoryPool.h"
#include "SendBuffer.h"
#include "LogManager.h"
#include "StrUtils.h"
#include "TimeUtils.h"
