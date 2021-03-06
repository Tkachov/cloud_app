#pragma once
#include <fstream>
using std::ifstream;

#include "storage.h"
using namespace cloud;

namespace cloud { namespace dropbox {

class dropbox_storage: public storage {
	static const string KEY, SECRET;
	static string load_contents(string filename);

public:
	static string get_auth_link();
	static dropbox_storage* connect_with_code(string code);
	static dropbox_storage* load(ifstream& fin);

private:
	string token, uid;

	dropbox_storage(string& token, string& uid);

public:	
	~dropbox_storage();

	virtual string info();
	virtual bool upload(string local_file, string remote_file);
	virtual vector<string> list_directory(string directory, bool recursive);
	virtual vector<file_record> list_directory_files(string directory, bool recursive);
	virtual bool download(string remote_file, string local_file);
	virtual bool delete_file(string file);
	virtual bool create_directory(string file);

	virtual void save(ofstream& fout);
	virtual string name() const;
};

} } //namespace cloud::dropbox