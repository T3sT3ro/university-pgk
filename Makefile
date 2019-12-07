all: 1 2 arkanoid

CC=g++
CCFLAGS=-std=c++11

# first tutorials
z1-1.o: z1/tutorial01.cpp
	$(CC) $(CCFLAGS) $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
z1-2.o: z1/tutorial02.cpp common/shader.cpp
	$(CC) $(CCFLAGS) $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
1: z1-1.o z1-2.o

# game of lines
2: z2/main.cpp
	$(CC) $(CCFLAGS) $< -o z2/$@.o common/shader.cpp z2/gameManager.cpp -lGLEW -lGL -lglfw -I.

# arkanoid
arkanoid_cc = $(wildcard arkanoid/**/*.cpp)
arkanoid_hc = $(wildcard arkanoid/**/*.hpp)

arkanoid: $(arkanoid_hc) $(arkanoid_cc)
	$(CC) $(CCFLAGS) $< -o arkanoid/$@.o arkanoid/App.cpp -lGLEW -lGL -lglfw -Iarkanoid

.PHONY: clean run arkanoid
run:
	./run
	
clean:
	find . -type f -name '*.o' -delete
