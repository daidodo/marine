PFTEST(__Oss, ctor)
{
    __Oss oss0;
    __Oss oss1;
    __Oss oss2;
    __Oss oss3;
    __Oss oss4;
    __Oss oss5;
    __Oss oss6;
    __Oss oss7;
    __Oss oss8;
    __Oss oss9;
    __Oss oss10;
    __Oss oss11;
    __Oss oss12;
    __Oss oss13;
    __Oss oss14;
    __Oss oss15;
    __Oss oss16;
    __Oss oss17;
    __Oss oss18;
    __Oss oss19;
    __Oss oss20;
    __Oss oss21;
    __Oss oss22;
    __Oss oss23;
    __Oss oss24;
    __Oss oss25;
    __Oss oss26;
    __Oss oss27;
    __Oss oss28;
    __Oss oss29;
}

PFTEST(__Oss, out)
{
    typedef unsigned long long __Int;
    __Int v = -1;
    __Oss oss;
    for(int i = 0;i < 1000;++i){
        oss<<(v - i);
        oss.str(std::string());
    }
}
