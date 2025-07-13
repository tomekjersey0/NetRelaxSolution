#pragma once

#include "NetSocket.h"

#include <string>
#include <map>

namespace Net {
    class GetError
    {
    private:
        struct ErrorInfo
        {
            int code;
            std::string name;
            std::string meaning;
        };

#ifdef _WIN32
        static inline const std::map<int, ErrorInfo> WSAErrorMap = {
            {10035, {10035, "WSAEWOULDBLOCK", "Operation would block; try again later"}},
            {10036, {10036, "WSAEINPROGRESS", "Operation in progress"}},
            {10037, {10037, "WSAEALREADY", "Operation already in progress"}},
            {10061, {10061, "WSAECONNREFUSED", "Connection refused by target host"}},
            {10060, {10060, "WSAETIMEDOUT", "Connection timed out"}},
            {10065, {10065, "WSAEHOSTUNREACH", "No route to host"}},
            {10051, {10051, "WSAENETUNREACH", "Network unreachable"}},
            {10064, {10064, "WSAEHOSTDOWN", "Host is down"}},
            {10054, {10054, "WSAECONNRESET", "Connection reset by peer"}},
            {10053, {10053, "WSAECONNABORTED", "Connection aborted"}},
            {10048, {10048, "WSAEADDRINUSE", "Address already in use"}},
            {10049, {10049, "WSAEADDRNOTAVAIL", "Cannot assign requested address"}},
            {10038, {10038, "WSAENOTSOCK", "Descriptor is not a socket"}},
            {10056, {10056, "WSAEISCONN", "Socket is already connected"}},
            {10057, {10057, "WSAENOTCONN", "Socket is not connected"}},
            {10058, {10058, "WSAESHUTDOWN", "Cannot send after socket shutdown"}},
            {10055, {10055, "WSAENOBUFS", "No buffer space available"}},
            {10024, {10024, "WSAEMFILE", "Too many open files"}},
            {10013, {10013, "WSAEACCES", "Permission denied"}},
            {10022, {10022, "WSAEINVAL", "Invalid argument"}},
            {10014, {10014, "WSAEFAULT", "Bad address"}},
            {10009, {10009, "WSAEBADF", "Bad file descriptor"}},
            {10093, {10093, "WSANOTINITIALISED", "WSAStartup not called"}},
            {10091, {10091, "WSASYSNOTREADY", "Network subsystem not ready"}},
            {10092, {10092, "WSAVERNOTSUPPORTED", "Winsock version not supported"}},
            {11001, {11001, "WSAHOST_NOT_FOUND", "Host not found"}},
            {11002, {11002, "WSATRY_AGAIN", "Non-authoritative host not found; try again"}},
            {11003, {11003, "WSANO_RECOVERY", "Non-recoverable name server error"}},
            {11004, {11004, "WSANO_DATA", "Valid name, no data record of requested type"}}
        };

        static ErrorInfo getLastErrorInfo()
        {
            int code = WSAGetLastError();
            auto it = WSAErrorMap.find(code);
            if (it != WSAErrorMap.end())
            {
                return it->second;
            }
            else
            {
                return ErrorInfo{
                    code,
                    "UNKNOWN_ERROR",
                    "Unknown Winsock error code"
                };
            }
        }
#endif

    public:
        static std::string getLastErrorFullMessage()
        {
#ifdef _WIN32
            auto info = getLastErrorInfo();
            return "WSAError " + std::to_string(info.code) + " (" + info.name + "): " + info.meaning;
#else
            int err = errno;
            return "POSIX Error " + std::to_string(err) + ": " + std::strerror(err);
#endif
        }

        static std::string getLastErrorName()
        {
#ifdef _WIN32
            return getLastErrorInfo().name;
#else
            int err = errno;
            return "POSIX_ERR_" + std::to_string(err);
#endif
        }

        static std::string getLastErrorMeaning()
        {
#ifdef _WIN32
            return getLastErrorInfo().meaning;
#else
            int err = errno;
            return std::strerror(err);
#endif
        }

        static int getLastErrorCode()
        {
#ifdef _WIN32
            return WSAGetLastError();
#else
            return errno;
#endif
        }
    };

}