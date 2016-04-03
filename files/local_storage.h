#pragma once

#include <string>
#include <vector>
using std::string;
using std::vector;

#include "file_record.h"

namespace files {

	class local_storage {
		static bool delete_regular_file(file_record& rec);
		static bool delete_file_by_record(file_record& rec);
	public:
		static file_record get_file_by_path(string path);
		static vector<file_record> list_directory(string path, bool recursive = true, bool list_dots = false);
		static bool delete_file(string path);
		static bool create_directory(string path);
	};

} //namespace files