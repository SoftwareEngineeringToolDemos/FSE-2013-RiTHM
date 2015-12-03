.PHONY: all clean
.SUFFIXES: .c

SRC=$(shell find -type f -name '*.c' | sort)
OUTPUT_DIR=../../build/monitor
OUTPUT_SRC=$(subst ./,$(OUTPUT_DIR)/,$(SRC))

all: $(OUTPUT_SRC)

$(OUTPUT_DIR)/%.c: %.c | $(OUTPUT_DIR)
	cp $< $@	

$(OUTPUT_DIR):
	mkdir -p $@

clean:
	rm -rf 



