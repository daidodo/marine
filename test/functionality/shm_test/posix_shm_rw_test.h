TEST(CPosixShm, init_1)
{
    {
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_LT(0, shm.init(PATH, kSize))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());

        __Shm shm2;
        ASSERT_FALSE(shm2.valid());
        ASSERT_EQ(0, shm2.init(PATH, kSize, ADDR))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm2.valid());
        ASSERT_TRUE(shm2.setDeleted());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_LT(0, shm.init(PATH, kSize, ADDR, 0621))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());

        __Shm shm2;
        ASSERT_FALSE(shm2.valid());
        ASSERT_EQ(0, shm2.init(PATH, kSize, ADDR, 0642))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm2.valid());

        __Shm shm3;
        ASSERT_EQ(0, shm3.init(PATH, kSize, NULL, 0621))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm3.valid());
        ASSERT_TRUE(shm3.setDeleted());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        int ret = shm.init(kSize);
        if(ret < 0){
            cerr<<"init MAP_ANONYMOUS failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            ASSERT_LT(0, ret);
            ASSERT_TRUE(shm.valid());
        }
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        int ret = shm.init(kSize, ADDR);
        if(ret < 0){
            cerr<<"init MAP_ANONYMOUS failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            ASSERT_LT(0, ret);
            ASSERT_TRUE(shm.valid());
        }
    }
}

TEST(CPosixShm, setResident)
{
    {
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_LT(0, shm.init(PATH, kSize))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());

        ASSERT_TRUE(shm.setResident(true));
        ASSERT_TRUE(shm.valid());

        ASSERT_TRUE(shm.setResident(false));
        ASSERT_TRUE(shm.valid());

        ASSERT_TRUE(shm.setDeleted());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        int ret = shm.init(kSize);
        if(ret < 0){
            cerr<<"init MAP_ANONYMOUS failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            ASSERT_TRUE(shm.setResident(true));
            ASSERT_TRUE(shm.valid());

            ASSERT_TRUE(shm.setResident(false));
            ASSERT_TRUE(shm.valid());
        }
    }
}

TEST(CPosixShm, uninit)
{
    {
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_LT(0, shm.init(PATH, kSize))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());
        ASSERT_TRUE(shm.setDeleted());

        shm.uninit();
        ASSERT_FALSE(shm.valid());

        int ret = shm.init(kSize);
        if(ret < 0){
            cerr<<"init MAP_ANONYMOUS failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            ASSERT_TRUE(shm.valid());
            shm.uninit();
            ASSERT_FALSE(shm.valid());
        }
    }
}

TEST(CPosixShm, const_access)
{
    {
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_LT(0, shm.init(PATH, kSize))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());
        ASSERT_TRUE(shm.setDeleted());

        const __Shm & s = shm;
        //size
        ASSERT_EQ(kSize, shm.size());
        //ptr
        const CTest * ptr = s.ptr();
        ASSERT_TRUE(NULL != ptr);
        CTest t;
        for(size_t i = 0;i < shm.size();++i)
            t = ptr[i];
        //operator []
        for(size_t i = 0;i < shm.size();++i)
            t = s[i];
        //front
        t = s.front();
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        int ret = shm.init(kSize);
        if(ret < 0){
            cerr<<"init MAP_ANONYMOUS failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            const __Shm & s = shm;
            //size
            ASSERT_EQ(kSize, shm.size());
            //ptr
            const CTest * ptr = s.ptr();
            ASSERT_TRUE(NULL != ptr);
            CTest t;
            for(size_t i = 0;i < shm.size();++i)
                t = ptr[i];
            //operator []
            for(size_t i = 0;i < shm.size();++i)
                t = s[i];
            //front
            t = s.front();
        }
    }
}

TEST(CPosixShm, access)
{
    {
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_LT(0, shm.init(PATH, kSize))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());
        ASSERT_TRUE(shm.setDeleted());

        __Shm & s = shm;
        //size
        ASSERT_EQ(kSize, shm.size());
        //ptr
        CTest * ptr = s.ptr();
        ASSERT_TRUE(NULL != ptr);
        CTest t;
        t.a = t.b = 1;
        t.c = 1;
        for(size_t i = 0;i < shm.size();++i)
            ptr[i] = t;
        //operator []
        for(size_t i = 0;i < shm.size();++i)
            s[i] = t;
        //front
        s.front() = t;
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        int ret = shm.init(kSize);
        if(ret < 0){
            cerr<<"init MAP_ANONYMOUS failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            __Shm & s = shm;
            //size
            ASSERT_EQ(kSize, shm.size());
            //ptr
            CTest * ptr = s.ptr();
            ASSERT_TRUE(NULL != ptr);
            CTest t;
            t.a = t.b = 1;
            t.c = 1;
            for(size_t i = 0;i < shm.size();++i)
                ptr[i] = t;
            //operator []
            for(size_t i = 0;i < shm.size();++i)
                s[i] = t;
            //front
            s.front() = t;
        }
    }
}
