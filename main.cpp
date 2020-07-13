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
		values[std::string(key.asChar())] = val;
	}
public:
	char* layout;
	bool hasLayout = false;
	string32 parsed;
	std::unordered_map<std::string, string32> values;
  frontMatter(string32 unparsedPage) {
		std::vector<string32> lines = unparsedPage.split("\n");
		if (lines.front() == "---") {
			int line = 1;
			for (;lines[line]!="---";line++) {
				std::vector dataToParse = lines[line].split(": ");
				if (dataToParse[0] == "layout") {
					layout = dataToParse[1].asChar();
					hasLayout = true;
				} else
					set("page." + dataToParse[0], dataToParse[1]);
			}
			line++;
			for (;line<lines.size();line++) {
				parsed += lines[line] + "\n";
			}
		} else {
			parsed = unparsedPage;
		}
		std::vector<string32> config = readfile("source/_config.yml").split("\n");
		for (string32 line : config) {
			std::vector<string32> split = line.split(": ");
			set("site." + split[0], split[1]);
		}
  }
	string32 operator[](string32 s) {
		return values[std::string(s.asChar())];
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
	int i (0);
	string32 searchString = "{% ";
	string32 endString = " %}";
	while (i < page.cs.size()) {
		int havematched (0);
		for (;i<page.cs.size();i++) {
			if (page.cs[i] == searchString[havematched])
				havematched++;
			else
				havematched = 0;
			if (havematched == searchString.cs.size())
				break;
		}
		if (!havematched)
			break;
		int si (i-3);
		int ei (0);
		for (;i<page.cs.size();i++) {
			if (page.cs[i] == endString[havematched]) {
				havematched++;
				ei = i;
			} else
				havematched = 0;
			if (havematched == endString.cs.size()) {
				ei -= endString.cs.size();
				break;
			}
		}
		string32 testString;
		for (int j(si+4);j<=ei;j++)
			testString += page[j];
		page.cs.erase(page.cs.begin()+si, page.cs.begin()+ei+4);
		std::vector<string32> split = testString.split(" ");
		if (split[0] == "include") {
			string32 toInclude = readfile("source/_includes/" + std::string(split[1].asChar()));
			page.cs.insert(page.cs.begin()+si, toInclude.cs.begin(), toInclude.cs.end());
		}
		i = 0;
		si = 0;
		ei = 0;
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
