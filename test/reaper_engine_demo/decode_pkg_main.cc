#include <string>
#include <fstream>
#include <iostream>
#include <tools/debug.hh>
#include "sync_cmd.hh"

using namespace std;

static bool decodeFile(const char * fname)
{
    ifstream inf(fname);
    string buf;
    while(!inf.eof()){
        string line;
        getline(inf, line);
        buf.append(tools::UnHex(line));
    }
    CInByteStream ds(buf);
    CCmdPushData cmd;
    return decodePkg(ds, cmd);
}

int main(int argc, const char ** argv)
{
#ifdef __USE_MT_ALLOC
    cerr<<"__USE_MT_ALLOC\n";
#endif
    if(argc < 2){
        cerr<<"Usage: "<<argv[0]<<" FILE\n";
        return 1;
    }
    if(!decodeFile(argv[1]))
        return 1;
    return 0;
}

