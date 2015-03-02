all: bin/pngm

dist/lodepng.o: lib/lodepng/lodepng.cpp
        g++ -c lib/lodepng/lodepng.cpp -o dist/lodepng.o

dist/main.o: src/pngm/main.cpp
        g++ -Ilib/lodepng/  -c src/pngm/main.cpp -o dist/main.o

bin/pngm: dist/lodepng.o dist/main.o
        g++ dist/lodepng.o dist/main.o -o bin/pngm

clean: 
        rm -rf dist/*.o bin/pngm

