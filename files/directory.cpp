#include "directory.h"

#include "../base_exception.h"

#include <iostream>
using std::cout;

namespace files {

directory::directory(string path): file(path, 0, 0) {}

directory::directory(string path, string name): file(path, name, 0, 0) {}

directory::~directory() {
	for (map<string, file*>::iterator i = contents.begin(); i != contents.end(); ++i)
		delete i->second;
}

size_t max(size_t a, size_t b) { return (a > b ? a : b); }

void directory::add_file(file* f) {
	if (!f)
		throw base_exception("invalid pointer passed");
	if(contents.count(f->get_name()))
		throw base_exception("directory already contains file with such name");	

	contents[f->get_name()] = f;
	size += f->get_size();
	timestamp = max(timestamp, f->get_timestamp());

	cout << path << "\n";
	cout << "added file: " << f->get_name() << "\n\n";
}

void directory::recalculate() {
	size = 0;
	timestamp = 0;
	for (map<string, file*>::iterator i = contents.begin(); i != contents.end(); ++i) {
		directory* dir = static_cast<directory*>(i->second);
		if (dir) dir->recalculate();
		size += i->second->get_size();
		timestamp = max(timestamp, i->second->get_timestamp());
	}
}

void directory::print(int level) {
	for (int i = 0; i < level - 1; ++i)
		cout << "  ";
	cout << name << "/ " << timestamp << "\n";

	for (map<string, file*>::iterator i = contents.begin(); i != contents.end(); ++i) {
		directory* dir = dynamic_cast<directory*>(i->second);
		if (dir) dir->print(level+1);
		else {
			for (int j = 0; j < level; ++j)
				cout << "  ";
			cout << i->second->get_name() << " " << i->second->get_timestamp() << "\n";
		}
	}

	cout << "\n";
}

} //namespace files