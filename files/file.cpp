#include "file.h"

namespace files {

	file::file(string p, size_t sz, size_t ts) {
		path = p;

		name = path;
		size_t idx = name.rfind('/');
		if (idx != -1) name = name.substr(idx+1);
		idx = name.rfind('\\');
		if (idx != -1) name = name.substr(idx+1);

		size = sz;
		timestamp = ts;
	}

	file::file(string p, string n, size_t sz, size_t ts) {
		path = path;
		name = name;
		size = sz;
		timestamp = ts;
	}

	file::~file()
	{
	}

} //namespace files