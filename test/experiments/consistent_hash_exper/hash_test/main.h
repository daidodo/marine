static void printUsage(const char * exe)
{
    std::cerr<<"Usage: "<<exe<<" [NODE] [VNODE] [HASH]\n"
        <<"    NODE      node count, default "<<kNode<<"\n"
        <<"    VNODE     vnode count per node, default "<<kVNode<<"\n"
        <<"    HASH      test single hash algorithm, from 1\n"
        ;
}

static void testHash(__Hash hash, uint32_t node, uint32_t vnode)
{
    assert(hash);
    CBitset bs;
    size_t rpt = 0;
    for(uint32_t k = 0;k < node;++k){
        uint32_t last = 0;
        for(uint32_t v = 0;v < vnode;){
            const uint32_t old = last;
            last = hash(k, v, last);
            if(bs.test(last)){
                //std::cout<<"hash(key="<<k<<", index="<<v<<", last="<<old<<") = "<<last<<"\n";
                if(++rpt > 10 * node * vnode)
                    break;
            }else{
                bs.set(last);
                ++v;
            }
        }
    }
    const size_t sum = rpt + node * vnode;
   // std::cout<<"\t"<<std::setw(8)<<(double(rpt) * 10000000000. / sum / sum)
    std::cout<<"\t"<<std::setw(8)<<rpt
        <<std::flush;
    std::cout<<std::flush;
}

int main(int argc, const char ** argv)
{
    uint32_t node = kNode;
    uint32_t vnode = kVNode;
    int hash = 0;
    if(argc > 3)
        hash = ::atoi(argv[3]);
    if(argc > 2)
        vnode = ::atoi(argv[2]);
    if(argc > 1)
        node = ::atoi(argv[1]);
    if(!node || !vnode){
        printUsage(argv[0]);
        return 1;
    }
    if(hash){
        if(hash < 0 || hash > sizeof kHash / sizeof kHash[0]){
            printUsage(argv[0]);
            return 1;
        }
        testHash(kHash[hash - 1], node, vnode);
    }else{
        for(size_t h = 0;h < sizeof kHash / sizeof kHash[0];++h)
            testHash(kHash[h], node, vnode);
    }
    std::cout<<"\n";
}
