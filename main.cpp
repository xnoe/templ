#include <iostream>
#include <fstream>
#include <filesystem>

#include "utf8.h"

namespace fs = std::filesystem;

const int MAX_FILE_SIZE = 65536;

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
	fs::remove_all("output");

	for (const auto & file : fs::recursive_directory_iterator(path)) {
		if (file.is_regular_file()) {
			if (file.path().stem().string().at(0) != '_') {
				string32 newFile = layout;
				newFile.replace("{{ content }}", readfile(file.path().c_str()));
				fs::create_directories("output" + file.path().parent_path().string().substr(6));
				std::ofstream outfile("output" + file.path().string().substr(6));
				outfile << newFile;
				outfile.close();
			}
		}
	}
}
