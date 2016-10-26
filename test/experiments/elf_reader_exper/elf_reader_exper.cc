#include <elf.h>
#include <link.h>
#include <iostream>
#include <cassert>

#include <marine/file.hh>
#include <marine/data_stream.hh>
#include <marine/tools/debug.hh>
#include <marine/tools/other.hh>

using namespace std;
using namespace marine;

#ifndef __WORDSIZE
#   ifdef __x86_64__
#       define __WORDSIZE	64
#   else
#       define __WORDSIZE	32
#   endif
#endif

#ifndef __ELF_NATIVE_CLASS
#   define __ELF_NATIVE_CLASS __WORDSIZE
#endif

#ifndef _ElfW
#   define _ElfW(e,w,t)	_ElfW_1 (e, w, _##t)
#   define _ElfW_1(e,w,t)	e##w##t
#endif

ElfW(Ehdr) ehdr;
vector<char> shstrtab;  //section name string table
vector<char> strtab;    //symbol name string table
vector<char> dynstr;    //dynamic name string table
vector<ElfW(Shdr)> symtab;  //symbol table headers
vector<ElfW(Shdr)> dynsym;  //dynamic symbol table headers
vector<ElfW(Shdr)> dwarfs;  //dwarf section headers
vector<char> debug_str;     //dwarf info string table

static const char * sectionName(size_t off)
{
    assert(off < shstrtab.size());
    return &shstrtab[off];
}

static string demangleName(const char * s)
{
    string n(s);
    size_t p = n.find('@');
    string r = tools::CxxDemangle(n.substr(0, p).c_str());
    if(p != string::npos)
        r.append(n, p, string::npos);
    return r;
}

static string symbolName(size_t off)
{
    assert(off < strtab.size());
    return demangleName(&strtab[off]);
}

static string dynamicSymbolName(size_t off)
{
    assert(off < dynstr.size());
    return demangleName(&dynstr[off]);
}

template<typename T>
static bool readFrom(CFile & f, T * v, long off = -1)
{
    assert(f.valid() && v);
    if(off >= 0 && !f.seek(off, SEEK_SET))
        return false;
    return (sizeof *v == f.read(reinterpret_cast<char *>(v), sizeof *v));
}

static bool readFrom(CFile & f, vector<char> & buf, size_t sz, long off = -1)
{
    assert(f.valid());
    if(off >= 0 && !f.seek(off, SEEK_SET))
        return false;
    return f.read(buf, sz, false);
}

static bool readFrom(CFile & f, string & buf, size_t sz, long off = -1)
{
    assert(f.valid());
    if(off >= 0 && !f.seek(off, SEEK_SET))
        return false;
    return f.read(buf, sz, false);
}

static void show(const ElfW(Ehdr) & h)
{
    cout<<"---ELF Header---"
        <<"\ne_ident:\t"<<tools::Dump(h.e_ident)
        <<"\ne_type:\t\t"<<h.e_type;
    switch(h.e_type){
        case ET_NONE:cout<<" (ET_NONE)";break;
        case ET_REL:cout<<" (ET_REL)";break;
        case ET_EXEC:cout<<" (ET_EXEC)";break;
        case ET_DYN:cout<<" (ET_DYN)";break;
        case ET_CORE:cout<<" (ET_CORE)";break;
        case ET_NUM:cout<<" (ET_NUM)";break;
        case ET_LOOS ... ET_HIOS:cout<<" (OS-specific)";break;
        case ET_LOPROC ... ET_HIPROC:cout<<" (Processor-specific)";break;
        default:cout<<" (unknown)";
    }
    cout<<"\ne_machine:\t"<<h.e_machine
        <<"\ne_version:\t"<<h.e_version
        <<"\ne_entry:\t0x"<<hex<<h.e_entry<<dec
        <<"\ne_phoff:\t"<<h.e_phoff
        <<"\ne_shoff:\t"<<h.e_shoff
        <<"\ne_flags:\t"<<h.e_flags
        <<"\ne_ehsize:\t"<<h.e_ehsize
        <<"\ne_phentsize:\t"<<h.e_phentsize
        <<"\ne_phnum:\t"<<h.e_phnum
        <<"\ne_shentsize:\t"<<h.e_shentsize
        <<"\ne_shnum:\t"<<h.e_shnum
        <<"\ne_shstrndx:\t"<<h.e_shstrndx
        <<endl;
}

