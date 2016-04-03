#include "file_record.h"

namespace files {

file_record::file_record(string path, size_t ts) {
	this->path = path;

	name = path;
	size_t idx = name.rfind('/');
	if (idx != -1) name = name.substr(idx);
	idx = name.rfind('\\');
	if (idx != -1) name = name.substr(idx);

	timestamp = ts;
}

file_record::file_record(string path, string name, size_t ts) {
	this->path = path;
	this->name = name;
	timestamp = ts;
}

} //namespace files