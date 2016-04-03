#pragma once
#include <fstream>
#include <string>
#include <vector>
using std::ofstream;
using std::string;
using std::vector;

#include "../base_exception.h"
#include "../files/file_record.h"
using files::file_record;

namespace cloud {

class storage {
public:	
	//api
	virtual string info() = 0;
	virtual bool upload(string file) = 0;
	virtual vector<string> list_directory(string directory, bool recursive = false) = 0; //TODO: introduce file/directory clases and use vector<file> or directory
	virtual vector<file_record> list_directory_files(string directory, bool recursive = false) = 0;
	virtual bool download(string file) = 0;
	bool sync(string local_syncing_directory_root, string remote_syncing_directory_root, bool recursive = true);

	//utils
	virtual void save(ofstream& fout) = 0;
	virtual string name() const = 0;

	class storage_auth_exception: public base_exception {
	public:
		storage_auth_exception(): base_exception("failed to auth") {};
	};
};

} //namespace cloud