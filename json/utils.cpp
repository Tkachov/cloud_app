#include "utils.h"

namespace json {

string get_value(string json, string name) throw (json_exception) {
	name += "\": \"";

	size_t begin = json.find(name);
	if (begin == string::npos) return "";

	begin += name.size();
	size_t end = json.find("\"", begin);
	if (end == string::npos) return "";

	return json.substr(begin, end - begin);
}

} //namespace json