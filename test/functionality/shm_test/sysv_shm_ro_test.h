TEST(CSysvShm, init_1)
{
    {
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_GT(0, shm.init(PATH, 1))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm.valid());

        __RwShm rwshm;
        ASSERT_LT(0, rwshm.init(PATH, 1))<<__Shm::ErrMsg();

        __Shm shm2;
        ASSERT_FALSE(shm2.valid());
        ASSERT_EQ(0, shm2.init(PATH, 1, ADDR))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm2.valid());
        ASSERT_TRUE(shm2.setDeleted());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_GT(0, shm.init(PATH, 0, ADDR, 0621))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm.valid());

        __RwShm rwshm;
        ASSERT_LT(0, rwshm.init(PATH, 0, ADDR, 0621))<<__Shm::ErrMsg();

        __Shm shm2;
        ASSERT_FALSE(shm2.valid());
        ASSERT_GT(0, shm2.init(PATH, 0, ADDR, 0642))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm2.valid());

        ASSERT_EQ(0, shm2.init(PATH, 0, NULL, 0621))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm2.valid());
        ASSERT_TRUE(shm2.setDeleted());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_GT(0, shm.init("not_exist", 1))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm.valid());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_GT(0, shm.init("not_exist", 0))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm.valid());
    }
}

TEST(CSysvShm, init_2)
{
    {
        const key_t key = 0x12345;
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_GT(0, shm.init(key))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm.valid());

        __RwShm rwshm;
        ASSERT_LT(0, rwshm.init(key))<<__Shm::ErrMsg();

        __Shm shm2;
        ASSERT_FALSE(shm2.valid());
        ASSERT_EQ(0, shm2.init(key, ADDR))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm2.valid());
        ASSERT_TRUE(shm2.setDeleted());
    }{
        const key_t key = 0x23456;
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_GT(0, shm.init(key, ADDR, 0621))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm.valid());

        __RwShm rwshm;
        ASSERT_LT(0, rwshm.init(key, ADDR, 0621))<<__Shm::ErrMsg();

        __Shm shm2;
        ASSERT_FALSE(shm2.valid());
        ASSERT_GT(0, shm2.init(key, ADDR, 0642))<<__Shm::ErrMsg();
        ASSERT_FALSE(shm2.valid());

        ASSERT_EQ(0, shm2.init(key, NULL, 0621))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm2.valid());
        ASSERT_TRUE(shm2.setDeleted());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        int ret = shm.init();
        if(ret < 0){
            cerr<<"init IPC_PRIVATE failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            ASSERT_LT(0, ret);
            ASSERT_TRUE(shm.valid());
        }
    }
}

TEST(CSysvShm, setResident)
{
    {
        const key_t key = 0x23456;
        __RwShm rwshm;
        ASSERT_LT(0, rwshm.init(key))<<__Shm::ErrMsg();

        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_EQ(0, shm.init(key))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());

        ASSERT_TRUE(shm.setResident(true));
        ASSERT_TRUE(shm.valid());

        ASSERT_TRUE(shm.setResident(false));
        ASSERT_TRUE(shm.valid());

        ASSERT_TRUE(shm.setDeleted());
    }{
        __Shm shm;
        ASSERT_FALSE(shm.valid());
        int ret = shm.init();
        if(ret < 0){
            cerr<<"init IPC_PRIVATE failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            ASSERT_TRUE(shm.setResident(true));
            ASSERT_TRUE(shm.valid());

            ASSERT_TRUE(shm.setResident(false));
            ASSERT_TRUE(shm.valid());
        }
    }
}

TEST(CSysvShm, uninit)
{
    {
        const key_t key = 0x23456;
        __RwShm rwshm;
        ASSERT_LT(0, rwshm.init(key))<<__Shm::ErrMsg();

        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_EQ(0, shm.init(key))<<__Shm::ErrMsg();
        ASSERT_TRUE(shm.valid());
        ASSERT_TRUE(shm.setDeleted());

        shm.uninit();
        ASSERT_FALSE(shm.valid());

        int ret = shm.init();
        if(ret < 0){
            cerr<<"init IPC_PRIVATE failed"<<__Shm::ErrMsg()<<endl;
            ASSERT_FALSE(shm.valid());
        }else{
            ASSERT_TRUE(shm.valid());
            shm.uninit();
            ASSERT_FALSE(shm.valid());
        }
    }
}

TEST(CSysvShm, const_access)
{
    {
        const key_t key = 0x23456;
        __RwShm rwshm;
        ASSERT_LT(0, rwshm.init(key))<<__Shm::ErrMsg();

        __Shm shm;
        ASSERT_FALSE(shm.valid());
        ASSERT_EQ(0, shm.init(key))<<__Shm::ErrMsg();
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
        int ret = shm.init();
        if(ret < 0){
            cerr<<"init IPC_PRIVATE failed"<<__Shm::ErrMsg()<<endl;
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
