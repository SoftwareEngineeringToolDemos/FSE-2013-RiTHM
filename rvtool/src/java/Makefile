.SUFFIXES: .java .class
.PHONY: classes clean

SRC = $(shell find src/ -type f -name '*.java' | sort)
CLASSES = $(subst src/,bin/,$(SRC:.java=.class))
JARS = $(shell find lib/ -type f -name '*.jar' -printf '%p:')
JAR = rvtool.jar
BUILD_DIR =../../build/jars/
BIN_DIR=bin/

$(BUILD_DIR)$(JAR): classes | $(BUILD_DIR)
	$(shell cd bin/; jar cf ../$@ ca/*)
	@echo copying all jar libraries to build directory
	cp lib/*.jar $(BUILD_DIR)
	@echo jar file successfully created.

classes: $(CLASSES)
	@echo compiled Java classes successfully.

bin/%.class: src/%.java | $(BIN_DIR)
	nice javac -g -d bin/ -sourcepath src/ -cp bin:$(JARS) $<

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf bin/* $(BUILD_DIR)$(JAR)
