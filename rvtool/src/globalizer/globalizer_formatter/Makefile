.PHONY: clean

###################PATH VARIABLES############################################
LIBCONFIG_HEADER_PATH=../../../build/libconfig-1.4.8/lib
LIBCONFIG_LINK_PATH=../../../build/libconfig-1.4.8/lib/.libs
C_FILE=globalizer_spec_formatter.c
OUTPUT_DIR=../../../build/globalizer/
BIN_NAME=globalizer_spec_formatter
CC=gcc
#############################################################################

${OUTPUT_DIR}${BIN_NAME}: ${C_FILE}
	mkdir -p ${OUTPUT_DIR}
	${CC} -o $@ -I${LIBCONFIG_HEADER_PATH} -L${LIBCONFIG_LINK_PATH} $< -lconfig

clean:
	rm -rf ${OUTPUT_DIR}
