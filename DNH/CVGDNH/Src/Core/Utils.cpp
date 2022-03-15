#include "Utils.h"

boost::asio::ip::address_v6 sinaddr_to_asio(sockaddr_in6 *addr) 
{
    boost::asio::ip::address_v6::bytes_type buf;
    memcpy(buf.data(), addr->sin6_addr.s6_addr, sizeof(addr->sin6_addr));
    return boost::asio::ip::make_address_v6(buf, addr->sin6_scope_id);
}

#if defined(_WIN32)
    std::vector<boost::asio::ip::address> get_local_interfaces() 
    {
        // It's a windows machine, we assume it has 512KB free memory
        DWORD outBufLen = 1 << 19;
        AddrList ifaddrs = (AddrList) new char[outBufLen];

        std::vector<boost::asio::ip::address> res;

        ULONG err = 
            GetAdaptersAddresses(
                AF_UNSPEC,
                GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_ANYCAST, 
                NULL, 
                ifaddrs,
                &outBufLen);

        if (err == NO_ERROR) 
        {
            for (AddrList addr = ifaddrs; addr != 0; addr = addr->Next) 
            {
                if (addr->OperStatus != IfOperStatusUp) 
                    continue;

                for (Addr* uaddr = (Addr*)addr->FirstUnicastAddress; uaddr != 0; uaddr = uaddr->Next) 
                {
                    if (uaddr->Address.lpSockaddr->sa_family == AF_INET) 
                    { 
                        
                        sockaddr_in* sin = reinterpret_cast<sockaddr_in *>(uaddr->Address.lpSockaddr);
                        res.push_back(boost::asio::ip::make_address_v4(ntohl(sin->sin_addr.s_addr)));
                    }
                    else if (uaddr->Address.lpSockaddr->sa_family == AF_INET6) 
                    {
                        res.push_back(
                            sinaddr_to_asio(
                                reinterpret_cast<sockaddr_in6 *>(
                                    uaddr->Address.lpSockaddr)));
                    }
                }
            }
        } 
        //else 
        //{}

        delete[]((char *)ifaddrs);
        return res;
    }

#elif defined(__APPLE__) || defined(__linux__)

    std::vector<boost::asio::ip::address> get_local_interfaces() 
    {
        std::vector<boost::asio::ip::address> res;
        ifaddrs *ifs;
        if (getifaddrs(&ifs)) 
        {
            return res;
        }
        for (auto addr = ifs; addr != nullptr; addr = addr->ifa_next) 
        {
            // No address? Skip.
            if (addr->ifa_addr == nullptr) 
                continue;

            // Interface isn't active? Skip.
            if (!(addr->ifa_flags & IFF_UP)) 
                continue;

            if(addr->ifa_addr->sa_family == AF_INET) 
            {
                res.push_back(
                    boost::asio::ip::make_address_v4(
                        ntohl(reinterpret_cast<sockaddr_in *>(addr->ifa_addr)->sin_addr.s_addr)));
            } 
            else if(addr->ifa_addr->sa_family == AF_INET6) 
            {
                res.push_back(
                    sinaddr_to_asio(
                        reinterpret_cast<sockaddr_in6 *>(addr->ifa_addr)));
            } else 
                continue;
        }
        freeifaddrs(ifs);
        return res;
    }

#endif

/// <summary>
/// Chances are there will be several IP addresses, one or more
// for each internet adapter. We need to choose the one the typical
// (local) internet can use to reach the machine.
/// </summary>
/// <param name="sels">
/// The selections of self addresses to choose from.
/// </param>
/// <returns>
/// The best canidate for a self-address that can be shared with other
/// machines on the local network.
/// </returns>
boost::asio::ip::address GetBestSelfAddress(
    const std::vector<boost::asio::ip::address>& sels)
{
    if(sels.empty())
    {
        // Null address object.
        return boost::asio::ip::address();
    }

    // Get the first found IPv4
    for(const boost::asio::ip::address & a : sels)
    {
        if(a.is_loopback())
        {
            // This is exactly what we're trying to avoid
            continue; 
        }

        if(a.is_v4())
            return a;
    }

    // If no IPv4s, get the first anything
    for(const boost::asio::ip::address & a : sels)
    {
        if(a.is_loopback())
            continue;

        return a;
    }
    // If we're out of options, we've still go to return something
    return sels[0];
}