templ:
	g++ -std=c++17 -o templ main.cpp

install:
	mv templ $(DESTDIR)/usr/bin/templ
