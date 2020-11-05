default: segments

%: %.cpp
	g++ -I. $< -o $@ AGL3Window.cpp intersection.hpp  -lepoxy -lGL -lglfw
clean:
	rm a.out *.o *~ segments
