#include <assert.h>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#include <set>

#include "json.hpp"
#include "time_used.hxx"

using json = nlohmann::json;

static json get_json_from_file(const std::string & file_name)
{
    std::ifstream f(file_name);
    json j;
    f >> j;
    f.close();
    return j;
}

struct json_statment_t {
    const std::string var_name;
    const std::string statment;
};

static int count = 0;
std::string get_var_name()
{
    std::string name = "j" + std::to_string(count++);
    return name;
}

static json_statment_t get_cpp_statment_of_json(const json &j) {
    std::string name;
    std::string statment;
    bool is_array = j.is_array();
    bool is_int = j.is_number_integer();
    bool is_unsigned = j.is_number_unsigned();
    bool is_float = j.is_number_float();
    bool is_string = j.is_string();
    bool is_object = j.is_object();

    if (is_object) {
        name = get_var_name();
        std::string sub_item_string, sub_item_append;
        for (const auto &item:j.items()) {
            auto j_st = get_cpp_statment_of_json(item.value());
            sub_item_string += j_st.statment;
            sub_item_append += "    ret[\"" +item.key() + "\"] = " + j_st.var_name + ";\n";
        }
        statment += sub_item_string + "static json " + name + "()\n{\n    json ret;\n" + sub_item_append + "\n    return ret;\n}\n\n";
        name += "()";
    } else if (is_array) {
        name = get_var_name();
        std::string sub_item_string, sub_item_append;
        for (const auto &item:j) {
            auto j_st = get_cpp_statment_of_json(item);
            sub_item_string += j_st.statment;
            sub_item_append += "    ret.push_back(" + j_st.var_name + ");\n";
        }
        statment += sub_item_string + "static json " + name + "()\n{\n    json ret;\n" + sub_item_append + "\n    return ret;\n}\n\n";
        name += "()";
    } else if (is_int) {
        name = "json(" + std::to_string(j.get<int>()) + ")";
    } else if (is_unsigned) {
        name = "json(" + std::to_string(j.get<unsigned>()) + ")";
    } else if (is_float) {
        name = "json(" + std::to_string(j.get<double>()) + ")";
    } else if (is_string) {
        name = "json(\"" + j.get<std::string>() + "\")";
    } else {
        assert(false);
        statment = "";
    }

    return { name, statment };
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " xxx.json" << std::endl;
        exit(1);
    }

    json j = get_json_from_file(argv[1]);

    std::ofstream f("dump.json", std::ios::out);
    f << j.dump(4);
    f.close();

    auto ret = get_cpp_statment_of_json(j);

    std::cout << "#include <iostream>\n#include \"json.hpp\"\nusing json = nlohmann::json;\n";
    std::cout << ret.statment;
    std::cout << "\n\n\nint main()\n{\n";
    std::cout << "    std::cout << " + ret.var_name + ".dump(4);\n";
    std::cout << "    return 0;\n}\n";

    return 0;
}
