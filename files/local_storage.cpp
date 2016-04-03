#include "local_storage.h"
#include <dirent.h>

#include "../base_exception.h"

//#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

namespace files {

file_record local_storage::get_file_by_path(string path) {
	struct _stat buf;
	int result;	
	char* filename = "crt_stat.c";	
	
	result = _stat(path.c_str(), &buf);
	if (result != 0)
		throw base_exception("_stat");

	return file_record(path, buf.st_mtime); //hell yeah -- that's UNIX timestamp

	/*
	//this thingie prints date/time in "Thu Feb 07 14:39:36 2002" format
	char timebuf[26];
	errno_t err = ctime_s(timebuf, 26, &buf.st_mtime);
	printf("Time modified : %s", timebuf);
	*/
}

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

				//result.push_back(file_record(prefix + string(listed_file->d_name), listed_file->d_name, 0));
				result.push_back(get_file_by_path(prefix + string(listed_file->d_name)));
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