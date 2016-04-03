#include "file_record.h"

namespace files {

file_record::file_record(string pth, size_t sz, size_t ts, bool is_dir) {
	path = pth;

	name = pth;
	size_t idx = name.rfind('/');
	if (idx != -1) name = name.substr(idx);
	idx = name.rfind('\\');
	if (idx != -1) name = name.substr(idx);

	timestamp = ts;
	size = sz;
	timestamp = ts;
	directory = is_dir;
}

file_record::file_record(string pth, string nm, size_t sz, size_t ts, bool is_dir) {
	path = pth;
	name = nm;
	size = sz;
	timestamp = ts;
	directory = is_dir;
}

} //namespace files