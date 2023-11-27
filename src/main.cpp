#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "json.hpp"

extern "C" int rsa_main(void);
extern "C" void sha_main(void);
int main(int, char *[]) {
    auto js = R"({})"_json;
    js["name"] = "Hello";
    std::cout << js.dump(4) << std::endl;
    sha_main();
    rsa_main();
    return 0;
}
