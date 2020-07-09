templ:
	g++ -std=c++17 -o templ main.cpp

install:
	mkdir -p $(DESTDIR)/usr/bin
	mv templ $(DESTDIR)/usr/bin/templ
