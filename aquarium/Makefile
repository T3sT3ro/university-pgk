SRC_FILES := $(wildcard *.cpp)
OBJ_FILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
LDFLAGS := -lGLEW -lGL -lglfw  -L/home/tooster/lib/
CPPFLAGS := -std=c++14 -Wall -Wextra -g
INCLUDES := -I.
GCC := g++

aquarium.out: $(OBJ_FILES)
	$(GCC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp %.hpp
	$(GCC) $(CPPFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f *.o *.out
