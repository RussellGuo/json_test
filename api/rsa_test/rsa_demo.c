#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define RSA_PADDING_MODE RSA_PKCS1_PADDING

inline static int private_encrypt(unsigned char *data, size_t data_len, const char *key, unsigned char *encrypted)
{
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
    return result;
}

inline static int public_decrypt(unsigned char *enc_data, size_t data_len, const char *key, unsigned char *decrypted)
{
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

int main(void)
{
    unsigned char plainText[2048/8]; //key length : 2048
    char publicKey[] = "rsapub.key";
    char privateKey[]= "rsa.key";

    unsigned char  encrypted[4098];
    unsigned char decrypted[4098];

    memset(plainText, 0, sizeof plainText);
    strcpy((char *)plainText, "Hello! This is an example of RSA");
    int encrypted_length, decrypted_length;

    encrypted_length= private_encrypt(plainText, strlen((const char *)plainText), privateKey, encrypted);
    if (encrypted_length == -1) {
        exit(1);
    }
    printf("Encrypted length =%d\n", encrypted_length);

    decrypted_length = public_decrypt(encrypted, (size_t)encrypted_length, publicKey, decrypted);
    if (decrypted_length == -1) {
        exit(1);
    }
    printf("Decrypted Text =%s\n",decrypted);
    printf("Decrypted Length =%d\n",decrypted_length);

    return 0;
}
