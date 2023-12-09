/* gcc -g3 -O1 -Wall -std=c99 -I/usr/local/ssl/darwin/include t-rsa.c /usr/local/ssl/darwin/lib/libcrypto.a -o t-rsa.exe */
/* gcc -g2 -Os -Wall -DNDEBUG=1 -std=c99 -I/usr/local/ssl/darwin/include t-rsa.c /usr/local/ssl/darwin/lib/libcrypto.a -o t-rsa.exe */
 
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

typedef unsigned char byte;
#define UNUSED(x) ((void)x)
const char hn[] = "SHA512";

/* Returns 0 for success, non-0 otherwise */
int make_keys(EVP_PKEY** skey, EVP_PKEY** vkey);

/* Returns 0 for success, non-0 otherwise */
int sign_it(const byte* msg, size_t mlen, byte** sig, size_t* slen, EVP_PKEY* pkey);

/* Returns 0 for success, non-0 otherwise */
int verify_it(const byte* msg, size_t mlen, const byte* sig, size_t slen, EVP_PKEY* pkey);

/* Prints a buffer to stdout. Label is optional */
int print_it(const byte* buff, size_t len);

int main(void)
{
    printf("Testing RSA functions with EVP_DigestSign and EVP_DigestVerify\n");
    
    OpenSSL_add_all_algorithms();
    
    /* Sign and Verify HMAC keys */
    EVP_PKEY *skey = NULL, *vkey = NULL;
    
    int rc = make_keys(&skey, &vkey);
    if(rc != 0)
        exit(1);
    
    if(skey == NULL)
        exit(1);
    
    if(vkey == NULL)
        exit(1);
    
    const byte msg[] = {'1', '2', '3', '4'};
    byte* sig = NULL;
    size_t slen = 0;
    
    /* Using the skey or signing key */
    rc = sign_it(msg, sizeof(msg), &sig, &slen, skey);
    if(rc == 0) {
        printf("Created signature\n");
    } else {
        printf("Failed to create signature, return code %d\n", rc);
        exit(1); /* Should cleanup here */
    }
    
    print_it(sig, slen);
    
#if 0
    /* Tamper with signature */
    printf("Tampering with signature\n");
    sig[0] ^= 0x01;
#endif
    
#if 0
    /* Tamper with signature */
    printf("Tampering with signature\n");
    sig[slen - 1] ^= 0x01;
#endif
    
    /* Using the vkey or verifying key */
    rc = verify_it(msg, sizeof(msg), sig, slen, vkey);
    if(rc == 0) {
        printf("Verified signature\n");
    } else {
        printf("Failed to verify signature, return code %d\n", rc);
    }
    
    if(sig)
        OPENSSL_free(sig);
    
    if(skey)
        EVP_PKEY_free(skey);
    
    if(vkey)
        EVP_PKEY_free(vkey);
    
    return 0;
}