static void show(const ElfW(Phdr) & h)
{
    cout<<"p_type:\t\t"<<h.p_type;
    switch(h.p_type){
        case PT_NULL:cout<<" (PT_NULL)";break;
        case PT_LOAD:cout<<" (PT_LOAD)";break;
        case PT_DYNAMIC:cout<<" (PT_DYNAMIC)";break;
        case PT_INTERP:cout<<" (PT_INTERP)";break;
        case PT_NOTE:cout<<" (PT_NOTE)";break;
        case PT_SHLIB:cout<<" (PT_SHLIB)";break;
        case PT_PHDR:cout<<" (PT_PHDR)";break;
        case PT_TLS:cout<<" (PT_TLS)";break;
        case PT_NUM:cout<<" (PT_NUM)";break;
        case PT_GNU_EH_FRAME:cout<<" (PT_GNU_EH_FRAME)";break;
        case PT_GNU_STACK:cout<<" (PT_GNU_STACK)";break;
        case PT_GNU_RELRO:cout<<" (PT_GNU_RELRO)";break;
        case PT_SUNWBSS:cout<<" (PT_SUNWBSS)";break;
        case PT_SUNWSTACK:cout<<" (PT_SUNWSTACK)";break;
        default:
            if(PT_LOSUNW <= h.p_type && h.p_type <= PT_HISUNW)
                cout<<" (Sun Specific)";
            else if(PT_LOOS <= h.p_type && h.p_type <= PT_HIOS)
                cout<<" (OS specific)";
            else if(PT_LOPROC <= h.p_type && h.p_type <= PT_HIPROC)
                cout<<" (processor-specific)";
            else
                cout<<" (unknown)";
    }
    cout<<"\np_offset:\t"<<h.p_offset
        <<"\np_vaddr:\t0x"<<hex<<h.p_vaddr
        <<"\np_paddr:\t0x"<<h.p_paddr<<dec
        <<"\np_filesz:\t"<<h.p_filesz
        <<"\np_memsz:\t"<<h.p_memsz
        <<"\np_flags:\t"<<h.p_flags;
    if(h.p_flags){
        cout<<" ("
            <<(0 != (h.p_flags & PF_X) ? 'x' : '-')
            <<(0 != (h.p_flags & PF_W) ? 'w' : '-')
            <<(0 != (h.p_flags & PF_R) ? 'r' : '-')
            <<')';
    }
    cout<<"\np_align:\t"<<h.p_align
        <<endl;
}

