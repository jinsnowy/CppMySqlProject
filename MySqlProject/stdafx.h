#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstring>
#include <cmath>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <utility>

#include <mutex>
#include <thread>

#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "mysqlcppconn-static.lib")

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

#include "Format.h"
#include "Random.h"
#include "Stopwatch.h"

#include "Synchronization.h"
#include "PathManager.h"
#include "Logger.h"