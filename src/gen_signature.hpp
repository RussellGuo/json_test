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
    TODO: 申请的内存未释放，需要释放
 */
static inline bool sign_by_sha512_rsa2048_pkcs1_pss_padding(
    const char *rsa_private_key_pem_filename,
    const uint8_t *origin_data, size_t origin_size,
    uint8_t sign_data[SIGNATURE_LEN]) {
    // 跟签名有关的上下文
    memset(sign_data, 0, SIGNATURE_LEN);
    EVP_PKEY *key = nullptr;
    EVP_MD_CTX *digest_md_ctx = nullptr;
    const EVP_MD *md = nullptr;
    EVP_PKEY_CTX *pkey_ctx = nullptr;
    FILE *f = nullptr;
    bool is_ok = false;
    do {
        f = fopen(rsa_private_key_pem_filename, "rb");  // 打开pem文件供读入私钥
        if (f == NULL) {
            break;
        }
        key = PEM_read_PrivateKey(f, NULL, NULL, NULL);  // 读入私钥
        if (key == nullptr) {
            break;
        }
        if (EVP_PKEY_id(key) != EVP_PKEY_RSA || EVP_PKEY_bits(key) != SIGN_BITLEN) {  // 检查签名钥的规格
            break;
        }

        // 下面的访问序列是标准的签名序列
        digest_md_ctx = EVP_MD_CTX_new();
        md = EVP_sha512();
        if (digest_md_ctx == nullptr || md == nullptr) {
            break;
        }
        if (!EVP_DigestInit(digest_md_ctx, md)) {
            break;
        }
        if (!EVP_DigestSignInit(digest_md_ctx, &pkey_ctx, md, NULL, key)) {
            break;
        }
        if (!EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PADDING_MODE)) {
            break;
        }
        if (!EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, md)) {
            break;
        }
        if (!EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, RSA_PSS_SALTLEN_MAX)) {
            break;
        }
        if (!EVP_DigestSignUpdate(digest_md_ctx, origin_data, origin_size)) {
            break;
        }
        size_t sig_len = SIGNATURE_LEN;
        if (!EVP_DigestSignFinal(digest_md_ctx, sign_data, &sig_len)) {
            break;
        }
        if (sig_len != SIGNATURE_LEN) {
            break;
        }
        is_ok = true;
    } while (false);

    if (f) {
        bool r = fclose(f) == 0;
        is_ok &= r;
        f = nullptr;
    }

    // TODO: 申请资源未释放，目前没搞清楚哪些资源需要释放
    // 作为一次性签名的子函数，不释放没问题
    // 猜测是下面俩调用：
    //   EVP_MD_CTX_free(digest_md_ctx);
    //   EVP_PKEY_free(key);

    return is_ok;
}

/*
    签名验证，顾名思义对应的签名过程采用的是sha512的数据摘要算法+RSA2048的签名，填充采用PKCS1 PSS的方案
    参数 rsa_private_key_pem_filename，私钥的文件名，pem格式
    origin_data、origin_size是原始数据。不用担心超长，因为签名前会先做摘要，成为固定长度的数据
    sign_data签名本身，长度固定是RSA2048_RESULT_LEN
    返回值：true表示成功，否则表示失败
    TODO: 申请的内存未释放，需要释放
 */
static inline bool verify_sign_by_sha512_rsa2048_pkcs1_pss_padding(
    const char *rsa_public_key_pem_filename,
    const uint8_t *origin_data, size_t origin_size,
    const uint8_t sign_data[SIGNATURE_LEN]) {
    // 跟签名验证有关的上下文
    EVP_PKEY *key = nullptr;
    EVP_MD_CTX *digest_md_ctx = nullptr;
    const EVP_MD *md = nullptr;
    EVP_PKEY_CTX *pkey_ctx = nullptr;
    FILE *f = nullptr;
    bool is_ok = false;

    do {
        f = fopen(rsa_public_key_pem_filename, "rb");  // 打开pem文件供读入公钥
        if (f == NULL) {
            break;
        }
        key = PEM_read_PUBKEY(f, NULL, NULL, NULL);  // 读入公钥
        if (key == nullptr) {
            break;
        }
        if (EVP_PKEY_id(key) != EVP_PKEY_RSA || EVP_PKEY_bits(key) != SIGN_BITLEN) {  // 检查验签钥的规格
            break;
        }

        // 下面的访问序列是标准的签名验证序列
        digest_md_ctx = EVP_MD_CTX_new();
        md = EVP_sha512();
        if (digest_md_ctx == nullptr || md == nullptr) {
            break;
        }
        if (!EVP_DigestInit(digest_md_ctx, md)) {
            break;
        }
        if (!EVP_DigestVerifyInit(digest_md_ctx, &pkey_ctx, md, NULL, key)) {
            break;
        }
        if (!EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PADDING_MODE)) {
            break;
        }
        if (!EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, md)) {
            break;
        }
        if (!EVP_PKEY_CTX_set_rsa_pss_saltlen(pkey_ctx, RSA_PSS_SALTLEN_MAX)) {
            break;
        }
        if (!EVP_DigestVerifyUpdate(digest_md_ctx, origin_data, origin_size)) {
            break;
        }
        if (!EVP_DigestVerifyFinal(digest_md_ctx, sign_data, SIGNATURE_LEN)) {
            break;
        }
        is_ok = true;
    } while (0);

    if (f) {
        bool r = fclose(f) == 0;
        is_ok &= r;
        f = nullptr;
    }

    // TODO: 申请资源未释放，目前没搞清楚哪些资源需要释放
    // 作为一次性签名验证的子函数，不释放没问题
    // 猜测是下面俩调用：
    //   EVP_MD_CTX_free(digest_md_ctx);
    //   EVP_PKEY_free(key);

    return is_ok;
}

#endif
