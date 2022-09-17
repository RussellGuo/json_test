#include <stdio.h>

#include <regex>
#include <assert.h>
#include <iostream>


int main(int argc, char *argv[])
{
    std::wstring src = LR"(#include __PATH__ "main.cpp";)";
    std::wsmatch m;
    std::wstring include_pattern = LR"(#\s*include\s+(__PATH__)?\s*\"(.*)\"\s*;?\s*)";
    auto ret = std::regex_match(src, m, std::wregex(include_pattern));
    if (ret) {
        assert(m.size() > 2);
        std::wstring path = m[1].str();
        std::wstring file = m[2].str();
        std::wcout << L"path: '" << path << L"', file: '" << file << L"'" << std::endl;
    }

    std::string line {"type ‘SYSENUM&’ to "};
    for(;;) {
        bool found = false;
        for (const std::string sub:{"‘","’"}) {
            auto pos = line.find(sub);
            auto len = sub.length();
            if (pos != std::string::npos) {
                found = true;
                line.replace(pos, len, "'");
            }
        }
        if (!found) {
            break;
        }
    }

    return 0;
}
