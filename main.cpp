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
	char* data = (char*) calloc(length+1, 1);
	file_stream.read(data, length);

	file_stream.close();
	return data;
}

string32 readfile(std::string name) {
	char* data = read(name.c_str());
	string32 file = string32 (data);
	free (data);
	return file;
}

struct frontMatter {
	char* layout;
	string32 parsed;
  frontMatter(string32 unparsedPage) {
		std::vector<string32> lines = unparsedPage.split("\n");
		if (lines.front() == "---") {
			int line = 1;
			for (;lines[line]!="---";line++) {
				std::vector dataToParse = lines[line].split(": ");
				if (dataToParse[0] == "layout")
					layout = dataToParse[1].asChar();
			}
			line++;
			for (;line<lines.size();line++) {
				parsed += lines[line] + "\n";
			}
		}
  }
};

int main() {
	std::string path = "source";

	fs::remove_all("output");

	for (const auto & file : fs::recursive_directory_iterator(path)) {
		if (file.is_regular_file()) {
			if (file.path().stem().string().at(0) == '_')
				continue;
			std::string ext = file.path().extension().string();
			fs::create_directories("output" + file.path().parent_path().string().substr(6));
			std::ofstream outfile("output" + file.path().string().substr(6));
			if (ext == ".html") {
				frontMatter fm = frontMatter(readfile(file.path().c_str()));
				std::cout << "Layout: `" << fm.layout << "`\nParsed: " << fm.parsed << "\n";
				outfile << readfile("source/_layouts/" + std::string(fm.layout) + ".html").replace("{{ content }}", fm.parsed);
			} else {
				outfile << read(file.path().c_str());
			}
			outfile.close();
		}
	}
}
