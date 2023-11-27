#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "json.hpp"

extern "C" int rsa_main(void);
extern "C" void sha_main(void);
extern "C" bool sign_by_sha512_rsa2048_pkcs1_padding(
    const char *rsa_private_key_pem_filename,
    const uint8_t *origin_data, size_t origin_size,
    uint8_t sign_data[]);

int main(int, char *[]) {
    auto js = R"({})"_json;
    js["name"] = "Hello";
    std::cout << js.dump(4) << std::endl;
    sha_main();
    rsa_main();

    uint8_t sign_data[1024] = {0};
    auto ret = sign_by_sha512_rsa2048_pkcs1_padding("financial.pem", (const uint8_t *)"1234", 4, sign_data);

    for (int i = 0; i < 256; i++) {
        fprintf(stderr, "    0x%02X,", sign_data[i]);
        if (i % 16 == 15) {
            fprintf(stderr, "\n");
        }
    }

    return ret ? 0 : 1;
}
