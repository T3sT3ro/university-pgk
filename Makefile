all: 1 2

# dirst tutorials
z1-1.o: z1/tutorial01.cpp
	g++ $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
z1-2.o: z1/tutorial02.cpp common/shader.cpp
	g++ $< -o z1/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.
1: z1-1.o z1-2.o

# game of lines
2: z2/main.cpp
	g++ $< -o z2/$@ common/shader.cpp -lGLEW -lGL -lglfw -I.




.PHONY: clean
clean:
	find . -type f -name '*.o' -delete