static void show(const ElfW(Shdr) & h)
{
    cout<<"sh_name:\t"<<h.sh_name<<" ("<<sectionName(h.sh_name)<<')'
        <<"\nsh_type:\t"<<h.sh_type;
    switch(h.sh_type){
        case SHT_NULL:cout<<" (SHT_NULL)";return;
        case SHT_PROGBITS:cout<<" (SHT_PROGBITS)";break;
        case SHT_SYMTAB:cout<<" (SHT_SYMTAB)";break;
        case SHT_STRTAB:cout<<" (SHT_STRTAB)";break;
        case SHT_RELA:cout<<" (SHT_RELA)";break;
        case SHT_HASH:cout<<" (SHT_HASH)";break;
        case SHT_DYNAMIC:cout<<" (SHT_DYNAMIC)";break;
        case SHT_NOTE:cout<<" (SHT_NOTE)";break;
        case SHT_NOBITS:cout<<" (SHT_NOBITS)";break;
        case SHT_REL:cout<<" (SHT_REL)";break;
        case SHT_SHLIB:cout<<" (SHT_SHLIB)";break;
        case SHT_DYNSYM:cout<<" (SHT_DYNSYM)";break;
        case SHT_INIT_ARRAY:cout<<" (SHT_INIT_ARRAY)";break;
        case SHT_FINI_ARRAY:cout<<" (SHT_FINI_ARRAY)";break;
        case SHT_PREINIT_ARRAY:cout<<" (SHT_PREINIT_ARRAY)";break;
        case SHT_GROUP:cout<<" (SHT_GROUP)";break;
        case SHT_SYMTAB_SHNDX:cout<<" (SHT_SYMTAB_SHNDX)";break;
        case SHT_NUM:cout<<" (SHT_NUM)";break;
        case SHT_GNU_LIBLIST:cout<<" (SHT_GNU_LIBLIST)";break;
        case SHT_CHECKSUM:cout<<" (SHT_CHECKSUM)";break;
        case SHT_SUNW_move:cout<<" (SHT_SUNW_move)";break;
        case SHT_SUNW_COMDAT:cout<<" (SHT_SUNW_COMDAT)";break;
        case SHT_SUNW_syminfo:cout<<" (SHT_SUNW_syminfo)";break;
        case SHT_GNU_verdef:cout<<" (SHT_GNU_verdef)";break;
        case SHT_GNU_verneed:cout<<" (SHT_GNU_verneed)";break;
        case SHT_GNU_versym:cout<<" (SHT_GNU_versym)";break;
        default:
            if(SHT_LOSUNW <= h.sh_type && h.sh_type <= SHT_HISUNW)
                cout<<" (Sun-specific)";
            else if(SHT_LOOS <= h.sh_type && h.sh_type <= SHT_HIOS)
                cout<<" (OS-specific)";
            else if(SHT_LOPROC <= h.sh_type && h.sh_type <= SHT_HIPROC)
                cout<<" (processor-specific)";
            else if(SHT_LOUSER <= h.sh_type && h.sh_type <= SHT_HIUSER)
                cout<<" (application-specific)";
            else
                cout<<" (unknown)";
    }
    cout<<"\nsh_flags:\t"<<h.sh_flags
        <<"\nsh_addr:\t0x"<<hex<<h.sh_addr<<dec
        <<"\nsh_offset:\t"<<h.sh_offset
        <<"\nsh_size:\t"<<h.sh_size
        <<"\nsh_link:\t"<<h.sh_link
        <<"\nsh_info:\t"<<h.sh_info
        <<"\nsh_addralign:\t"<<h.sh_addralign
        <<"\nsh_entsize:\t"<<h.sh_entsize
        <<endl;
}

static void show(const ElfW(Sym) & s, bool dynamic)
{
    cout<<"st_name:\t"<<s.st_name;
    if(s.st_name){
        cout<<" (";
        if(dynamic)
            cout<<dynamicSymbolName(s.st_name);
        else
            cout<<symbolName(s.st_name);
        cout<<')';
    }
    cout<<"\nst_value:\t0x"<<hex<<s.st_value<<dec
        <<"\nst_size:\t"<<s.st_size
        <<"\nst_shndx:\t"<<s.st_shndx;
    if(SHN_ABS == s.st_shndx)
        cout<<" (SHN_ABS)";
    else if(SHN_COMMON == s.st_shndx)
        cout<<" (SHN_COMMON)";
    else if(SHN_UNDEF == s.st_shndx)
        cout<<" (SHN_UNDEF)";
    else if(SHN_XINDEX == s.st_shndx)
        cout<<" (SHN_XINDEX)";
    cout<<"\nst_info:\t0x"<<hex<<int(s.st_info)<<dec<<" (";
    int i = _ElfW(ELF, __ELF_NATIVE_CLASS, ST_BIND)(s.st_info);
    switch(i){
        case STB_LOCAL:cout<<"STB_LOCAL";break;
        case STB_GLOBAL:cout<<"STB_GLOBAL";break;
        case STB_WEAK:cout<<"STB_WEAK";break;
        case STB_NUM:cout<<"STB_NUM";break;
        default:
            if(STB_LOOS <= i && i <= STB_HIOS)
                cout<<"OS-specific";
            else if(STB_LOPROC <= i && i <= STB_HIPROC)
                cout<<"processor-specific";
            else
                cout<<"unknown";
    }
    cout<<", ";
    i = _ElfW(ELF, __ELF_NATIVE_CLASS, ST_TYPE)(s.st_info);
    switch(i){
        case STT_NOTYPE:cout<<"STT_NOTYPE";break;
        case STT_OBJECT:cout<<"STT_OBJECT";break;
        case STT_FUNC:cout<<"STT_FUNC";break;
        case STT_SECTION:cout<<"STT_SECTION";break;
        case STT_FILE:cout<<"STT_FILE";break;
        case STT_COMMON:cout<<"STT_COMMON";break;
        case STT_TLS:cout<<"STT_TLS";break;
        case STT_NUM:cout<<"STT_NUM";break;
        default:
            if(STT_LOOS <= i && i <= STT_HIOS)
                cout<<"OS-specific";
            else if(STT_LOPROC <= i && i <= STT_HIPROC)
                cout<<"processor-specific";
            else
                cout<<"unknown";
    }
    cout<<")\nst_other:\t"<<int(s.st_other)
        <<endl;
}

