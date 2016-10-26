#ifndef DOZERG_ENCRYPTOR_AES_H_20080306
#define DOZERG_ENCRYPTOR_AES_H_20080306

/*
    Aes�����㷨��װ
        CEncryptorAes
//*/

#include <string>
#include <vector>
#include <cstring>          //memset
#include <openssl/aes.h>
#include "tools/ssl.hh"     //Md5

NS_SERVER_BEGIN

class CEncryptorAes
{
    typedef unsigned char __Char;
    static const size_t kKeyLen = 16;   //Aesֻʹ��keystr_��ǰ16�ֽ�
    CEncryptorAes(const CEncryptorAes &);   //disable copy and assignment
    CEncryptorAes & operator =(const CEncryptorAes &);
public:
    enum EKeyIntensity{     //Aes֧�ֵ�3����Կǿ��
        L128 = 128,
        L192 = 192,
        L256 = 256
    };
    CEncryptorAes()
        : intens_(L128)
    {}
    CEncryptorAes(const std::string & key, EKeyIntensity intensity)
        : key_(key)
        , intens_(intensity)
    {
        updateKeys();
    }
    //���ü���ǿ��
    void setIntensity(EKeyIntensity intensity){
        intens_ = intensity;
        updateKeys();
    }
    //�����û���Կ
    void setKey(const std::string & key){
        key_ = key;
        updateKeys();
    }
    //�����û���Կ�ͼ���ǿ��
    void setKeyAndIntensity(const std::string & key, EKeyIntensity intensity){
        key_ = key;
        intens_ = intensity;
        updateKeys();
    }
    //return value for encrypt & decrypt
    //    0       success
    //    -1      param error
    //    -2      decrypt data format error
    //����/����input�д�fromƫ�ƿ�ʼ������, �������output��
    int encrypt(const std::vector<char> & input, std::vector<char> & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    int decrypt(const std::vector<char> & input, std::vector<char> & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
    int encrypt(const std::vector<signed char> & input, std::vector<signed char> & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    int decrypt(const std::vector<signed char> & input, std::vector<signed char> & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
    int encrypt(const std::vector<unsigned char> & input, std::vector<unsigned char> & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    int decrypt(const std::vector<unsigned char> & input, std::vector<unsigned char> & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
    int encrypt(const std::string & input, std::string & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    int decrypt(const std::string & input, std::string & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
private:
    void updateKeys(){
        std::string md5 = tools::Md5(key_);
        AES_set_encrypt_key(reinterpret_cast<const __Char *>(&md5[0]), intens_, &enKey_);
        AES_set_decrypt_key(reinterpret_cast<const __Char *>(&md5[0]), intens_, &deKey_);
    }
    template<class Buffer>
    int encryptTemplate(const Buffer & input, Buffer & output, size_t from) const{
        size_t inlen = input.size();
        if(inlen < from)
            return -1;
        inlen -= from;
        output.resize(from + (inlen / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE);
        std::copy(input.begin(), input.begin() + from, output.begin());
        for(;inlen >= AES_BLOCK_SIZE;inlen -= AES_BLOCK_SIZE, from += AES_BLOCK_SIZE)
            AES_encrypt(
                    reinterpret_cast<const __Char *>(&input[from]),
                    reinterpret_cast<__Char *>(&output[from]),
                    &enKey_);
        unsigned char tmp[AES_BLOCK_SIZE];
        memset(tmp, AES_BLOCK_SIZE - inlen, AES_BLOCK_SIZE);
        memcpy(tmp, &input[from], inlen);
        AES_encrypt(tmp, reinterpret_cast<__Char *>(&output[from]), &enKey_);
        return 0;
    }
    template<class Buffer>
    int decryptTemplate(const Buffer & input, Buffer & output, size_t from) const{
        size_t inlen = input.size();
        if(inlen < from || (inlen - from) % AES_BLOCK_SIZE != 0)
            return -1;
        output.resize(inlen);
        std::copy(input.begin(), input.begin() + from, output.begin());
        for(inlen -= from;inlen;inlen -= AES_BLOCK_SIZE, from += AES_BLOCK_SIZE)
            AES_decrypt(
                    reinterpret_cast<const __Char *>(&input[from]),
                    reinterpret_cast<__Char *>(&output[from]),
                    &deKey_);
        inlen = *output.rbegin();
        if(inlen == 0 || inlen > AES_BLOCK_SIZE)
            return -2;
        output.resize(output.size() - inlen);
        return 0;
    }
    std::string     key_;       //�û���Կ
    EKeyIntensity   intens_;    //��Կǿ��
    AES_KEY         enKey_;     //������Կ
    AES_KEY         deKey_;     //������Կ
};

NS_SERVER_END

#endif
