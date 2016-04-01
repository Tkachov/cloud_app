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
	virtual bool upload(string file);
	virtual vector<string> list_directory(string directory, bool recursive);

	virtual void save(ofstream& fout);
	virtual string name() const;
};

} } //namespace cloud::dropbox