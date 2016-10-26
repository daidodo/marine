PFINIT()
{
    for(int i = 0;i < 900;++i)
        Attr_API(i, 1);
}

PFTEST(NAME, add)
{
    for(int i = 0;i < 10000;++i)
        Attr_API(12345, 1);
}
