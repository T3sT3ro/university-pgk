TASK = aquarium

CXX := g++
CXXFLAGS := -std=c++14 -Wall -Iinclude

CPPS := $(wildcard $(TASK)/*.cpp)
OBJS := $(addprefix obj/, $(notdir $(CPPS:.cpp=.o)))

# LDFLAGS := -L.
LDLIBS = -lGLEW -lGL -lglfw


.PHONY: all clean

all: obj app$(TASK)

obj:
	mkdir $@

app$(TASK): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)
	
obj/%.o: $(TASK)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm app$(TASK) $(OBJS)
	# find . -maxdepth 1 -executable -type f -delete