#include <iostream>
#include <fstream>

#include "../C++UTF8Test/utf8.h"

const int MAX_FILE_SIZE = 8096;

int main() {
  std::fstream layout_file;
  layout_file.open("testfiles/_layout.html");
  char* data = new char [MAX_FILE_SIZE];
  int count(0);
  while (layout_file)
		data[count++] = layout_file.get();
	data[--count] = 0;

	string32 layout (data);

	delete data;
	std::cout << layout << "\n";
}