int sign_it(const byte* msg, size_t mlen, byte** sig, size_t* slen, EVP_PKEY* pkey)
{
    /* Returned to caller */
    int result = -1;
    
    if(!msg || !mlen || !sig || !pkey) {
        return -1;
    }
    
    if(*sig)
        OPENSSL_free(*sig);
    
    *sig = NULL;
    *slen = 0;
    
    EVP_MD_CTX* ctx = NULL;
    
    do
    {
        ctx = EVP_MD_CTX_create();
        if(ctx == NULL) {
            printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        const EVP_MD* md = EVP_sha512();
        if(md == NULL) {
            printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        int rc = EVP_DigestInit(ctx, md);
        if(rc != 1) {
            printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        EVP_PKEY_CTX *pkey_ctx = NULL;
        rc = EVP_DigestSignInit(ctx, &pkey_ctx, md, NULL, pkey);
        if(rc != 1) {
            printf("EVP_DigestSignInit failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        rc = EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING);
        rc = EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, EVP_sha512());
        
        rc = EVP_DigestSignUpdate(ctx, msg, mlen);
        if(rc != 1) {
            printf("EVP_DigestSignUpdate failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        size_t req = 0;
        rc = EVP_DigestSignFinal(ctx, NULL, &req);
        if(rc != 1) {
            printf("EVP_DigestSignFinal failed (1), error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        if(!(req > 0)) {
            printf("EVP_DigestSignFinal failed (2), error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        *sig = OPENSSL_malloc(req);
        if(*sig == NULL) {
            printf("OPENSSL_malloc failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        *slen = req;
        rc = EVP_DigestSignFinal(ctx, *sig, slen);
        if(rc != 1) {
            printf("EVP_DigestSignFinal failed (3), return code %d, error 0x%lx\n", rc, ERR_get_error());
            break; /* failed */
        }
        
        if(rc != 1) {
            printf("EVP_DigestSignFinal failed, mismatched signature sizes %zd, %zd", req, *slen);
            break; /* failed */
        }
        
        result = 0;
        
    } while(0);
    
    if(ctx) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    
    return !!result;
}

int verify_it(const byte* msg, size_t mlen, const byte* sig, size_t slen, EVP_PKEY* pkey)
{
    /* Returned to caller */
    int result = -1;
    
    if(!msg || !mlen || !sig || !slen || !pkey) {
        return -1;
    }
    
    EVP_MD_CTX* ctx = NULL;
    
    do
    {
        ctx = EVP_MD_CTX_create();
        if(ctx == NULL) {
            printf("EVP_MD_CTX_create failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        const EVP_MD* md = EVP_sha512();
        if(md == NULL) {
            printf("EVP_get_digestbyname failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        int rc = EVP_DigestInit(ctx, md);
        if(rc != 1) {
            printf("EVP_DigestInit_ex failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        EVP_PKEY_CTX *pkey_ctx = NULL;
        rc = EVP_DigestVerifyInit(ctx, &pkey_ctx, md, NULL, pkey);
        if(rc != 1) {
            printf("EVP_DigestVerifyInit failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        rc = EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING);
        rc = EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, EVP_sha512());
        
        rc = EVP_DigestVerifyUpdate(ctx, msg, mlen);
        if(rc != 1) {
            printf("EVP_DigestVerifyUpdate failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        /* Clear any errors for the call below */
        ERR_clear_error();
        
        rc = EVP_DigestVerifyFinal(ctx, sig, slen);
        if(rc != 1) {
            printf("EVP_DigestVerifyFinal failed, error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        result = 0;
        
    } while(0);
    
    if(ctx) {
        EVP_MD_CTX_destroy(ctx);
        ctx = NULL;
    }
    
    return !!result;

}

int print_it(const byte* buff, size_t len)
{
    FILE *f = stderr;

    // 打印签名数据的C数组形式。为的是复制后贴入MCU端看那边解码是否正确
    for (size_t i = 0; i < len; i++) {
        fprintf(f, "%s", i % 16 == 0 ? "    " : " ");  // 往下都是格式控制
        fprintf(f, "0x%02X,", buff[i]);
        if (i % 16 == 15) {
            fprintf(f, "\n");
        }
    }
    fprintf(f, "\n");
    int is_ok = !ferror(f);
    is_ok &= fclose(f) == 0;  // 各种检查
    return is_ok;
}

int make_keys(EVP_PKEY** skey, EVP_PKEY** vkey)
{
    int result = -1;
    
    if(!skey || !vkey)
        return -1;
    
    if(*skey != NULL) {
        EVP_PKEY_free(*skey);
        *skey = NULL;
    }
    
    if(*vkey != NULL) {
        EVP_PKEY_free(*vkey);
        *vkey = NULL;
    }
    
    do
    {
        FILE *f;
        f = fopen("financial.pem", "rb");
        *skey = PEM_read_PrivateKey(f, NULL, NULL, NULL);
        fclose(f);
        if(*skey == NULL) {
            printf("PEM_read_PrivateKey failed (1), error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        
        f = fopen("financial_pub.pem", "rb");
        *vkey = PEM_read_PUBKEY(f, NULL, NULL, NULL);
        fclose(f);
        if(*vkey == NULL) {
            printf("EVP_PKEY_new failed (2), error 0x%lx\n", ERR_get_error());
            break; /* failed */
        }
        result = EVP_PKEY_get_bits(*skey);
        result = EVP_PKEY_get_bits(*vkey);
        result = EVP_PKEY_get_id(*skey);
        result = EVP_PKEY_get_id(*vkey);
        result = EVP_PKEY_get_base_id(*skey);
        result = EVP_PKEY_get_base_id(*vkey);

        result = 0;
    } while(0);

    return !!result;
}
