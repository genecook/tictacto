# using makefile due to non-standard boost install and finicky cmake...

TOOLS_DIR := ${HOME}/Desktop/tools
BOOSTLIB := ${TOOLS_DIR}/lib/libboost_program_options.a
BOOST_INCLUDES := -I${TOOLS_DIR}/boost_1_71_0

CPP = g++

CFLAGS = ${BOOST_INCLUDES} -O
LDFLAGS = ${BOOSTLIB}

tictacto_gen: main.C
	$(CPP) $(CFLAGS) -o $@ $<

test: tictacto_gen
	./tictacto_gen

clean:
	rm -f tictacto_gen

