#include <stdio.h>

#include <regex>
#include <assert.h>
#include <iostream>


int main(int argc, char *argv[])
{
	std::string src = R"(use __PATH__ "main.tdl";)";
	std::smatch m;
	std::string include_pattern = R"(\s*((use)|(\$\s*include))\s+(__PATH__)?\s*\"(.*)\"\s*;?\s*)";
	auto ret = std::regex_match(src, m, std::regex(include_pattern));
	if (ret) {
		assert(m.size() > 5);
		std::string path = m[4].str();
		std::string file  = m[5].str();
		std::cout << "path: '" << path <<"', file: '" << file << "'" << std::endl;
	}

	return 0;
}
