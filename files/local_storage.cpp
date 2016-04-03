#include "local_storage.h"
#include <dirent.h>

#include "../base_exception.h"

namespace files {

vector<file_record> local_storage::list_directory(string path, bool recursive, bool list_dots) {
	vector<file_record> result;

	vector<string> queue;
	queue.push_back(path);

	while (!queue.empty()) {
		string prefix = queue.back();
		DIR* directory = opendir(prefix.c_str());
		queue.pop_back();

		struct dirent* listed_file = NULL;
		if (directory != NULL) {
			while (listed_file = readdir(directory)) {
				if (!list_dots)
					if (string(listed_file->d_name) == ".." || string(listed_file->d_name) == ".")
						continue;

				if (recursive && listed_file->d_type == DT_DIR) { //they say it's not portable
					string new_dir = prefix + string(listed_file->d_name);
					if (new_dir == "" || new_dir.back() != '/')
						new_dir += '/';
					queue.push_back(new_dir);
				}

				result.push_back(file_record(prefix + string(listed_file->d_name), listed_file->d_name, 0));
			}

			closedir(directory);
		}
		else {
			throw base_exception("Unable to open specified directory (\"" + prefix +"\").");
		}
	}

	return result;
}

} //namespace files