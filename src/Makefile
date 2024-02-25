all: build run
	
build:
	g++ -std=c++11 main.cpp gauss_matrix.cpp -o main
	
run:
	./main input.txt output.txt

clean:
	rm -rf main