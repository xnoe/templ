#include <iostream>
#include <fstream>
#include <filesystem>

#include "../C++UTF8Test/utf8.h"

namespace fs = std::filesystem;

const int MAX_FILE_SIZE = 8096;

string32 readfile(const char* name) {
	std::fstream file_stream;
  file_stream.open(name);

  char* data = new char [MAX_FILE_SIZE];
  int count(0);
  while (file_stream)
		data[count++] = file_stream.get();
	file_stream.close();
	data[--count] = 0;
	string32 file = string32 (data);
	delete data;
	return file;
}

int main() {
	std::string path = "source";

	string32 layout = readfile("source/_layout.html");

	for (const auto & file : fs::recursive_directory_iterator(path)) {
		if (file.is_regular_file()) {
			if (file.path().stem().string().at(0) != '_') {
				string32 newFile = layout;
				newFile.replace("{{ content }}", readfile(file.path().string().c_str()));
				std::cout << "File: " << file.path() << "\n-----\n" << newFile << "\n-----\n";
			}
		}
	}

	std::cout << layout << "\n";
}
