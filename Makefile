all: z1 z2 arkanoid

CC=g++
CCFLAGS=-std=c++11 -O0

# first tutorials
z1-1.o: z1/tutorial01.cpp
	$(CC) $(CCFLAGS) $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
z1-2.o: z1/tutorial02.cpp common/shader.cpp
	$(CC) $(CCFLAGS) $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
z1: z1-1.o z1-2.o

# game of lines
z2: z2/main.cpp
	$(CC) $(CCFLAGS) $< -o z2/$@.o common/shader.cpp z2/gameManager.cpp -lGLEW -lGL -lglfw -I.

.PHONY: z2

# arkanoid
arkanoid_cc = $(wildcard arkanoid/**/*.cpp)
arkanoid_hc = $(wildcard arkanoid/**/*.hpp)

arkanoid: $(arkanoid_hc) $(arkanoid_cc)
	$(CC) $(CCFLAGS) $< -o arkanoid/$@.o arkanoid/App.cpp -lGLEW -lGL -lglfw -Iarkanoid

.PHONY: clean run arkanoid

# aquarium
aquarium_cc = $(wildcard aquarium/*.cpp)
aquarium_hc = $(wildcard aquarium/*.hpp)

.PHONY: aquarium

aquarium: $(aquarium_hc) $(aquarium_cc)
	$(CC) $(CCFLAGS) $< -o aquarium/$@.o $^ -lGLEW -lGL -lglfw -Iaquarium -g
	chmod +x aquarium/$@.o



# terrain
terrain_cc = $(wildcard terrain/*.cpp)
terrain_hc = $(wildcard terrain/*.hpp)

terrain: $(terrain_hc) $(terrain_cc)
	$(CC) $(CCFLAGS) $< -o terrain/$@.o $^ -lGLEW -lGL -lglfw -Iterrain -g
	chmod +x terrain/$@.o

.PHONY: terrain


run:
	./run
	
clean:
	find . -type f -name '*.o' -delete

