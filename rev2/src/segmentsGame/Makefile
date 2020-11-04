default: segments

%: %.cpp
	g++ -I. $< -o $@ segments.cpp  -lepoxy -lGL -lglfw
clean:
	rm a.out *.o *~ segments
