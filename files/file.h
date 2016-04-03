#pragma once

#include <string>
using std::string;

namespace files {

class file {
protected:
	string path, name;
	size_t size;
	size_t timestamp;

public:
	file(string p, size_t sz, size_t ts);
	file(string p, string n, size_t sz, size_t ts);
	virtual ~file();

	string get_path() { return path; }
	string get_name() { return name; }
	size_t get_size() { return size; }
	size_t get_timestamp() { return timestamp; }
};

} //namespace files