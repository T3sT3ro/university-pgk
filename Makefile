all: 1 2

# first tutorials
z1-1.o: z1/tutorial01.cpp
	g++ $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
z1-2.o: z1/tutorial02.cpp common/shader.cpp
	g++ $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
1: z1-1.o z1-2.o

# game of lines
2: z2/main.cpp
	g++ $< -o z2/$@.o common/shader.cpp z2/gameManager.cpp -lGLEW -lGL -lglfw -I.

arkanoid: arkanoid/%.cpp
    g++ $< -o arkanoid/$@.o -lGLEW -lGL -lglfw

.PHONY: clean run
run:
	./run
	
clean:
	find . -type f -name '*.o' -delete
