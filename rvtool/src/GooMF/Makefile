.PHONY: all clean

BUILD_DIR := ../../build/GooMF

goomf_generator := GooMFGenerator
goomf_library   := GooMFLibrary
goomf_run       := GooMFRun.sh

all:
	mkdir -p $(BUILD_DIR)/
	mkdir -p $(BUILD_DIR)/$(goomf_generator)
	mkdir -p $(BUILD_DIR)/$(goomf_library)
	cp -R $(goomf_generator)/ $(BUILD_DIR)/
	cp -R $(goomf_library)/ $(BUILD_DIR)/
	cp $(goomf_run) $(BUILD_DIR)/
	cd $(BUILD_DIR)/$(goomf_generator); make

clean:
	rm -rf $(BUILD_DIR)*


