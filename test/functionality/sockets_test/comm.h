bool validBufSize(size_t sz)
{
    return (256 == sz || 2288 == sz || 2296 == sz || 2304 == sz);
}

bool validSendSize(size_t sz)
{
    return (2048 == sz || 4608 == sz);
}

