CFLAGS=-Wall -Wno-unused-variable
LDLIBS=-LC:/GnuWin32/lib -lfl -lm
LSOURCES=lex.l
YSOURCES=grammar.ypp
CSOURCES=debug.cpp esta_plus.cpp heap.cpp main.cpp stjn.cpp timing.cpp tmsp.cpp token.cpp chaining.cpp flexibility.cpp exceptions.cpp
HSOURCES=$(CSOURCES:.cpp=.h) includes.h
OBJECTS=$(LSOURCES:.l=.o) $(YSOURCES:.ypp=.tab.o) $(CSOURCES:.cpp=.o)

all: solver

solver: CFLAGS+=-g -O0 -DDEBUG
solver: $(OBJECTS) main.o debug.o
	g++ $(CFLAGS) -o $@ $^ $(LDLIBS)

solver.release: CFLAGS+=-O5
solver.release: $(OBJECTS) main.o
	g++ $(CFLAGS) -o $@ $^ $(LDLIBS)

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
