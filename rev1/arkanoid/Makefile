
CXX := g++
CXXFLAGS := -std=c++14 -Wall -Wextra -g
LDFLAGS := -lGLEW -lGL -lglfw  -L/home/tooster/lib/

SRCDIR := .
OBJDIR := obj

TARGET 		:= $(shell basename $(CURDIR)).out
SRC    		:= $(wildcard *.cpp)
INCLUDES 	:= -I.
OBJ 		:= $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRC))

$(TARGET): objdir $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

.PHONY: objdir
objdir:
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(SRCDIR)/%.hpp)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f *.o *.out
	rm -rf obj
