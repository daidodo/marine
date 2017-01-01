/*
 * Copyright (c) 2016 Zhao DAI <daidodo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see accompanying file LICENSE.txt
 * or <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 * @brief Configuration file parsing and loading.
 * @author Zhao DAI
 */

#ifndef DOZERG_CONFIGURATION_H_20070821
#define DOZERG_CONFIGURATION_H_20070821

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>           //std::numeric_limits
#include <cstdlib>          //atoi
#include "template.hh"      //CTypeTraits
#include "tools/string.hh"  //Trim,AbsFilename

NS_SERVER_BEGIN

//TODO: re-write unit test
//TODO: add json support, add range support(1,3-10,20,23-100)
/**
 * @brief Parse config file and get attributes.
 * A config file is a text file consisting of attributes. An attribute consists of a name and its
 * value.
 * @n There is only one attribute per line. Empty lines are allowed. Any content after @c # will be
 * treated as comments, until the end of the line.
 * @n Attribute name and value consist of any readable characters. Leading and trailing spaces will
 * be trimmed. But spaces inside of a name or value are considered part of it.
 * @n CConfiguration supports 3 formats of config files:
 *
 * @par @c kFormatEqual
 * An attribute has the format of `NAME = VALUE`, which means @c = cannot be part of an attribute
 * name or value.
 * @n Example:
 * @code
 *   # this is an example of kFormatEqual conf file.
 * first attribute name = first attribute value     # first attribute description
 *   second:attribute:name = second:attribute:value
 * third_attribute_name       # with an empty value
 * @endcode
 *
 * @par @c kFormatSpace
 * An attribute has the format of `NAME  VALUE`, which means @c SPACE cannot be part of an attribute
 * name or value.
 * @n Example:
 * @code
 *   # this is an example of kFormatSpace conf file.
 * first=attribute=name     first=attribute=value     # first attribute description
 *   second:attribute:name  second:attribute:value
 * third_attribute_name       # with an empty value
 * @endcode
 *
 * @par @c kFormatColon
 * An attribute has the format of `NAME : VALUE`, which means @c : cannot be part of an attribute
 * name or value.
 * @n Example:
 * @code
 *   # this is an example of kFormatColon conf file.
 * first attribute name : first attribute value     # first attribute description
 *   second attribute name : second attribute value
 * third_attribute_name       # with an empty value
 * @endcode
 */
class CConfiguration
{
    typedef std::map<std::string, std::string>   container_type;
public:
    static const int kFormatEqual = 0;
    static const int kFormatSpace = 1;
    static const int kFormatColon = 2;
    /**
     * @brief Get config file name.
     * @return Config file name
     */
    std::string getConfName() const{return conf_file_;}
    /**
     * @brief Clear all attributes loaded.
     * This function won't change the content of config file.
     */
    void clear(){content_.clear();}
    /**
     * @brief Parse and load content of a config file.
     * @param file_name Config file name
     * @param format Config file format, could be @c kFormatEqual, @c kFormatSpace or @c
     * kFormatColon
     * @return @c true if succeeded; otherwise @c false
     */
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
    /**
     * @brief Get string value of an attribute.
     * @param key Attribute name
     * @param strdefault Default value if @c key is not found
     * @return Value of the attribute
     */
    std::string getString(const std::string & key, const std::string & strdefault = "") const{
        container_type::const_iterator wh = content_.find(key);
        if(wh == content_.end())
            return strdefault;
        return wh->second;
    }
    /**
     * @brief Get file name value of an attribute.
     * This function considers the value to be a file name relative to the current working
     * directory, and tries to expand it to a full pathname.
     * @n If the value is empty or a full pathname already, no expansion will take place.
     * @param key Attribute name
     * @param strdefault Default value if @c key is not found, @em before expansion
     * @return Value of the attribute
     */
    std::string getFilepath(const std::string & key, const std::string & strdefault = "") const{
        const std::string v = getString(key, strdefault);
        return (v.empty() ? v : tools::AbsFilename(v));
    }
    /**
     * @name getInt
     * @{
     * @details These functions try to translate the value to an integer.
     * @n A number of suffixes (case insensitive) could appear at the end of the value:
     * | Suffix | Explanation |
     * | --- | --- |
     * | @c k or @c kb | @f$N\times1024@f$ |
     * | @c m or @c mb | @f$N\times1024^2(=1,048,576)@f$ |
     * | @c g or @c gb | @f$N\times1024^3(=1,073,741,824)@f$ |
     * | @c t or @c tb | @f$N\times1024^4(=1,099,511,627,776)@f$ |
     * | @c p or @c pb | @f$N\times1024^5(=1,125,899,906,842,624)@f$ |
     * | @c e or @c eb | @f$N\times1024^6(=1,152,921,504,606,846,976)@f$ |
     * For example, @c 2KB is equal to @c 2048.
     * @n If @c key is not found, @c ndefault will be used as an candidate;
     * @n If the returned integer would be smaller than @c min, @c min will be returned;
     * @n If the returned integer would be larger than @c max, @c max will be returned;
     * @param key Attribute name
     * @param strdefault Default value if @c key is not found, @em before range checking
     * @param min Lower boundary of the returned value
     * @param max Upper boundary of the returned value
     * @return Value of the attribute
     */
    int getInt(const std::string & key, int ndefault = 0,
        int min = std::numeric_limits<int>::min(),
        int max = std::numeric_limits<int>::max()) const
    {
        return getInt<int>(key, ndefault, min, max);
    }
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
    /**  @} */
    /**
     * @brief Get a readable description of this object.
     * @return A readable description of this object
     */
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

