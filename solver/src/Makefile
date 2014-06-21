CFLAGS=-Wall
LDLIBS=-LC:/GnuWin32/lib -lfl -lm
LSOURCES=lex.l
YSOURCES=grammar.ypp
CSOURCES=output.cpp esta_plus.cpp heap.cpp main.cpp stjn.cpp timing.cpp tmsp.cpp chaining.cpp exceptions.cpp constraints.cpp flexibility.cpp solve.cpp
HSOURCES=$(CSOURCES:.cpp=.h) includes.h
OBJECTS=$(LSOURCES:.l=.o) $(YSOURCES:.ypp=.tab.o) $(CSOURCES:.cpp=.o)

# C++ Compiler options
CXXFLAGS = -std=c++11 -O3 -pipe -DNDEBUG -pedantic-errors -Wparentheses -Wreturn-type -Wcast-qual -Wall -Wpointer-arith -Wwrite-strings -Wconversion -Wno-unknown-pragmas -Wno-long-long -DCLP_BUILD

EXTRAFLAG = -std=c++11 -Wl,--rpath -Wl,../../coin-Clp/lib -O3 -pipe -DNDEBUG -pedantic-errors -Wparentheses -Wreturn-type -Wcast-qual -Wall -Wpointer-arith -Wwrite-strings -Wconversion -Wno-unknown-pragmas -Wno-long-long   -DCLP_BUILD -DSAMPLEDIR=\"`PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --variable=datadir coindatasample`\" 

EXTRALIB = `PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --libs clp`

all: solver

solver: CFLAGS+=-g -O0 -DDEBUG
solver: $(OBJECTS) main.o output.o
	g++ $(CFLAGS) $(EXTRAFLAG) -o $@ $^ $(LDLIBS) $(EXTRALIB)
	
solver.release: CFLAGS+=-O5
solver.release: $(OBJECTS) main.o
	g++ $(CFLAGS) $(EXTRAFLAG) -o $@ $^ $(LDLIBS) $(EXTRALIB)

solverstatic: CFLAGS+=-static -g -O0 -DNDEBUG
solverstatic: $(OBJECTS) main.o
	g++ $(CFLAGS) $(EXTRAFLAG) -o $@ $^ $(LDLIBS) $(EXTRALIB)

main.o: main.cpp
	g++ $(CXXFLAGS) -DSAMPLEDIR=\"`PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --variable=datadir coindatasample`\" `PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --cflags clp`  -c -o main.o `test -f 'main.cpp' || echo './'`main.cpp

solve.o: solve.cpp
	g++ $(CXXFLAGS) -DSAMPLEDIR=\"`PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --variable=datadir coindatasample`\" `PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --cflags clp`  -c -o solve.o `test -f 'solve.cpp' || echo './'`solve.cpp

flexibility.o: flexibility.cpp
	g++ $(CXXFLAGS) -DSAMPLEDIR=\"`PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --variable=datadir coindatasample`\" `PKG_CONFIG_PATH=../../coin-Clp/lib64/pkgconfig:../../coin-Clp/lib/pkgconfig:../../coin-Clp/share/pkgconfig: pkg-config --cflags clp`  -c -o flexibility.o `test -f 'flexibility.cpp' || echo './'`flexibility.cpp

%.o: %.cpp
	g++ -c $(CFLAGS) -o $@ $<

lex.cpp: lex.l grammar.tab.cpp grammar.tab.hpp
	flex -o$@ $<

%.tab.cpp %.tab.hpp: %.ypp
	bison --verbose -d $<

ifneq ($(LSOURCES),)
$(LSOURCES:.l=.cpp): $(YSOURCES:.y=.tab.h)
endif

-include $(OBJECTS:.o=.d)

clean:
	rm -f $(OBJECTS) $(OBJECTS:.o=.d) $(YSOURCES:.ypp=.tab.cpp) $(YSOURCES:.ypp=.tab.hpp) $(YSOURCES:.ypp=.output) $(LSOURCES:.l=.cpp) solver solver.release 2>/dev/null

.PHONY: all clean debug release
