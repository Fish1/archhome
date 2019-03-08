install:
	g++ index.cpp

debug:
	g++ -DDUMP_RAM -DVERBOSE index.cpp
