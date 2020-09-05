#include <iostream>
#include <fstream>
#include <filesystem>
#include <unordered_map>

#include "utf8.h"

namespace fs = std::filesystem;

char* read(const char* name) {
	std::ifstream file_stream(name, std::ios_base::binary);

	file_stream.seekg(0, file_stream.end);
	int length = file_stream.tellg();
	file_stream.seekg(0, file_stream.beg);
	char* data = (char*) calloc(length+5, 1);
	file_stream.read(data+4, length);
	memcpy(data, &length, 4);

	file_stream.close();
	return data;
}

string32 readfile(std::string name) {
	char* data = read(name.c_str());
	string32 file = string32 (data+4);
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

string32 parsePage(string32 pageContents) {
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
	int index (0);
	while (index < page.len()) {
		int havematched = 0;
		for (;index<page.len();index++) {
			if (havematched == tofind)
				break;
			if (page.cs[index] == searchString[havematched])
				havematched++;
			else
				havematched = 0;
		}

		if (!havematched)
			break;

		havematched--;
		int startIndex (index-tofind);
		int endIndex (0);

		for (;index<page.len();index++) {
			if (page[index] == searchString[havematched])
				endIndex = index, havematched--;
			else
				havematched = tofind-1;
			if (!havematched) {
				endIndex -= tofind-1;
				break;
			}
		}
		std::vector<string32> split = string32(page, startIndex+tofind, endIndex+1).split(" ");
		page.cs.erase(page.cs.begin()+startIndex, page.cs.begin()+endIndex+tofind+1);

		if (split[0] == "include") {
			string32 toInclude = readfile("source/_includes/" + std::string(split[1].asChar()));
			page.cs.insert(page.cs.begin()+startIndex, toInclude.cs.begin(), toInclude.cs.end());
		}
	}
	return page;
}

string32 parsePageHTML(string32 htmlContents) {
	return parsePage(htmlContents);
}

string32 parsePageMD(string32 mdContents) {
	string32 htmlContents;
	// Process markdown in to HTML

	// Markdown is nicely separated by `\n\n` for each "section" so we can process them individually.
	std::vector<string32> sections = mdContents.split("\n\n");

	for (string32 section : sections) {
		char* asChar = section[0].toChar();
		char firstChar = *asChar;
		delete asChar;

		string32 toAdd = "";

		if (section.substr(0, 3) == "---") {
			htmlContents += section + "\n\n";
			continue;
	  } else if (firstChar == '#') {
	  	toAdd += "<h1>" + section.substr(1) + "</h1>";
	  } else if (firstChar >= 0x30 && firstChar <= 0x39) {
	  	// ToDo: Impement Numeric Lists
	  } else if (section.substr(0, 3) == "```") {
			std::vector<string32> lines = section.split("\n");
			string32 language = lines[0].substr(3);
			lines.erase(lines.begin());
			lines.pop_back();

			// ToDo: Handle syntax highlighting.

			toAdd += "<code>";
			for (auto line : lines)
				toAdd += line + "<br>";
			toAdd += "</code>";
	  } else {
		  toAdd += "<p>" + section + "</p>";
		}

		// Single Code
		toAdd.replaceAroundSelfAsymAll("`", "`", "<code>", "</code>");

		// Underline
		toAdd.replaceAroundSelfAsymAll("__", "__", "<u>", "</u>");

		// Bold / Italic
		toAdd.replaceAroundSelfAll("***", "** *"); // Bold and Italic
		toAdd.replaceAroundSelfAsymAll("**", "**", "<strong>", "</strong>"); // Bold
		toAdd.replaceAroundSelfAsymAll("*", "*", "<em>", "</em>"); // Italic
		toAdd.replaceAroundSelfAsymAll("_", "_", "<em>", "</em>"); // Italic

		htmlContents += toAdd;
	}

	// Run it through normal pagePage
	return parsePage(htmlContents);
}

int main() {
	std::string path = "source";

	fs::remove_all("output");

	for (const auto & file : fs::recursive_directory_iterator(path)) {
		if (file.path().stem().string()[0] == '_' || file.path().string()[7] == '_')
			continue;

		if (file.is_regular_file()) {
			std::string ext = file.path().extension().string();
			std::string writeExt = ext;
			fs::create_directories("output" + file.path().parent_path().string().substr(6));
			std::string path = file.path().string();
			if (writeExt == ".md")
				writeExt = ".html";
			std::ofstream outfile("output" + path.substr(6, path.size() - ext.size() - 6) + writeExt);
			if (ext == ".html") {
				outfile << parsePageHTML(readfile(file.path().c_str()));
			} else if (ext == ".md") {
				outfile << parsePageMD(readfile(file.path().c_str()));
			} else {
				char* data = read(file.path().c_str());
				int length = *(int*)data;
				outfile.write(data+4, length);
				delete data;
			}
			outfile.close();
		}
	}
}
