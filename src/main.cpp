#include <stdio.h>

#include <regex>
#include <assert.h>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

enum state_t {
    stop, running, completed, invalid
};

NLOHMANN_JSON_SERIALIZE_ENUM( state_t, {
    {invalid, "error"},
    {stop, "stopped"},
    {running, "running"},
    {completed, "completed"},
})

int main(int argc, char *argv[])
{
    json j = R"({"index":1,"value":"NA"})"_json;
    j["state"] = stop;
    auto str = j.dump(2);
    auto bson = json::to_bson(j);
    auto j2 = json::from_bson(bson);
    auto str2 = j2.dump(2);
    assert(str == str2);
    return 0;
}
