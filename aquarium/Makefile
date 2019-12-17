SRC_DIR := .
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
LDFLAGS := -lGLEW -lGL -lglfw  -L/home/tooster/lib/
CPPFLAGS := -std=c++14 -Wall -Wextra -g
INCLUDES := -I.
GCC := g++

aquarium.out: $(OBJ_FILES)
	$(GCC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(GCC) $(CPPFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o *.out
