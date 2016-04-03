#include "local_storage.h"
#include <dirent.h>

#include "../base_exception.h"

//#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <direct.h>

#include <iostream>
using std::cout;

namespace files {

file_record local_storage::get_file_by_path(string path) {	
	struct _stat buf;
	int result = _stat(path.c_str(), &buf);
	if (result != 0)
		throw base_exception("_stat");

	return file_record(path, buf.st_size, buf.st_mtime, S_ISDIR(buf.st_mode)); //hell yeah -- that's UNIX timestamp

	/*
	//this thingie prints date/time in "Thu Feb 07 14:39:36 2002" format
	char timebuf[26];
	errno_t err = ctime_s(timebuf, 26, &buf.st_mtime);
	printf("Time modified : %s", timebuf);
	*/
}

vector<file_record> local_storage::list_directory(string path, bool recursive, bool list_dots) {
	vector<file_record> result;

	if (path == "" || path.back() != '/')
		path += '/';

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

bool local_storage::delete_regular_file(file_record& rec) {
	if (rec.is_directory()) {
		cout << "\tfailed to remove " << rec.get_path() << " (is a directory)\n";
		return false;
	}
	bool res = remove(rec.get_path().c_str()) == 0; //from cstdio
	if (!res) cout << "\tfailed to remove " << rec.get_path() << "\n";
	return res;
}

bool local_storage::delete_file_by_record(file_record& rec) {
	if (rec.is_directory()) {
		vector<file_record> listed = list_directory(rec.get_path());
		for (file_record& f : listed)
			if (!delete_file_by_record(f))
				cout << "failed to remove " << f.get_path() << "\n";
		bool res = _rmdir(rec.get_path().c_str()) == 0;
		if(!res) cout << "\tfailed to remove " << rec.get_path() << "\n";
		return res;
	} else {
		return delete_regular_file(rec);		
	}

	return false;
}

bool local_storage::delete_file(string path) {
	try {
		file_record rec = get_file_by_path(path);
		return delete_file_by_record(rec);
	} catch(base_exception& e) {
		cout << "exception " << e.what() << "\n";
	} catch (...) {
		//stat file not found => cannot be deleted => false
		cout << "unknown exception\n";
	}
	cout << "\tfailed to remove " << path << " (stat / not found)\n";
	return false;
}

bool local_storage::create_directory(string path) {
	return _mkdir(path.c_str()) == 0;
}

} //namespace files