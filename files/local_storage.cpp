#include "local_storage.h"
#include <dirent.h>

#include "../base_exception.h"

namespace files {

vector<file_record> local_storage::list_directory(string path, bool recursive, bool list_dots) {
	vector<file_record> result;

	//TODO: recursive

	DIR* directory = opendir(path.c_str());
	struct dirent* listed_file = NULL;
	if(directory != NULL) {
		while(listed_file = readdir(directory)) {
			if (!list_dots)
				if (string(listed_file->d_name) == ".." || string(listed_file->d_name) == ".")
					continue;
			result.push_back(file_record(listed_file->d_name, 0));
		}

		closedir(directory);
	} else {
		throw base_exception("Unable to open specified directory.");
	}

	return result;
}

} //namespace files