static void show(const char * buf, size_t sz, size_t mline = 2)
{
    assert(buf);
    cout<<"("<<sz<<")\n";
    size_t mlen = mline * 16;
    if(sz < mlen)
        cout<<tools::DumpXxd(buf, sz, false);
    else
        cout<<tools::DumpXxd(buf, mlen, false)
            <<"...\n";
}

static void show(const vector<char> & buf, size_t mline = 2)
{
    show(&buf[0], buf.size(), mline);
}

static void show(const string & buf, size_t mline = 2)
{
    show(&buf[0], buf.size(), mline);
}

static bool parseSectionNoteData(CFile & f, size_t sz, long off)
{
    string buf;
    if(!readFrom(f, buf, sz, off))
        return false;
    CInByteStream in(buf, false);
    uint32_t nameSz, descSz, type;
    if(!(in>>nameSz>>descSz>>type))
        return false;
    string name, desc;
    if(!(in>>Manip::raw(name, nameSz)>>Manip::raw(desc, descSz)))
        return false;
    cout<<"data:"
        <<"\n  type:\t\t"<<type
        <<"\n  name:\t\t"<<name.c_str()
        <<"\n  desc:\t\t";
    show(desc);
    if(in.left() > 0){
        cout<<"  LEFT:\t\t";
        show(in.data(), in.left());
    }
    cout<<'\n';
    return true;
}

static bool parseProgramHeader(CFile & f)
{
    if(!ehdr.e_phoff)
        return true;
    assert(f.valid());
    assert(ehdr.e_phentsize == sizeof(ElfW(Phdr)));
    cout<<"\n---Program Header table---";
    for(int i = 0;i < ehdr.e_phnum;++i){
        cout<<"\n["<<i<<"]\n";
        ElfW(Phdr) phdr;
        if(!readFrom(f, &phdr, ehdr.e_phoff + i * ehdr.e_phentsize)){
            cerr<<"read program header entry failed\n";
            return false;
        }
        show(phdr);
        if(phdr.p_offset && phdr.p_filesz){
            switch(phdr.p_type){
                case PT_INTERP:{
                    string path;
                    if(!readFrom(f, path, phdr.p_filesz, phdr.p_offset)){
                        cerr<<"read PT_INTERP data failed\n";
                        return false;
                    }
                    cout<<"data:\t\t"<<path.c_str()<<endl;
                    break;
                }
                case PT_NOTE:
                    if(!parseSectionNoteData(f, phdr.p_filesz, phdr.p_offset)){
                        cerr<<"read PT_NOTE data failed\n";
                        return false;
                    }
                    break;
                case PT_PHDR:
                    break;
                default:{
                    vector<char> buf;
                    if(!readFrom(f, buf, phdr.p_filesz, phdr.p_offset)){
                        cerr<<"read program header data failed\n";
                        return false;
                    }
                    cout<<"data:\t\t";
                    show(buf);
                }
            }
        }
    }
    return true;
}

