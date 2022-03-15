#pragma once
#include <boost/asio.hpp>

// get_local_interfaces and sinaddr_to_asio:
// See link below for original source
// https://stackoverflow.com/a/62303963/2680066
// This is for when a connection is loopback, the address that ASIO
// will give back will be a loopback address, which is useless for
// other equipment trying to use that address.

boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr);

#if defined(_WIN32)
    #undef UNICODE
    #include <winsock2.h>

    // Headers that need to be included after winsock2.h:
    #include <iphlpapi.h>
    #include <ws2ipdef.h>

    typedef IP_ADAPTER_UNICAST_ADDRESS_LH Addr;
    typedef IP_ADAPTER_ADDRESSES *AddrList;

    std::vector<boost::asio::ip::address> get_local_interfaces();

#elif defined(__APPLE__) || defined(__linux__)
    #include <arpa/inet.h>
    #include <ifaddrs.h>
    #include <net/if.h>
    #include <sys/types.h>

    std::vector<boost::asio::ip::address> get_local_interfaces();
#else
    #error "..."
#endif

boost::asio::ip::address GetBestSelfAddress(const std::vector<boost::asio::ip::address>& sels);