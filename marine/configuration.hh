#ifndef DOZERG_CONFIGURATION_H_20070821
#define DOZERG_CONFIGURATION_H_20070821

/*
    配置文件读取
        CConfiguration
    TODO:
        add json support
        add range support(1,3-10,20,23-100)
//*/

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>           //std::numeric_limits
#include <cstdlib>          //atoi
#include "template.hh"      //CTypeTraits
#include "tools/string.hh"  //Trim,AbsFilename

/*
配置文件格式：

---- kFormatEqual ----
[SPACE] # comments [ENDLINE]
[SPACE] expression [SPACE] = [SPACE] results [SPACE] [# comments] [ENDLINE]
[SPACE] expression [SPACE] = [SPACE] [# comments] [ENDLINE]
[SPACE] expression [SPACE] [# comments] [ENDLINE]

---- kFormatSpace ----
[SPACE] # comments [ENDLINE]
[SPACE] expression [SPACE] results [SPACE] [# comments] [ENDLINE]
[SPACE] expression [SPACE] [# comments] [ENDLINE]

---- kFormatColon ----
[SPACE] # comments [ENDLINE]
[SPACE] expression [SPACE] : [SPACE] results [SPACE] [# comments] [ENDLINE]
[SPACE] expression [SPACE] : [SPACE] [# comments] [ENDLINE]
[SPACE] expression [SPACE] [# comments] [ENDLINE]

//*/

NS_SERVER_BEGIN

//TODO: re-write unit test
class CConfiguration
{
    typedef std::map<std::string, std::string>   container_type;
public:
    static const int kFormatEqual = 0;
    static const int kFormatSpace = 1;
    static const int kFormatColon = 2;
    //得到配置文件名
    std::string getConfName() const{return conf_file_;}
    //清除读入的配置项
    void clear(){content_.clear();}
    //从文件中加载配置项
    bool load(const std::string & file_name, int format = kFormatSpace){
        std::string abs_file = tools::AbsFilename(file_name);
        std::ifstream inf(abs_file.c_str());
        if(!inf.is_open())
            return false;
        clear();
        for(std::string line;!inf.eof();){
            std::getline(inf, line);
            parseFormat(line.substr(0, line.find_first_of("#")), format);
        }
        conf_file_ = abs_file;
        return true;
    }
    //得到配置项的字符串值
    std::string getString(const std::string & key, const std::string & strdefault = "") const{
        container_type::const_iterator wh = content_.find(key);
        if(wh == content_.end())
            return strdefault;
        return wh->second;
    }
    //得到配置项的字符串值，如果需要，转换成文件绝对路径
    std::string getFilepath(const std::string & key, const std::string & strdefault = "") const{
        const std::string v = getString(key, strdefault);
        return (v.empty() ? v : tools::AbsFilename(v));
    }
    //得到配置项的int值
    int getInt(const std::string & key, int ndefault = 0,
        int min = std::numeric_limits<int>::min(),
        int max = std::numeric_limits<int>::max()) const
    {
        return getInt<int>(key, ndefault, min, max);
    }
    //得到配置项的整数值
    //支持格式示例:
    //  "123"   --->    123
    //  "123k"  --->    123 * 1024
    //  "123kb" --->    同上
    template<typename Int>
    Int getInt(const std::string & key, Int ndefault = 0,
        Int min = std::numeric_limits<Int>::min(),
        Int max = std::numeric_limits<Int>::max()) const
    {
        container_type::const_iterator wh = content_.find(key);
        Int ret = ndefault;
        if(wh != content_.end()){
            if(CTypeTraits<Int>::kIsChar){
                ret = atoi(wh->second.c_str());
            }else{
                std::istringstream iss(wh->second);
                iss>>ret;
            }
            if(ret && wh->second.size() > 0){
                char c = *wh->second.rbegin();
                if(('b' == c || 'B' == c)   //TODO: unit test
                        && wh->second.size() > 1)
                    c = *(wh->second.rbegin() + 1);
                switch(c){
                    case 'k':case 'K':
                        ret <<= 10;
                        break;
                    case 'm':case 'M':
                        ret <<= 20;
                        break;
                    case 'g':case 'G':
                        ret <<= 30;
                        break;
                    case 't':case 'T':
                        ret *= (1ull << 40);
                        break;
                    case 'p':case 'P':
                        ret *= (1ull << 50);
                        break;
                    case 'e':case 'E':
                        ret *= (1ull << 60);
                        break;
                    default:;
                }
            }
        }
        return (ret <= min ? min : (ret >= max ? max : ret));
    }
    //输出可读字符串
    //TODO: unit test
    std::string toString() const{
        CToString oss;
        oss<<"{conf_file_="<<conf_file_
            <<", content_={";
        int s = 0;
        for(container_type::const_iterator i = content_.begin();i != content_.end();++i, ++s){
            if(s)
                oss<<", ";
            oss<<"\""<<i->first<<"\"=\""<<i->second;
        }
        oss<<"}}";
        return oss.str();
    }
private:
    void parseFormat(const std::string & line, int format){
        if(line.empty())
            return;
        switch(format){
            case kFormatEqual:{
                std::string::size_type i = line.find_first_of("=");
                content_[tools::Trim(line.substr(0, i))] = tools::Trim((std::string::npos == i ? "" : line.substr(i + 1)));
                break;}
            case kFormatSpace:{
                std::string::size_type i = line.find_first_of(" \t");
                content_[tools::Trim(line.substr(0, i))] = tools::Trim((std::string::npos == i ? "" : line.substr(i + 1)));
                break;}
            case kFormatColon:{
                std::string::size_type i = line.find_first_of(":");
                content_[tools::Trim(line.substr(0, i))] = tools::Trim((std::string::npos == i ? "" : line.substr(i + 1)));
                break;}
            default:;
        }
    }
    //members
    std::string     conf_file_;
    container_type  content_;
};

NS_SERVER_END

#endif