static bool parseSectionNameTable(CFile & f)
{
    if(!ehdr.e_shoff)
        return true;
    assert(f.valid());
    assert(ehdr.e_shentsize == sizeof(ElfW(Shdr)));
    ElfW(Shdr) shdr;
    if(!readFrom(f, &shdr, ehdr.e_shoff + ehdr.e_shstrndx * ehdr.e_shentsize)){
        cerr<<"read section name string header failed\n";
        return false;
    }
    if(SHT_STRTAB != shdr.sh_type){
        cerr<<"type error for section name string header\n";
        return false;
    }
    if(!readFrom(f, shstrtab, shdr.sh_size, shdr.sh_offset)){
        cerr<<"read section name string table data failed\n";
        return false;
    }
    if(shdr.sh_name >= shstrtab.size()
            || 0 != strcmp(".shstrtab", sectionName(shdr.sh_name))){
        cerr<<"name error for section name string header\n";
        return false;
    }
    return true;
}

static bool parseSectionHeader(CFile & f)
{
    if(!ehdr.e_shoff)
        return true;
    assert(f.valid());
    assert(ehdr.e_shentsize == sizeof(ElfW(Shdr)));
    cout<<"\n---Section Header table---\n"
        <<"[0]\t\t(SHN_UNDEF)\n";
    for(int i = 1;i < ehdr.e_shnum;++i){
        cout<<"\n["<<i<<"]\n";
        ElfW(Shdr) shdr;
        if(!readFrom(f, &shdr, ehdr.e_shoff + i * ehdr.e_shentsize)){
            cerr<<"read section header entry failed\n";
            return false;
        }
        show(shdr);
        switch(shdr.sh_type){
            case SHT_NOTE:
                if(!parseSectionNoteData(f, shdr.sh_size, shdr.sh_offset)){
                    cerr<<"read "<<sectionName(shdr.sh_name)<<" section data failed\n";
                    return false;
                }
                break;
            case SHT_SYMTAB:
                symtab.push_back(shdr);
                break;
            case SHT_DYNSYM:
                dynsym.push_back(shdr);
                break;
            case SHT_STRTAB:
                if(0 == strcmp(".shstrtab", sectionName(shdr.sh_name))){
                    break;
                }else if(0 == strcmp(".strtab", sectionName(shdr.sh_name))){
                    if(!readFrom(f, strtab, shdr.sh_size, shdr.sh_offset)){
                        cerr<<"read "<<sectionName(shdr.sh_name)<<" section data failed\n";
                        return false;
                    }
                    break;
                }else if(0 == strcmp(".dynstr", sectionName(shdr.sh_name))){
                    if(!readFrom(f, dynstr, shdr.sh_size, shdr.sh_offset)){
                        cerr<<"read "<<sectionName(shdr.sh_name)<<" section data failed\n";
                        return false;
                    }
                    break;
                }
            case SHT_PROGBITS:
                if(0 == strcmp(".debug_abbrev", sectionName(shdr.sh_name))){
                    dwarfs.push_back(shdr);
                    break;
                }else if(0 == strcmp(".debug_aranges", sectionName(shdr.sh_name))){
                    dwarfs.push_back(shdr);
                    break;
                }else if(0 == strcmp(".debug_str", sectionName(shdr.sh_name))){
                    if(!readFrom(f, debug_str, shdr.sh_size, shdr.sh_offset)){
                        cerr<<"read "<<sectionName(shdr.sh_name)<<" section data failed\n";
                        return false;
                    }
                    break;
                }else if(0 == strcmp(".interp", sectionName(shdr.sh_name))){
                    string path;
                    if(!readFrom(f, path, shdr.sh_size, shdr.sh_offset)){
                        cerr<<"read "<<sectionName(shdr.sh_name)<<" section data failed\n";
                        return false;
                    }
                    cout<<"data:\t\t"<<path.c_str()<<endl;
                    break;
                }
            default:{
                vector<char> buf;
                if(!readFrom(f, buf, shdr.sh_size, shdr.sh_offset)){
                    cerr<<"read "<<sectionName(shdr.sh_name)<<" section data failed\n";
                    return false;
                }
                cout<<"data:\t\t";
                show(buf);
            }
        }
    }
    return true;
}

