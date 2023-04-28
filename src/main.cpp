#include <assert.h>
#include <stdio.h>

#include <iostream>
#include <memory>
#include <regex>
#include <set>

#include "json.hpp"
#include "time_used.hxx"

using json = nlohmann::json;
int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " xxx.json" << std::endl;
        exit(1);
    }
    return 0;
}
