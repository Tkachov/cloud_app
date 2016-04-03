#pragma once

#include <string>
using std::string;

namespace files {

class file_record {
	string path, name;
	size_t size, timestamp;
	bool directory;
public:
	file_record(string pth, size_t sz, size_t ts, bool is_dir);
	file_record(string pth, string nm, size_t sz, size_t ts, bool is_dir);

	string get_path() { return path; }
	string get_name() { return name; }
	size_t get_size() { return size; }
	size_t get_timestamp() { return timestamp; }
	bool is_directory() { return directory; }

	void set_size(size_t sz) { size = sz; }
	void set_timestamp(size_t ts) { timestamp = ts; }
};

} //namespace files