static bool parseSymbolTable(CFile & f)
{
    if(symtab.empty())
        return true;
    assert(f.valid());
    for(size_t i = 0;i < symtab.size();++i){
        const ElfW(Shdr) & h = symtab[i];
        assert(sizeof(ElfW(Sym)) == h.sh_entsize);
        int n = h.sh_size / h.sh_entsize;
        cout<<"\n---"<<sectionName(h.sh_name)<<" Symbol table---";
        for(int j = 0;j < n;++j){
            cout<<"\n["<<j<<"]\n";
            ElfW(Sym) sym;
            if(!readFrom(f, &sym, h.sh_offset + j * h.sh_entsize)){
                cerr<<"read "<<sectionName(h.sh_name)<<" symbol failed\n";
                return false;
            }
            show(sym, false);
        }
    }
    return true;
}

static bool parseDynamicSymbolTable(CFile & f)
{
    if(dynsym.empty())
        return true;
    assert(f.valid());
    for(size_t i = 0;i < dynsym.size();++i){
        const ElfW(Shdr) & h = dynsym[i];
        assert(sizeof(ElfW(Sym)) == h.sh_entsize);
        int n = h.sh_size / h.sh_entsize;
        cout<<"\n---"<<sectionName(h.sh_name)<<" Symbol table---";
        for(int j = 0;j < n;++j){
            cout<<"\n["<<j<<"]\n";
            ElfW(Sym) sym;
            if(!readFrom(f, &sym, h.sh_offset + j * h.sh_entsize)){
                cerr<<"read "<<sectionName(h.sh_name)<<" symbol failed\n";
                return false;
            }
            show(sym, true);
        }
    }
    return true;
}

/* 7.5.3 Abbreviations Tables

   [...] Each declaration begins with an unsigned LEB128 number
   representing the abbreviation code itself.  [...]  The
   abbreviation code is followed by another unsigned LEB128
   number that encodes the entry's tag.  [...]

   [...] Following the tag encoding is a 1-byte value that
   determines whether a debugging information entry using this
   abbreviation has child entries or not. [...]

   [...] Finally, the child encoding is followed by a series of
   attribute specifications. Each attribute specification
   consists of two parts. The first part is an unsigned LEB128
   number representing the attribute's name. The second part is
   an unsigned LEB128 number representing the attribute's form.  */
static bool parseDwarfAbbrev(const string & buf)
{
    CInByteStream in(buf, false);
    for(;in && in.left();){  //CUs
        cout<<"Number TAG (0x"<<hex<<in.cur()<<dec<<")\n";
        for(;in;){  //DIEs
            uint64_t code = 0, tag = 0;
            uint8_t child = 0;
            in>>Manip::varint(code)>>Manip::varint(tag)>>child;
            cout<<"  "<<code<<'\t'<<tag<<'\t'<<int(child)<<endl;
            for(;in;){  //ATTRs
                uint64_t attrname = 0, attrform = 0;
                in>>Manip::varint(attrname)>>Manip::varint(attrform);
                if(!attrname && !attrform)
                    break;
                cout<<"    "<<attrname<<'\t'<<attrform<<endl;
            }
            assert(in.left() > 0);
            if('\0' != *in.data())
                continue;
            in.skip(1);
            break;
        }
    }
    if(!in){
        cerr<<"read dwarf abbrev data failed\n";
        return false;
    }
    return true;
}

/* Each entry starts with a header:

   1. A 4-byte or 12-byte length containing the length of the
   set of entries for this compilation unit, not including the
   length field itself. [...]

   2. A 2-byte version identifier containing the value 2 for
   DWARF Version 2.1.

   3. A 4-byte or 8-byte offset into the .debug_info section. [...]

   4. A 1-byte unsigned integer containing the size in bytes of
   an address (or the offset portion of an address for segmented
   addressing) on the target system.

   5. A 1-byte unsigned integer containing the size in bytes of
   a segment descriptor on the target system.  */
