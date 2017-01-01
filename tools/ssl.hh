#ifndef DOZERG_TOOLS_SSL_H_20130122
#define DOZERG_TOOLS_SSL_H_20130122

/*
    ssl相关函数
        Md5
*/

#include <cassert>
#include <string>
#include <vector>
#include <openssl/md5.h>    //MD5
#include "../impl/environment.hh"

NS_SERVER_BEGIN

namespace tools
{
    //计算buf的md5
    inline std::string Md5(const char * buf, size_t sz)
    {
        assert(buf);
        std::string md5(MD5_DIGEST_LENGTH, 0);
        MD5(reinterpret_cast<const unsigned char *>(buf), sz, reinterpret_cast<unsigned char *>(&md5[0]));
        return md5;
    }

    inline std::string Md5(const std::string & buf)
    {
        if(buf.empty())
            return Md5("", 0);
        return Md5(&buf[0], buf.size());
    }

    inline std::string Md5(const std::vector<char> & buf)
    {
        if(buf.empty())
            return Md5("", 0);
        return Md5(&buf[0], buf.size());
    }

}//namespace tools

NS_SERVER_END

#endif

