.PHONY: clean

LIBCONFIG_DIR=../../libconfig-1.4.8/lib/.libs
INCLUDE_LIBCONFIG=../../libconfig-1.4.8/lib
LIBS=-lm -lconfig
BIN_DIR=./
CC=gcc

${BIN_DIR}GooMFGenerator: src/MonitorGenerator.c src/Globals.h src/FileParsing.h src/FSMParsing.h
	${CC} -g -o $@ -I${INCLUDE_LIBCONFIG} -L${LIBCONFIG_DIR} $<  ${LIBS}

clean:
	rm -rf ${BIN_DIR}GooMFGenerator
