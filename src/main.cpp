#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

#include "json.hpp"

int main(int, char *[]) {
    auto js = R"({})"_json;
    js["name"] = "Hello";
    std::cout << js.dump(4) << std::endl;
    return 0;
}
