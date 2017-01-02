#ifndef DOZERG_SIGNAL_H_20130729
#define DOZERG_SIGNAL_H_20130729

/*
    信号处理相关的函数
        SAFE_PRINT          对snprintf的封装
        SAFE_PRINT_L
        CReader             文件读封装
        CSymbolInfo         获取指定地址的符号信息
//*/

#include <elf.h>
#include <link.h>
#include <cxxabi.h>         //abi::__cxa_demangle
#include "../file.hh"
#include "debug.hh"

NS_SERVER_BEGIN

#define SAFE_PRINT(buf, left, fmt, args...)    do{ \
    if(left > 0){    \
        int ret = snprintf(buf, left, fmt, ##args);    \
        if(ret < 0 || ret > left)    \
            ret = left;  \
        buf += ret;   \
        left -= ret; \
    }   \
}while(0)

#define SAFE_PRINT_L(buf, left, fmt, args...)    SAFE_PRINT(buf, left, fmt "\n", ##args)

class CReader
{
public:
    CReader()
        : file_(NULL)
        , buf_(NULL)
        , len_(0)
        , dlen_(0)
        , cur_(0)
    {}
    CReader(CFile & file, char * buf, size_t len)
        : file_(&file)
        , buf_(buf)
        , len_(len)
        , dlen_(0)
        , cur_(0)
    {}
    void setFile(CFile & file){file_ = &file;}
    void setBuf(char * buf, size_t len){
        buf_ = buf;
        len_ = len;
    }
    bool valid() const{
        return (file_ && file_->valid()
                && NULL != buf_
                && len_ > 0
                && dlen_ <= len_
                && cur_ <= dlen_);
    }
    const char * data() const{return buf_ + cur_;}
    bool getChar(char & c){
        if(!valid())
            return false;
        if(!ensure(1))
            return false;
        c = buf_[cur_++];
        return true;
    }
    template<typename Int>
    bool getHex(Int & v){
        if(!valid())
            return false;
        v = 0;
        bool ok = false;
        for(char c = 0;getChar(c);){
            int i = tools::UnHex(c);
            if(i < 0){
                putBack(1);
                break;
            }
            v = v * 16 + i;
            ok = true;
        }
        return ok;
    }
    bool skipUntil(const char * end){
        if(NULL == end || !valid())
            return false;
        for(char c;getChar(c);){
            if(NULL != std::strchr(end, c)){
                putBack(1);
                break;
            }
        }
        return true;
    }
    template<typename Int>
    bool getDec(Int & v){
        if(!valid())
            return false;
        v = 0;
        bool ok = false;
        for(char c = 0;getChar(c);){
            if(c < '0' || c > '9'){
                putBack(1);
                break;
            }
            v = v * 10 + c - '0';
            ok = true;
        }
        return ok;
    }
    size_t getBuf(char * buf, size_t len, const char * end = NULL){
        if(NULL == buf || 0 == len)
            return true;
        size_t i = 0;
        for(char c = 0;i < len && getChar(c);){
            const bool d = (NULL == end ? false : (NULL != std::strchr(end, c)));
            if(!d){
                buf[i++] = c;
            }else{
                putBack(1);
                break;
            }
        }
        if(i < len)
            buf[i] = '\0';
        return i;
    }
    bool skip(size_t sz){
        if(!valid())
            return false;
        if(!ensure(sz))
            return false;
        cur_ += sz;
        return true;
    }
    bool skipSpace(){
        if(!valid())
            return false;
        for(char c;getChar(c);){
            if(!std::isspace(c) || '\n' == c){
                putBack(1);
                break;
            }
        }
        return true;
    }
private:
    size_t left() const{return (cur_ < dlen_ ? dlen_ - cur_ : 0);}
    bool ensure(size_t sz){
        if(!valid())
            return false;
        if(sz <= left())
            return true;
        if(!readData())
            return false;
        return (sz <= left());
    }
    bool readData(){
        if(!valid())
            return false;
        //move
        if(cur_ > 0 && cur_ < dlen_)
            memmove(buf_, buf_ + cur_, dlen_ - cur_);
        dlen_ -= cur_;
        cur_ = 0;
        //read
        if(dlen_ < len_){
            ssize_t ret = file_->read(buf_ + dlen_, len_ - dlen_);
            if(ret < 0)
                return false;
            dlen_ += ret;
        }
        return true;
    }
    void putBack(size_t sz){
        assert(sz <= cur_);
        cur_ -= sz;
    }
    CReader(const CReader &);   //disable copy and assignment
    CReader operator =(const CReader &);
    //fields
    CFile * file_;
    char * buf_;
    size_t len_;
    size_t dlen_;   //data length
    size_t cur_;
};

