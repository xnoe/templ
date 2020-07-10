#include <iostream>
#include <fstream>
#include <filesystem>

#include "utf8.h"

namespace fs = std::filesystem;

char* read(const char* name) {
	std::ifstream file_stream(name, std::ios_base::binary);

	int count = 0;
	file_stream.seekg(0, file_stream.end);
	int length = file_stream.tellg();
	file_stream.seekg(0, file_stream.beg);
	char* data = (char*) calloc(length, 1);
	file_stream.read(data, length);

	file_stream.close();
	return data;
}

string32 readfile(const char* name) {
	char* data = read(name);
	string32 file = string32 (data);
	free (data);
	return file;
}

int main() {
	std::string path = "source";

	string32 layout = readfile("source/_layout.html");
	fs::remove_all("output");

	for (const auto & file : fs::recursive_directory_iterator(path)) {
		if (file.is_regular_file()) {
			if (file.path().stem().string().at(0) == '_')
				continue;
			std::string ext = file.path().extension().string();
			fs::create_directories("output" + file.path().parent_path().string().substr(6));
			std::ofstream outfile("output" + file.path().string().substr(6));
			if (ext == ".html")
				outfile << layout.replace("{{ content }}", readfile(file.path().c_str()));
			else
				outfile << read(file.path().c_str());
			outfile.close();
		}
	}
}
