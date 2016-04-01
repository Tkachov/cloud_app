#include "storage_manager.h"
#include "storage.h"
#include "dropbox_storage.h"

#include <iostream>
#include <fstream>
using std::ifstream;
using std::ofstream;
using std::cout;

namespace cloud {

storage_manager::storage_manager() {
	current_storage = -1;
}


storage_manager::~storage_manager() {
	for (int i = 0; i < storages.size(); ++i)
		delete storages[i];
}

void storage_manager::load_settings() {
	ifstream fin("private/storage_manager.txt");
	fin >> current_storage;
	int n; fin >> n; //yep, the most stupid/simple approach - it's prototype, after all
	string s;
	for (int i = 0; i < n; ++i) {
		fin >> s;
		if (s == "" && !fin.eof()) {
			--i;
			continue;
		}
		if (s == "dropbox") storages.push_back(dropbox::dropbox_storage::load(fin));
		else {
			cout << "unknown \"" << s << "\" storage described\n";
		}
	}
	fin.close();
}

bool storage_manager::add_storage(storage* s, bool set_as_current) {
	if (s == 0) return false;
	storages.push_back(s);
	if (set_as_current) current_storage = storages.size() - 1;
	save_settings();
	return true;
}

const vector<storage*>& storage_manager::get_storages() const {
	return storages;
}

storage* storage_manager::get_current_storage() const {
	if (current_storage < 0 || current_storage >= storages.size()) return 0;
	return storages[current_storage];
}

//private:

void storage_manager::save_settings() {
	ofstream fout("private/storage_manager.txt");
	fout << current_storage << "\n";
	int n = storages.size();
	fout << n << "\n\n";	
	for (int i = 0; i < n; ++i) {
		storages[i]->save(fout);
		fout << "\n";
	}
	fout.close();
}

} //namespace cloud