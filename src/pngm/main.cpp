#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <getopt.h>
#include <fstream>
#include "lodepng.h"

/*
3 ways to encode a PNG from RGBA pixel data to a file (and 2 in-memory ways).
NOTE: this samples overwrite the file or test.png without warning!
 */

//g++ lodepng.cpp example_encode.cpp -ansi -pedantic -Wall -Wextra -O3

//Example 1
//Encode from raw pixels to disk with a single function call
//The image argument has width * height RGBA pixels or width * height * 4 bytes

void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
    //Encode the image


    unsigned error = lodepng::encode(filename, image, width, height);
    //if there's an error, display it
    if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

double interpolate(double val, double y0, double x0, double y1, double x1) {
    return (val - x0)*(y1 - y0) / (x1 - x0) + y0;
}

void help() {
    std::cout << "pngm \n";
    std::cout << "Creates a PNG from a matrix of doubles given in the standard input. \n";
    std::cout << "The PNG exportation is based on the library LodePNG from Lode Vandevenne.\n";
    std::cout << "\n";
    std::cout << "Usage:\n" ;
    std::cout << "cat file.txt | pngm -o file.png [[-m mx,my] [-s min,max] [-p palette.p] [-h]] \n\n" ; 
    std::cout << "Options" ;
    std::cout << "\t-o:\tName of the PNG to create.\n" ;
    std::cout << "\t-m:\tMagnification of the input file. For example 10,10 will magnify by ten the x and y.\n" ;
    std::cout << "\t-s:\tScale of the colorscale from option min to max. ie: 0,10.\n" ;
    std::cout << "\t-p:\tUsed to specify a palette file. A palette file is a list of 256 RGB values\n" ;

}


//saves image to filename given as argument. Warning, this overwrites the file without warning!

int main(int argc, char *argv[]) {
    unsigned COLORS = 256;
    char* filename = "out.png";
    unsigned magx, magy;
    magx = magy = 1;
    double * ared, *ablue, *agreen;
    ared = new double[COLORS];
    ablue = new double[COLORS];
    agreen = new double[COLORS];
    for (unsigned i = 0; i < COLORS; i++) {
        ared[i] = ablue[i] = agreen [i] = i;
    }



    double minValue = 0;
    double maxValue = 0;
    bool minmaxdefined = false;
    bool ready=true ; 
    int opt;
    while ((opt = getopt(argc, argv, "o:m:s:p:h")) != -1) {
        switch (opt) {
            case 'o':
                filename = optarg;
                ready = true ; 
                break;
            case 'm':
            { // Magnification 
                std::string vals(optarg);
                std::string buffer;
                std::istringstream ss(vals);
                std::getline(ss, buffer, ',');
                magx = atoi(buffer.c_str());
                std::getline(ss, buffer, ',');
                magy = atoi(buffer.c_str());
                break;
            }
            case 's':
            { // Scale
                minmaxdefined = true;
                std::string vals(optarg);
                std::string buffer;
                std::istringstream ss(vals);
                std::getline(ss, buffer, ',');
                minValue = atof(buffer.c_str());
                std::getline(ss, buffer, ',');
                maxValue = atof(buffer.c_str());
                break;
            }
            case 'p':
            {
                std::string line;
                std::ifstream palettefile(optarg);
                if (palettefile.is_open()) {
                    for (unsigned i = 0; i < COLORS; i++) {
                        std::getline(palettefile, line);
                        std::istringstream pv(line);
                        pv >> ared[i] >> agreen[i] >> ablue[i];
                    }
                    palettefile.close();
                }
                break;
            }
            case 'h':
            {
                help();
                exit(0);
                break;
            }
            default:
                break;

        }
    }
    if (!ready) {
        help() ; 
        exit(1) ; 
    }
    // This read the input
    std::vector<double> data;
    int n_x = 0;
    int n_y = 0;
    std::string line;

    while (!std::getline(std::cin, line, '\n').eof()) {
        std::istringstream reader(line);
        while (!reader.eof()) {

            double val;
            reader >> val;
            if (n_y == 0) {

                if ((!minmaxdefined) && (n_x == 0)) {
                    minValue = maxValue = val;
                }
                n_x++;
            }
            if (!minmaxdefined) {
                minValue = std::min(val, minValue);
                maxValue = std::max(val, maxValue);
            }
            data.push_back(val);

        }
        n_y++;
    }
    std::clog << "scaling : " << minValue << "\t" << maxValue << "\n";
    std::clog << "from : " << n_x << "x" << n_y << "\n";
    std::clog << "to : " << n_x * magx << "x" << n_y * magy << "\n";


    //generate some image
    //  unsigned width = 512, height = 512;
    std::vector<unsigned char> image;
    image.resize((n_x * magx) * (n_y * magy) * 4);
    for (unsigned y = 0; y < n_y; y++) {
        for (unsigned x = 0; x < n_x; x++) {

            double grayValue = (data[n_x * y + x] - minValue) / (maxValue - minValue);

            for (unsigned my = 0; my < magy; my++) {
                for (unsigned mx = 0; mx < magx; mx++) {

                    //int my=0 ; int mx = 0 ; 
                    image[4 * (n_x * magx) * (magy * y + my) + 4 * (magx * x + mx) + 0] = ared[(int) ((COLORS - 1)*(grayValue))];
                    image[4 * (n_x * magx) * (magy * y + my) + 4 * (magx * x + mx) + 1] = agreen[(int) ((COLORS - 1)*(grayValue))];
                    image[4 * (n_x * magx) * (magy * y + my) + 4 * (magx * x + mx) + 2] = ablue[(int) ((COLORS - 1)*(grayValue))];
                    image[4 * (n_x * magx) * (magy * y + my) + 4 * (magx * x + mx) + 3] = 255;
                }

            }

        }
    }




    encodeOneStep(filename, image, (n_x * magx), (n_y * magy));
}
