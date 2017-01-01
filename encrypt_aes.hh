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
 * @brief [Advanced Encryption Standard](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard)
 * (AES) algorithm interface.
 * @note [OpenSSL](https://www.openssl.org) is required.
 * @author Zhao DAI
 */

#ifndef DOZERG_ENCRYPTOR_AES_H_20080306
#define DOZERG_ENCRYPTOR_AES_H_20080306

#include <string>
#include <vector>
#include <cstring>          //memset
#include <openssl/aes.h>
#include "tools/ssl.hh"     //Md5

NS_SERVER_BEGIN

/**
 * @brief A convenient interface for [Advanced Encryption Standard]
 * (https://en.wikipedia.org/wiki/Advanced_Encryption_Standard) (AES) algorithm.
 * @note CEncryptorAes object can @em NOT be copied.
 */
class CEncryptorAes
{
    typedef unsigned char __Char;
    static const size_t kKeyLen = 16;
    CEncryptorAes(const CEncryptorAes &);   //disable copy and assignment
    CEncryptorAes & operator =(const CEncryptorAes &);
public:
    /// @brief Key sizes.
    enum EKeyIntensity{
        L128 = 128,
        L192 = 192,
        L256 = 256
    };
    /**
     * @brief Construct a default object.
     * Key size defaults to @c L128.
     * @n You need @ref setKey or @ref setKeyAndIntensity before encryption/decryption operations.
     */
    CEncryptorAes()
        : intens_(L128)
    {}
    /**
     * @brief Construct an object.
     * @param key Encryption key (password)
     * @param intensity Key size
     */
    CEncryptorAes(const std::string & key, EKeyIntensity intensity)
        : key_(key)
        , intens_(intensity)
    {
        updateKeys();
    }
    /**
     * @brief Set key size.
     * @param intensity Key size
     */
    void setIntensity(EKeyIntensity intensity){
        intens_ = intensity;
        updateKeys();
    }
    /**
     * @brief Set encryption key (password).
     * Password is an arbitrary string.
     * @param key Encryption key (password)
     */
    void setKey(const std::string & key){
        key_ = key;
        updateKeys();
    }
    /**
     * @brief Set encryption key (password) and key size.
     * Password is an arbitrary string.
     * @param key Encryption key (password)
     * @param intensity Key size
     */
    void setKeyAndIntensity(const std::string & key, EKeyIntensity intensity){
        key_ = key;
        intens_ = intensity;
        updateKeys();
    }
    /**
     * @name Encryption
     * @{
     * @brief Encrypt given data.
     * If @c from is non-zero, first @c from bytes of @c input will @em NOT be encrypted, i.e. data
     * between @c input.begin() and @c input.begin()+from will be copied to @c output straightly,
     * and data between @c input.begin()+from and @c input.end() will be encrypted and append to
     * @c output.
     * @param[in] input Data to be encrypted
     * @param[out] output A buffer to receive the result
     * @param[in] from Index in @c input from where data will be encrypted
     * @return
     *   @li @c 0: Succeeded
     *   @li @c -1: Invalid parameter
     */
    int encrypt(const std::vector<char> & input, std::vector<char> & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    int encrypt(const std::vector<signed char> & input, std::vector<signed char> & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    int encrypt(const std::vector<unsigned char> & input, std::vector<unsigned char> & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    int encrypt(const std::string & input, std::string & output, size_t from = 0) const{
        return encryptTemplate(input, output, from);
    }
    /**  @} */
    /**
     * @name Decryption
     * @{
     * @brief Decrypt given data.
     * If @c from is non-zero, first @c from bytes of @c input will @em NOT be decrypted, i.e. data
     * between @c input.begin() and @c input.begin()+from will be copied to @c output straightly,
     * and data between @c input.begin()+from and @c input.end() will be decrypted and append to
     * @c output.
     * @param[in] input Data to be decrypted
     * @param[out] output A buffer to receive the result
     * @param[in] from Index in @c input from where data will be decrypted
     * @return
     *   @li @c 0: Succeeded
     *   @li @c -1: Invalid parameter
     *   @li @c -2: Input data format error
     */
    int decrypt(const std::vector<char> & input, std::vector<char> & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
    int decrypt(const std::vector<signed char> & input, std::vector<signed char> & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
    int decrypt(const std::vector<unsigned char> & input, std::vector<unsigned char> & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
    int decrypt(const std::string & input, std::string & output, size_t from = 0) const{
        return decryptTemplate(input, output, from);
    }
    /**  @} */
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
    std::string     key_;
    EKeyIntensity   intens_;
    AES_KEY         enKey_;
    AES_KEY         deKey_;
};

NS_SERVER_END

#endif