static bool parseDwarfAranges(const string & buf)
{
    CInByteStream in(buf, false);
    for(;in && in.left();){ //arange entry
        //read length
        uint64_t length = 0;
        int lenSize = 4;
        {
            uint32_t t = 0;
            in>>t;
            if(t == 0xFFFFFFFFu){
                in>>length;
                lenSize = 8;
            }else if(t >= 0xFFFFFFF0u){
                cout<<"invlid debug arange length\n";
                return false;
            }else
                length = t;
        }
        //read version
        uint16_t version = 2;
        in>>version;
        if(2 != version){
            cout<<"invlid debug arange version\n";
            return false;
        }
        //read offset
        uint64_t offset = 0;
        if(4 == lenSize){
            uint32_t t = 0;
            in>>t;
            offset = t;
        }else
            in>>offset;
        //read addr & segment size
        uint8_t addr_size = lenSize, segment_size = 0;
        in>>addr_size>>segment_size;
        if(4 != addr_size && 8 != addr_size){
            cout<<"invlid debug arange addr_size\n";
            return false;
        }
        if(0 != segment_size){
            cout<<"invlid debug arange segment_size\n";
            return false;
        }
        if(!in)
            break;
        cout<<"length:\t\t"<<length
            <<"\nversion:\t"<<version
            <<"\noffset:\t\t0x"<<hex<<offset<<dec
            <<"\naddr_size:\t"<<int(addr_size)
            <<"\nsegment_size:\t"<<int(segment_size)
            <<endl;
        //align to 2*addr_size
        {
            size_t cur = in.cur();
            cur = (cur + 2 * addr_size - 1) / (2 * addr_size) * 2 * addr_size;
            in.seek(cur);
        }
        //read [address, size] pairs
        cout.fill('0');
        cout<<hex;
        for(;in;){
            uint64_t address = 0, size = 0;
            if(4 == addr_size){
                uint32_t t1 = 0, t2 = 0;
                in>>t1>>t2;
                address = t1;
                size = t2;
            }else
                in>>address>>size;
            if(!address && !size)
                break;  //end
            cout<<"  "<<setw(addr_size * 2)<<address
                <<"\t"<<setw(addr_size * 2)<<size
                <<endl;
        }
        cout<<dec;
    }
    if(!in){
        cerr<<"read dwarf aranges data failed\n";
        return false;
    }
    return true;
}

static bool parseDwarfs(CFile & f)
{
    if(dwarfs.empty())
        return true;
    assert(f.valid());
    for(size_t i = 0;i < dwarfs.size();++i){
        const ElfW(Shdr) & h = dwarfs[i];
        string buf;
        if(!readFrom(f, buf, h.sh_size, h.sh_offset)){
            cerr<<"read "<<sectionName(h.sh_name)<<" section data failed\n";
            return false;
        }
        cout<<"\n---"<<sectionName(h.sh_name)<<" section---\n";
        if(0 == strcmp(".debug_abbrev", sectionName(h.sh_name))){
            if(!parseDwarfAbbrev(buf))
                return false;
        }else if(0 == strcmp(".debug_aranges", sectionName(h.sh_name))){
            if(!parseDwarfAranges(buf))
                return false;
        }else
            cout<<tools::DumpXxd(buf, false);
    }
    return true;
}

int main(int argc, const char ** argv)
{
    //parse args
    const char * fname = argv[0];
    if(argc > 1)
        fname = argv[1];
    //open file
    CFile f(fname);
    if(!f.valid()){
        cerr<<"cannot open file '"<<fname<<"'\n";
        return 1;
    }
    //read ELF file header
    if(!readFrom(f, &ehdr)){
        cerr<<"read ELF file header failed\n";
        return 1;
    }
    show(ehdr);
    //read section name table
    if(!parseSectionNameTable(f))
        return 1;
    //read sections
    if(!parseSectionHeader(f))
        return 1;
    //read symbol table
    if(!parseSymbolTable(f))
        return 1;
    //read dynamic symbol table
    if(!parseDynamicSymbolTable(f))
        return 1;
    //read program headers
    if(!parseProgramHeader(f))
        return 1;
    //read dwarf sections
    if(!parseDwarfs(f))
        return 1;
    return 0;
}
