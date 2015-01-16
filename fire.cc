// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>  // rand

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
using namespace std;

#define WIDTH 32
#define HEIGHT 32
#define DURATION 20000


static int counter = 1;
static double step = 2 * M_PI / (WIDTH-1);

static float randomfloat(float low, float high) {

    float result = low + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / (high - low)));
    return result;
}

static void DrawOnCanvas(Canvas *canvas) {
    /*
     * Let's create a simple animation. We use the canvas to draw
     * pixels. We wait between each step to have a slower animation.
     */
    canvas->Fill(0, 0, 0);

    int seed = static_cast<int> (time(0));
    srand(seed);

    float seed1 = randomfloat(-.04, .030);
    float seed2 = randomfloat(-.04, .030);
    float seed3 = randomfloat(-.04, .030);
    float seed4 = randomfloat(-.04, .030);
    float seed5 = randomfloat(0.2, 3);
    float seed6 = randomfloat(0.2, 3);


    cout << "1: " << seed1 << endl;
    cout << "2: " << seed2 << endl;
    cout << "3: " << seed3 << endl;
    cout << "4: " << seed4 << endl;
    
    


    for (int i = 0; i < DURATION; ++i) {

        for (int x = 0; x < WIDTH; x++) {

            int red = 150 +((sin(counter * seed1 + x * step + M_PI / 4) + 1) / 2.0) * 126
                    + ((sin(counter * seed2 + x * step * seed5) + 1) / 2.0) * 126;

            for (int y = 0; y < HEIGHT; y++) {

                int green = 0.4 * ((sin(counter * seed3 + y * step + M_PI / 4) + 1) / 2.0) * 126
                    + ((sin(counter * seed4 + y * step * seed6) + 1) / 2.0) * 126;
                
                int blue = 2;
                if (red>255){
                    red=255;
                }
                if (red<=0){
                    red=250;
                    green=255;
                    blue=2;
                }
                if (green<=0){
                    red=250;
                    green=255;
                    blue=2;
                }
                
                canvas->SetPixel(x, y, red, green, blue);
            }
        }

        counter++;
        usleep(1 * 1000); // wait a little to slow down things. 
    }

}

int main(int argc, char *argv[]) {
    /*
     * Set up GPIO pins. This fails when not running as root.
     */
    GPIO io;
    if (!io.Init())
        return 1;

    /*
     * Set up the RGBMatrix. It implements a 'Canvas' interface.
     */
    int rows = 32; // A 32x32 display. Use 16 when this is a 16x32 display.
    int chain = 1; // Number of boards chained together.
    Canvas *canvas = new RGBMatrix(&io, rows, chain);

    DrawOnCanvas(canvas); // Using the canvas.

    // Animation finished. Shut down the RGB matrix.
    canvas->Clear();
    delete canvas;

    return 0;
}
