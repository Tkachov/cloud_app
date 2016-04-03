#include "storage.h"

#include <iostream>
using std::cout;
using std::pair;

#include "../files/local_storage.h"
#include "../files/file_record.h"
using namespace files;

namespace cloud {

string remove_prefix(string& prefixed, string& prefix) {
	string result = prefixed;
	size_t index = result.find(prefix);
	if (index != -1)
		return result.replace(index, prefix.size(), "");
	return result;
}

bool equal_names(file_record& a, file_record& b, string& a_root, string& b_root) {
	string a_name = remove_prefix(a.get_path(), a_root);
	string b_name = remove_prefix(b.get_path(), b_root);
	return (a_name == b_name);
}

bool storage::sync(string local_syncing_directory_root, string remote_syncing_directory_root, bool recursive) {
	//ok, so that's VERY not cool
	if (remote_syncing_directory_root == "" || remote_syncing_directory_root.back() != '/')
		remote_syncing_directory_root += '/';
	//but wait... down there comes more!

	//so, here's the plan:
	//1) list local directory
	//2) list remote directory
	//3) compose three vectors: <new_local>, <new_remote> and <duplicates>
	//4) sort these vectors (directories has shorter path than it's files, so we first create directory and then upload files)
	//5) upload all <new_local> files
	//6) download all <new_remote> files
	//7) sync all <duplicates> by comparing the date and downloading (or uploading) the newer one

	vector<file_record> local_files = local_storage::list_directory(local_syncing_directory_root, recursive);
	vector<file_record> remote_files;

	//TODO: this is just for now, we actually need vector<file_record> from list_directory
	string remote_dir = remote_syncing_directory_root;
	if (remote_dir != "" && remote_dir.back() == '/')
		remote_dir.pop_back(); //it happens Dropbox doesn't like '/' in the end and I do
	vector<string> remote_names = list_directory(remote_dir, recursive);
	for (auto name : remote_names)
		remote_files.push_back(file_record(name, 0));
	//----------

	//ok I don't like this, but I think that's more optimized
	vector<char> remote_files_found(remote_files.size(), 0); //0 means 'not found', 1 means 'found'	
	//----------

	vector<file_record> new_local_files, new_remote_files;
	vector<pair<file_record, file_record>> duplicate_files;
	for (file_record local_file : local_files) {
		bool found = false;
		size_t remote_file_index = 0;
		for (file_record remote_file : remote_files) {
			if (equal_names(local_file, remote_file, local_syncing_directory_root, remote_syncing_directory_root)) {
				duplicate_files.push_back(pair<file_record, file_record>(local_file, remote_file));
				remote_files_found[remote_file_index] = 1;
				found = true;
				break;
			}
			++remote_file_index;
		}

		if (!found) new_local_files.push_back(local_file);
	}

	for (size_t i = 0; i < remote_files.size(); ++i)
		if (remote_files_found[i] == 0)
			new_remote_files.push_back(remote_files[i]);

	//TODO: now sort the vectors
	//TODO: upload <new_local>
	//TODO: download <new_remote>
	//TODO: sync <duplicates>

	cout << "New local files:\n";
	for (auto f : new_local_files)
		cout << f.get_path() << "\n";

	cout << "\nNew remote files:\n";
	for (auto f : new_remote_files)
		cout << f.get_path() << "\n";

	cout << "\nDuplicate files:\n";
	for (auto f : duplicate_files)
		cout << f.first.get_path() << " vs. " << f.second.get_path() << "\n";

	return false;
}

} //namespace cloud