struct CMapsLine
{
    uint64_t start_;
    uint64_t end_;
    char access_[4];
    uint64_t offset_;
    char device_[5];
    uint64_t inode_;
    //functions
    bool parse(CReader & reader, char * filename, size_t len){
        //08048000-0813a000 r-xp 00000000 08:03 24530                              /usr/local/sbin/sshd2
        if(!reader.valid())
            return false;
        if(!reader.getHex(start_)       // 08048000
                || !reader.skip(1)      // -
                || !reader.getHex(end_) // 0813a000
                || !reader.skipSpace()
                || sizeof access_ != reader.getBuf(access_, sizeof access_)  // r-xp
                || !reader.skipSpace()
                || !reader.getHex(offset_)      // 00000000
                || !reader.skipSpace()
                || sizeof device_ != reader.getBuf(device_, sizeof device_)  // 08:03
                || !reader.skipSpace()
                || !reader.getDec(inode_)       // 24530
                || !reader.skipSpace())
            return false;
        if(NULL != filename && len > 0){
            reader.getBuf(filename, len, "\n"); // /usr/local/sbin/sshd2
        }else
            reader.skipUntil("\n");
        reader.skip(1); // '\n'
        return true;
    }
};

class CSymbolInfo
{
public:
    CSymbolInfo(){reset(NULL);}
    void symbolize(void * addr){
        //prepare
        reset(addr);
        if(NULL == addr)
            return;
        //search maps
        if(!searchMaps())
            return;
        //read elf header
        if(sizeof head_ != readNoSignal(&head_, sizeof head_, 0))
            return;
        if(0 != memcmp(head_.e_ident, ELFMAG, SELFMAG))
            return;
        //read initial section entry
        if(!getSectionEntryOfIndex(0, &init_))
            return;
        //get section entry count (e_shoff is already checked in getSectionEntryOfIndex())
        entryCount_ = (head_.e_shnum > 0 ? head_.e_shnum : init_.sh_size);
        //search in regular symbol table
        if(searchInSymTable(SHT_SYMTAB))
            return;
        //search in dynamic symbol table
        searchInSymTable(SHT_DYNSYM);
    }
    const char * symbolName() const{return ('\0' != demagling_[0] ? demagling_ : symname_);}
    const char * fileName() const{return filename_;}
    bool getStackRange(){
        //open maps
        CFile maps;
        if(!maps.open("/proc/self/maps", O_RDONLY))
            return false;
        //parse line
        CReader reader(maps, symname_ + 1, sizeof symname_ - 1);
        for(CMapsLine m;reader.valid();){
            //parse line
            if(!m.parse(reader, filename_, sizeof filename_))
                return false;
            if(0 == memcmp("[stack]", filename_, 8)){
                stackStart_ = m.start_;
                stackEnd_ = m.end_;
                return true;
            }
        }
        return false;
    }
    bool isInStack(void * pc, size_t offset = 0) const{
        const uint64_t p = reinterpret_cast<uintptr_t>(pc);
        return (stackStart_ <= p && p + offset < stackEnd_);
    }
    uint64_t stackTop() const{return stackStart_;}
    bool isCode(void * pc){
        if(NULL == pc)
            return false;
        uint64_t p = reinterpret_cast<uintptr_t>(pc);
        //open maps
        CFile maps;
        if(!maps.open("/proc/self/maps", O_RDONLY))
            return false;
        //parse line
        CReader reader(maps, symname_ + 1, sizeof symname_ - 1);
        for(CMapsLine m;reader.valid();){
            //parse line
            if(!m.parse(reader, NULL, 0))
                return false;
            if(0 == memcmp("r-xp", m.access_, 4)
                    && m.start_ <= p && p < m.end_)
                return true;
        }
        return false;
    }
    bool isInDynamic() const{return (ET_DYN == head_.e_type);}
    uint64_t mapOffset() const{return mapOffset_;}
private:
    void reset(void * addr){
        filename_[0] = symname_[0] = demagling_[0] = '\0';
        pc_ = reinterpret_cast<uintptr_t>(addr);
        mapOffset_ = 0;
        stackStart_ = stackEnd_ = 0;
        entryCount_ = 0;
        file_.close();
        memset(&head_, 0, sizeof head_);
    }
    bool searchMaps(){
        //open maps
        CFile maps;
        if(!maps.open("/proc/self/maps", O_RDONLY))
            return false;
        //parse line
        CReader reader(maps, symname_ + 1, sizeof symname_ - 1);
        for(CMapsLine m;reader.valid();){
            //parse line
            if(!m.parse(reader, filename_, sizeof filename_)){
                filename_[0] = '\0';
                return false;
            }
            //check addr
            if(m.start_ <= pc_ && pc_ < m.end_){
                mapOffset_ = m.start_;
                return file_.open(filename_, O_RDONLY);
            }
        }
        filename_[0] = '\0';
        return false;
    }
    ssize_t readNoSignal(void * buf, size_t len, size_t offset){
        if(NULL == buf || !file_.seek(offset, SEEK_SET))
            return -1;
        size_t cur = 0;
        while(cur < len){
            const size_t left = len - cur;
            const ssize_t ret = file_.read(reinterpret_cast<char *>(buf) + cur, left);
            if(ret < 0){
                if(EINTR == errno)
                    continue;
                return -1;
            }
            cur += ret;
            if(size_t(ret) < left)
                break;
        }
        return cur;
    }
    bool getSectionEntryOfIndex(size_t index, ElfW(Shdr) * entry){
        if(NULL == entry || 0 == head_.e_shoff)
            return false;
        //NOTE: no range check for index
        return (sizeof *entry == readNoSignal(entry, sizeof *entry, head_.e_shoff + index * sizeof *entry));
    }
    bool searchInSymTable(ElfW(Word) type){
        //read section entry
        if(!getSectionEntryOfType(type, &symtab_))
            return false;
        if(0 == symtab_.sh_entsize)
            return false;
        //read related string table section entry
        if(!getSectionEntryOfIndex(symtab_.sh_link, &strtab_))
            return false;
        //iterate symbol table
        const size_t count = symtab_.sh_size / symtab_.sh_entsize;
        const size_t off = (ET_DYN != head_.e_type ? 0 : mapOffset_);
        for(size_t i = 0;i < count;){
            size_t num = count - i;
            if(num > ARRAY_SIZE(tmpSym_))
                num = ARRAY_SIZE(tmpSym_);
            ssize_t ret = readNoSignal(tmpSym_,
                    num * sizeof tmpSym_[0],
                    symtab_.sh_offset + i * sizeof tmpSym_[0]);
            if(ret < 0)
                return false;
            num = ret / sizeof tmpSym_[0];
            for(size_t j = 0;j < num;++j){
                const ElfW(Sym) & s = tmpSym_[j];
                if(0 == s.st_value || STT_SECTION == s.st_info)
                    continue;   //skip useless
                const uint64_t addr = s.st_value + off;
                //test address
                if((s.st_size > 0 && addr <= pc_ && pc_ < addr + s.st_size)
                        || (0 == s.st_size && addr == pc_)){
                    //read symbol name
                    ret = readNoSignal(symname_, sizeof symname_, strtab_.sh_offset + s.st_name);
                    if(ret < 0){
                        symname_[0] = '\0';
                        return false;
                    }
                    if(NULL == memchr(symname_, '\0', sizeof symname_)){
                        symname_[0] = '\0';
                        return false;
                    }
                    demagling();
                    return true;
                }
            }
            i += num;
        }
        return false;
    }
    bool getSectionEntryOfType(ElfW(Word) type, ElfW(Shdr) * entry){
        if(NULL == entry)
            return false;
        for(size_t i = 0;i < entryCount_;){
            size_t num = entryCount_ - i;
            if(num > ARRAY_SIZE(tmpEntry_))
                num = ARRAY_SIZE(tmpEntry_);
            const ssize_t ret = readNoSignal(tmpEntry_,
                    num * sizeof tmpEntry_[0],
                    head_.e_shoff + i * sizeof tmpEntry_[0]);
            if(ret < 0)
                return false;
            num = ret / sizeof tmpEntry_[0];
            for(size_t j = 0;j < num;++j)
                if(tmpEntry_[j].sh_type == type){
                    *entry = tmpEntry_[j];
                    return true;
                }
            i += num;
        }
        return false;
    }
    void demagling(){
        int status = 0;
        size_t len = sizeof demagling_;
        //NOTE:
        //If len is not large enough, demagling_ will be *free()'ed* and realloc(), that is not we want.
        abi::__cxa_demangle(symname_, demagling_, &len, &status);
        if(0 != status)
            demagling_[0] = '\0';
    }
    //fields:
    char filename_[1024];
    char symname_[1024];
    char demagling_[1024];
    ElfW(Ehdr) head_;
    ElfW(Shdr) init_;
    ElfW(Shdr) symtab_;
    ElfW(Shdr) strtab_;
    ElfW(Shdr) tmpEntry_[16];
    ElfW(Sym) tmpSym_[32];
    uint64_t pc_;
    uint64_t mapOffset_;
    uint64_t stackStart_;
    uint64_t stackEnd_;
    size_t entryCount_;
    CFile file_;
};

NS_SERVER_END

#endif

