#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "file_record.h"

namespace files {

	class local_storage {
	public:
		static vector<file_record> list_directory(string path, bool recursive = true, bool list_dots = false);		
	};

} //namespace files