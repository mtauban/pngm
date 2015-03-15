all: bin/pngm

dist/lodepng.o: dist/ lib/lodepng/lodepng.cpp
	g++ -c lib/lodepng/lodepng.cpp -o dist/lodepng.o

dist/main.o: dist/ src/pngm/main.cpp
	g++ -Ilib/lodepng/  -c src/pngm/main.cpp -o dist/main.o

bin/pngm: bin/ dist/lodepng.o dist/main.o
	g++ dist/lodepng.o dist/main.o -o bin/pngm

bin/: 
	mkdir bin

dist/:
	mkdir dist

clean: 
	rm -rf dist/*.o bin/pngm



