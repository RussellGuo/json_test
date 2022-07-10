#include <stdio.h>

#include "json.hpp"

std::string hash(const std::string &filename)
{
	std::string cmd("sha256sum ");
	cmd.append(filename);

	std::string ret;
	do {
		auto f = popen(cmd.c_str(), "r");
		if (!f) {
			break;
		}
		char buf[1024];
		if (fscanf(f, "%41s", buf) == 1) {
			ret = buf;
		}
		auto r = pclose(f);
		if (r != 0) {
			ret = "";
		}
	} while(false);
	return ret;
}


template <typename consumer_t>
class deliverer_t {
public:
	deliverer_t(int v) {  }
	void run() { consumer_t(this); }
	void print_it() { printf("%s be called\n", __FUNCTION__);}

};

class consumer_t;
deliverer_t<consumer_t> x{1};

struct consumer_t {
	consumer_t(deliverer_t<consumer_t>*he) { he->print_it(); }
};
int main(int argc, char *argv[])
{
	x.run();
    auto j2 = R"(
{
	"version": "2.0.0",
	"tasks": [
		{
			"type": "cppbuild",
			"label": "C/C++: g++ build active file"
		},
		{
			"type": "2",
			"label": "2"
		}
	]
}
)"_json;
	std::string ver = j2["version"];
	auto &a =(j2["tasks"]);
	if (a.is_array()) {
		auto r = R"({ "type": "3"})"_json;
		a.push_back(r);
	}
    auto d = j2.dump(4);
    printf("%s", d.c_str());

	for (auto i = 1; i < argc; i++) {
		const char *fn = argv[i];
		auto h = hash(fn);
		printf("%s-\n", h.c_str());
	}
    return 0;
}
