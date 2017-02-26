#pragma once

#ifndef _H_STDAFX
#define _H_STDAFX

#include <cassert>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <fstream>

#include <list>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include <algorithm>
#include <functional>
#include <limits>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#ifdef ANDROID
#include <asio.hpp>
#else
#include <boost/asio.hpp>
namespace asio = boost::asio;
#endif

#include <boost/endian/conversion.hpp>

#include <cryptopp/cryptlib.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/aes.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/oids.h>
#include <cryptopp/modes.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QDir>
#include <QStandardPaths>
#include <QGuiApplication>

#ifdef _MSC_VER
#ifdef _DEBUG
#pragma comment (lib, "cryptlibd-md.lib")
#else
#pragma comment (lib, "cryptlib-md.lib")
#endif
#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "wldap32.lib")
#pragma comment (lib, "comctl32.lib")
#pragma comment (lib, "rpcrt4.lib")
#pragma comment (lib, "wsock32.lib")
#pragma comment (lib, "odbc32.lib")
#endif

#if (!defined(WIN32)) && (defined(_WIN32) || defined(_WIN32_WINNT))
#define WIN32
#endif

#if (!defined(__linux__)) && (defined(__linux))
#define __linux__
#endif

#ifdef ANDROID
#include <cstdio>
namespace std
{
    inline std::string to_string(int n)
    {
        char buf[20];
        sprintf(buf, "%d", n);
        return std::string(buf);
    }
}
#endif

#endif
