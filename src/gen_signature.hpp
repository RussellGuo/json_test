#ifndef __GEN_SIGNATURE_HPP__
#define __GEN_SIGNATURE_HPP__

/*
    PC端的签名程序
    郭强 2023-11-28 新建
    重点函数是，sign_by_sha512_rsa2048_pkcs1_padding，用于给指定的一段内存签名。
    签名方法顾名思义是sha512的数据摘要算法+RSA2048的签名。填充采用PKCS1的方案

 */
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

// 常数，符合签名规格
#define SHA512_RESULT_LEN SHA512_DIGEST_LENGTH
#define RSA2048_RESULT_LEN 256
#define RSA_PADDING_MODE RSA_PKCS1_PADDING

/*
    RSA私钥加密函数
    参数 private_key_pem_filename 是私钥的文件名，需要是pem格式
    参数data和len是被加密的原文数据指针和长度。因为采用填充，所以长度要低于RSA2048_RESULT_LEN - 11
    encrypted是输出的加密，大小必须不低于RSA2048_RESULT_LEN
    返回值：正确的返回值应该是RSA2048_RESULT_LEN。明确的错误都是-1. 如果传入一个更长的RSA，可能需要更大的encrypte的大小
 */
inline static int private_encrypt(
    const uint8_t *data, size_t data_len,
    const char *private_key_pem_filename,
    uint8_t encrypted[RSA2048_RESULT_LEN]) {
    FILE *f = fopen(private_key_pem_filename, "rb");  // 打开pem文件供读入私钥
    if (f == NULL) {
        return -1;
    }
    RSA *rsa = PEM_read_RSAPrivateKey(f, NULL, NULL, NULL);  // 读入私钥
    fclose(f);

    if (rsa == NULL) {
        return -1;
    }

    int result = -1;

    auto size = RSA_size(rsa);
    if (size == RSA2048_RESULT_LEN) {                                                         // 需要检查一下规格，是不是RSA2048
        result = RSA_private_encrypt((int)data_len, data, encrypted, rsa, RSA_PADDING_MODE);  // 完成主要操作
    }

    RSA_free(rsa);
    return result;
}

/*
    RSA公钥验证签名函数
    参数public_key_pem_filename 是公钥钥的文件名，需要是pem格式
    参数enc_data和datalen是签名本身的数据指针和长度。在本程序中data_len应该是RSA2048_RESULT_LEN
    参数decrypted是存放输出结果的，应保障RSA2048_RESULT_LEN的长度
    返回值：-1表示出错。0 ~ RSA2048_RESULT_LEN - 11之间的数值表示解码结果。不应该有其它种类的返回值
 */
inline static int public_decrypt(
    const uint8_t *enc_data, size_t data_len,
    const char *public_key_pem_filename,
    uint8_t decrypted[RSA2048_RESULT_LEN]) {
    FILE *f = fopen(public_key_pem_filename, "rb");  // 打开pem文件供读入公钥
    if (f == NULL) {
        return -1;
    }
    RSA *rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);  // 读入公钥
    fclose(f);

    if (rsa == NULL) {
        return -1;
    }

    int result = -1;
    auto size = RSA_size(rsa);
    if (size == RSA2048_RESULT_LEN) {                                                            // 需要检查一下规格，是不是RSA2048
        result = RSA_public_decrypt((int)data_len, enc_data, decrypted, rsa, RSA_PADDING_MODE);  // 用公钥解密
    }
    RSA_free(rsa);
    return result;
}

/*
    签名主函数，签名方法顾名思义是sha512的数据摘要算法+RSA2048的签名，填充采用PKCS1的方案
    参数 rsa_private_key_pem_filename，私钥的文件名，pem格式
    origin_data、origin_size是原始数据。不用担心超长，因为签名前会先做摘要，成为固定长度的数据
    sign_data存放签名本身，长度固定是RSA2048_RESULT_LEN
    返回值：true表示成功，否则表示失败
 */
static inline bool sign_by_sha512_rsa2048_pkcs1_padding(
    const char *rsa_private_key_pem_filename,
    const uint8_t *origin_data, size_t origin_size,
    uint8_t sign_data[RSA2048_RESULT_LEN]) {
    //
    uint8_t sha512_result[SHA512_DIGEST_LENGTH] = {0};
    bool is_ok = SHA512(origin_data, origin_size, sha512_result) != NULL;  // 算摘要
    if (!is_ok) {
        return false;
    }
    int encrypted_length = private_encrypt(sha512_result, sizeof sha512_result,
                                           rsa_private_key_pem_filename, sign_data);  // 对摘要加密（也就是签名）
    return encrypted_length == RSA2048_RESULT_LEN;
}

/*
    签名验证，顾名思义对应的签名过程采用的是sha512的数据摘要算法+RSA2048的签名，填充采用PKCS1的方案
    参数 rsa_private_key_pem_filename，私钥的文件名，pem格式
    origin_data、origin_size是原始数据。不用担心超长，因为签名前会先做摘要，成为固定长度的数据
    sign_data签名本身，长度固定是RSA2048_RESULT_LEN
    返回值：true表示成功，否则表示失败

 */
static inline bool verify_sign_by_sha512_rsa2048_pkcs1_padding(
    const char *rsa_public_key_pem_filename,
    const uint8_t *origin_data, size_t origin_size,
    const uint8_t sign_data[RSA2048_RESULT_LEN]) {
    //
    uint8_t origin_data_sha512_result[SHA512_DIGEST_LENGTH] = {0};
    bool is_ok = SHA512(origin_data, origin_size, origin_data_sha512_result) != NULL;  // 算原始摘要
    if (!is_ok) {
        return false;
    }
    uint8_t decrypted_data_sha512_result[SHA512_DIGEST_LENGTH] = {0};
    int decrypted_length = public_decrypt(sign_data, RSA2048_RESULT_LEN,
                                          rsa_public_key_pem_filename, decrypted_data_sha512_result);  // 解密（也就是从签名还原摘要）
    is_ok = decrypted_length == SHA512_DIGEST_LENGTH; 
    if (!is_ok) {
        return false;
    }
    is_ok = memcmp(decrypted_data_sha512_result, origin_data_sha512_result, SHA512_DIGEST_LENGTH) == 0; // 对比原始摘要和解密出来的摘要
    return is_ok;
}

#endif
