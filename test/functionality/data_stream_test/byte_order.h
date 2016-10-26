#include <arpa/inet.h>

inline bool le(bool net)
{
    return (net ? false : tools::HostByteOrder());
}

inline bool swapnb(bool net)
{
    return (net ? 1 != ntohs(1) : false);
}

inline bool swaple(bool le)
{
    return (le != tools::HostByteOrder());
}

inline const char * endianName(bool le)
{
    return (le ? "LittleEndian" : "BigEndian");
}
