.SUFFIXES: .cpp .o
.PHONY: clean

BUILD_DIR =../../build/TTRV/
SRC = $(shell find src/ -type f -name '*.cpp' | sort)
OBJ = $(subst src/,$(BUILD_DIR),$(SRC:.cpp=.o))

CXX =g++

EXE_NAME =clean-alias

$(BUILD_DIR)$(EXE_NAME): $(OBJ) | $(BUILD_DIR)
	$(CXX) $(OBJ) -o $@

$(BUILD_DIR)%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)*
	
