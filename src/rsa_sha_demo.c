#include <fcntl.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define RSA_PADDING_MODE RSA_PKCS1_PADDING

inline static int private_encrypt(uint8_t *data, size_t data_len, const char *key, uint8_t *encrypted) {
    FILE *f = fopen(key, "rb");
    if (f == NULL) {
        return -1;
    }
    RSA *rsa = PEM_read_RSAPrivateKey(f, NULL, NULL, NULL);
    fclose(f);

    if (rsa == NULL) {
        return -1;
    }

    int result = RSA_private_encrypt((int)data_len, data, encrypted, rsa, RSA_PADDING_MODE);

    RSA_free(rsa);
    return result;
}

inline static int public_decrypt(uint8_t *enc_data, size_t data_len, const char *key, uint8_t *decrypted) {
    FILE *f = fopen(key, "rb");
    if (f == NULL) {
        return -1;
    }
    RSA *rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
    fclose(f);

    if (rsa == NULL) {
        return -1;
    }

    int result = RSA_public_decrypt((int)data_len, enc_data, decrypted, rsa, RSA_PADDING_MODE);
    return result;
}

int rsa_main(void) {
    uint8_t plainText[] = "1234";
    char publicKey[] = "financial_pub.pem";
    char privateKey[] = "financial.pem";

    uint8_t encrypted[4096] = {0};
    uint8_t decrypted[4096] = {0};

    int encrypted_length, decrypted_length;

    printf("Plain Text = '%s'\n", plainText);
    encrypted_length = private_encrypt(plainText, (sizeof plainText) - 1, privateKey, encrypted);
    if (encrypted_length == -1) {
        printf("private_encrypt error\n");
        exit(1);
    }
    printf("Encrypted length = %d\n", encrypted_length);

    decrypted_length = public_decrypt(encrypted, (size_t)encrypted_length, publicKey, decrypted);
    if (decrypted_length == -1) {
        printf("private_decrypt error\n");
        exit(1);
    }
    printf("Decrypted Text = '%s'\n", decrypted);
    printf("Decrypted Length = %d\n", decrypted_length);

    return 0;
}

void sha_main(void) {
    uint8_t sha512_result[SHA512_DIGEST_LENGTH] = {0};
    bool is_ok = SHA512((const uint8_t *)"1234", 4, sha512_result) != NULL;
    (void)is_ok;
    memset(sha512_result, 0, sizeof sha512_result);
}
