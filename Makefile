all: med_filt

med_filt: main.o image_matrix.o
	g++ -std=c++11 main.o image_matrix.o -lpthread -o med_filt

main.o: main.cpp image_matrix.hpp
	g++ -std=c++11 -c -pthread main.cpp

image_matrix.o: image_matrix.cpp image_matrix.hpp
	g++ -std=c++11 -c -pthread image_matrix.cpp

clean:
	rm -rf *.o med_filt
