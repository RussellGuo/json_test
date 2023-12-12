#ifndef __GEN_SIGNATURE_HPP__
#define __GEN_SIGNATURE_HPP__

/*
    PC端的签名程序
    郭强 2023-11-28 新建
    重点函数是，sign_by_sha512_rsa2048_pkcs1_padding，用于给指定的一段内存签名。
    签名方法顾名思义是sha512的数据摘要算法+RSA2048的签名。填充采用PKCS1的方案
    郭强 2023-12-08 更新
    用PSS填充方案换掉了原先的固定填充方案，后者不再被建议使用
    PSS即Probabilistic signature scheme（概率性签名方案）
    郭强 2023-12-12 更新
    兼容了Open SSL 1.0版的API
    优化可读性

 */
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

// 常数，签名规格
auto constexpr SIGN_ALG_NAME = "RSA";
auto constexpr SIGN_BITLEN = 2048;
auto constexpr SIGNATURE_LEN = 256;
auto constexpr RSA_PADDING_MODE = RSA_PKCS1_PSS_PADDING;

/*
    签名主函数，签名方法顾名思义是sha512的数据摘要算法+RSA2048的签名，填充采用PKCS1 PSS的方案
    参数 rsa_private_key_pem_filename，私钥的文件名，pem格式
    origin_data、origin_size是原始数据。不用担心超长，因为签名前会先做摘要，成为固定长度的数据
    sign_data存放签名本身，长度固定是RSA2048_RESULT_LEN
    返回值：true表示成功，否则表示失败
 */
static inline bool sign_by_sha512_rsa2048_pkcs1_pss_padding(
    const char *rsa_private_key_pem_filename,
    const uint8_t *origin_data, size_t origin_size,
    uint8_t sign_data[SIGNATURE_LEN]) {
    //
    memset(sign_data, 0, SIGNATURE_LEN);

    // 从文件中读取签名用的私钥。成功返回私钥指针，失败返回nullptr
    auto get_key_from_file = [](const char *filename) {
        EVP_PKEY *key = nullptr;
        FILE *f = fopen(filename, "rb");
        if (f) {
            key = PEM_read_PrivateKey(f, NULL, NULL, NULL);
            fclose(f);  // TODO: it might be failed, should check it
        }
        return key;
    };

    // 跟签名有关的上下文
    EVP_PKEY *key = get_key_from_file(rsa_private_key_pem_filename);
    EVP_MD_CTX *digest_md_ctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha512();
    EVP_PKEY_CTX *pkey_ctx = nullptr;

    // 一系列的检查和操作，这个操作序列经测试可用于签名过程，并能和MCU端的程序配合
    bool is_ok = key != nullptr && digest_md_ctx != nullptr && md != nullptr;
    is_ok = is_ok && EVP_PKEY_id(key) == EVP_PKEY_RSA && EVP_PKEY_bits(key) == SIGN_BITLEN;
    is_ok = is_ok && EVP_DigestInit(digest_md_ctx, md);
    is_ok = is_ok && EVP_DigestSignInit(digest_md_ctx, &pkey_ctx, md, NULL, key);
    is_ok = is_ok && EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PADDING_MODE);
    is_ok = is_ok && EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, md);
    is_ok = is_ok && EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, RSA_PSS_SALTLEN_MAX);
    is_ok = is_ok && EVP_DigestSignUpdate(digest_md_ctx, origin_data, origin_size);

    size_t sig_len = SIGNATURE_LEN;
    is_ok = is_ok && EVP_DigestSignFinal(digest_md_ctx, sign_data, &sig_len);
    is_ok = is_ok && sig_len == SIGNATURE_LEN;

    // 释放上下文和key
    EVP_MD_CTX_free(digest_md_ctx);
    EVP_PKEY_free(key);
    // TODO：不确定md是否需要释放

    return is_ok;
}

/*
    签名验证，顾名思义对应的签名过程采用的是sha512的数据摘要算法+RSA2048的签名，填充采用PKCS1 PSS的方案
    参数 rsa_private_key_pem_filename，私钥的文件名，pem格式
    origin_data、origin_size是原始数据。不用担心超长，因为签名前会先做摘要，成为固定长度的数据
    sign_data签名本身，长度固定是RSA2048_RESULT_LEN
    返回值：true表示成功，否则表示失败
 */
static inline bool verify_sign_by_sha512_rsa2048_pkcs1_pss_padding(
    const char *rsa_public_key_pem_filename,
    const uint8_t *origin_data, size_t origin_size,
    const uint8_t sign_data[SIGNATURE_LEN]) {
    //
    // 从文件中读取验证签名用的共钥。成功返回私钥指针，失败返回nullptr
    auto get_key_from_file = [](const char *filename) {
        EVP_PKEY *key = nullptr;
        FILE *f = fopen(filename, "rb");
        if (f) {
            key = PEM_read_PUBKEY(f, NULL, NULL, NULL);
            fclose(f);  // TODO: it might be failed, should check it
        }
        return key;
    };

    // 跟签名验证有关的上下文
    EVP_PKEY *key = get_key_from_file(rsa_public_key_pem_filename);
    EVP_MD_CTX *digest_md_ctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha512();
    EVP_PKEY_CTX *pkey_ctx = nullptr;

    // 一系列的检查和操作，这个操作序列经测试可用于签名验证过程，并和MCU端的程序结果一致
    bool is_ok = key != nullptr && digest_md_ctx != nullptr && md != nullptr;
    is_ok = is_ok && EVP_PKEY_id(key) == EVP_PKEY_RSA && EVP_PKEY_bits(key) == SIGN_BITLEN;
    is_ok = is_ok && EVP_DigestInit(digest_md_ctx, md);
    is_ok = is_ok && EVP_DigestVerifyInit(digest_md_ctx, &pkey_ctx, md, NULL, key);
    is_ok = is_ok && EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PADDING_MODE);
    is_ok = is_ok && EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, md);
    is_ok = is_ok && EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, RSA_PSS_SALTLEN_MAX);
    is_ok = is_ok && EVP_DigestVerifyUpdate(digest_md_ctx, origin_data, origin_size);
    is_ok = is_ok && EVP_DigestVerifyFinal(digest_md_ctx, sign_data, SIGNATURE_LEN);

    // 释放上下文和key
    EVP_MD_CTX_free(digest_md_ctx);
    EVP_PKEY_free(key);
    // TODO：不确定md是否需要释放
    return is_ok;
}

#endif
