.PHONY: all clean

BUILD_DIR := ../../../build
SCRIPTS   := $(shell find ./ -type f -name '*.sh' | sort)
OUTPUT_SCRIPTS := $(subst ./,$(BUILD_DIR)/,$(SCRIPTS))

all: $(OUTPUT_SCRIPTS)

$(BUILD_DIR)/%.sh: ./%.sh
	mkdir -p $(BUILD_DIR)
	cp $< $@

clean:
	rm -f $(OUTPUT_SCRIPTS)
