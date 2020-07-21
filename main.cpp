#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

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

class frontMatter {
	void set(string32 key, string32 val) {
		char* asChar = key.asChar();
		values[std::string(asChar)] = val;
		delete asChar;
	}
public:
	char* layout;
	bool hasLayout = false;
	string32 parsed;
	std::unordered_map<std::string, string32> values;

  frontMatter(string32 unparsedPage) {
		std::vector<string32> lines = unparsedPage.split("\n");
		if (lines.front() != "---")
			parsed = unparsedPage;
		else {
			int line (0);
			while (line++, lines[line]!="---") {
				std::vector<string32> dataToParse = lines[line].split(": ");
				if (dataToParse[0] == "layout")
					hasLayout = layout = dataToParse[1].asChar(), true;
				else
					set("page." + dataToParse[0], dataToParse[1]);
			}
			line++;
			for (;line<lines.size();line++)
				parsed += lines[line] + "\n";
		}

		std::vector<string32> config = readfile("source/_config.yml").split("\n");
		for (string32 line : config) {
			std::vector<string32> split = line.split(": ");
			set("site." + split[0], split[1]);
		}
  }
};

string32 parsePageHTML(string32 pageContents) {
	frontMatter fm = frontMatter(pageContents);
	string32 page;
	if (fm.hasLayout)
		page = readfile("source/_layouts/" + std::string(fm.layout) + ".html").replace("{{ content }}", fm.parsed);
	else
		page = fm.parsed;

	for (auto pair : fm.values)
		page.replaceSelfAll("{{ " + pair.first + " }}", pair.second);

	string32 searchString = "{% ";
	int tofind (searchString.cs.size());
	int i (0);
	while (i < page.len()) {
		int havematched = 0;
		for (;i<page.len();i++) {
			if (page.cs[i] == searchString[havematched])
				havematched++;
			else
				havematched = 0;
			if (havematched == tofind)
				break;
		}
		if (!havematched)
			break;
		havematched--;
		int si (i-3);
		int ei (0);
		for (;i<page.len();i++) {
			if (page[i] == searchString[havematched])
				ei = i, havematched--;
			else
				havematched = tofind-1;
			if (!havematched) {
				ei -= tofind-1;
				break;
			}
		}
		std::vector<string32> split = string32(page, si+4, ei+1).split(" ");
		page.cs.erase(page.cs.begin()+si, page.cs.begin()+ei+4);
		if (split[0] == "include") {
			string32 toInclude = readfile("source/_includes/" + std::string(split[1].asChar()));
			page.cs.insert(page.cs.begin()+si, toInclude.cs.begin(), toInclude.cs.end());
		}
	}
	return page;
}

int main() {
	std::string path = "source";

	fs::remove_all("output");

	for (const auto & file : fs::recursive_directory_iterator(path)) {
		if (file.path().stem().string()[0] == '_' || file.path().string()[7] == '_')
			continue;
		if (file.is_regular_file()) {
			std::string ext = file.path().extension().string();
			fs::create_directories("output" + file.path().parent_path().string().substr(6));
			std::ofstream outfile("output" + file.path().string().substr(6));
			if (ext == ".html") {
				outfile << parsePageHTML(readfile(file.path().c_str()));
			} else {
				outfile << read(file.path().c_str());
			}
			outfile.close();
		}
	}
}
