#pragma once

#include <string>
using std::string;

namespace files {

class file_record {
	string path, name;
	size_t timestamp;
public:
	file_record(string path, size_t ts);
	file_record(string path, string name, size_t ts);

	string get_path() { return path; }
	string get_name() { return name; }
	size_t get_timestamp() { return timestamp; }
};

} //namespace files