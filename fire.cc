// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Small example how to use the library.
// For more examples, look at demo-main.cc
//
// This code is public domain
// (but note, that the led-matrix library this depends on is GPL v2)

#include "led-matrix.h"
#include "threaded-canvas-manipulator.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>  // rand

using namespace std;

using std::min;
using std::max;

using namespace rgb_matrix;
using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

#define WIDTH 32
#define HEIGHT 32

class FireManipulator : public ThreadedCanvasManipulator {
private:
    static const double step = 2 * M_PI / (WIDTH - 1);

    int counter;
    int seed;
    float seed1;
    float seed2;
    float seed3;
    float seed4;
    float seed5;
    float seed6;

    float randomfloat(float low, float high) {
        float result = low + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX / (high - low)));
        return result;
    }



public:

    FireManipulator(Canvas *m) : ThreadedCanvasManipulator(m) {
        counter = 1;

        seed = static_cast<int> (time(0));
        srand(seed);

        seed1 = randomfloat(-.04, .030);
        seed2 = randomfloat(-.04, .030);
        seed3 = randomfloat(-.04, .030);
        seed4 = randomfloat(-.04, .030);
        seed5 = randomfloat(0.2, 3);
        seed6 = randomfloat(0.2, 3);
    }

    void Run() {
        //clear matrix
        canvas()->Fill(0, 0, 0);
        while (running()) {
            for (int x = 0; x < WIDTH; x++) {

                int red = 150 + ((sin(counter * seed1 + x * step + M_PI / 4) + 1) / 2.0) * 126
                        + ((sin(counter * seed2 + x * step * seed5) + 1) / 2.0) * 126;

                for (int y = 0; y < HEIGHT; y++) {

                    int green = 0.4 * ((sin(counter * seed3 + y * step + M_PI / 4) + 1) / 2.0) * 126
                            + ((sin(counter * seed4 + y * step * seed6) + 1) / 2.0) * 126;

                    int blue = 7;
                    if (red > 255) {
                        red = 255;
                    }
                    if (red <= 2) {
                        red = 2;
                    }
                    if (green <= 2) {
                        green = 2;
                    }

                    canvas()->SetPixel(x, y, red, green, blue);
                }
            }

            counter++;
            usleep(1 * 1000); // wait a little to slow down things. 
        }
        //clear matrix
        canvas()->Fill(0, 0, 0);
    }
};

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
    bool do_luminance_correct = true;
    // The matrix, our "frame buffer" and display updater.
    RGBMatrix *matrix = new RGBMatrix(&io, rows, chain);
    matrix->set_luminance_correct(do_luminance_correct);
    Canvas *canvas = matrix;
    ThreadedCanvasManipulator *image_gen = new FireManipulator(canvas);
    // Now start the thread.
    image_gen->Start();

    // Things are set up. Just wait for <RETURN> to be pressed.
    printf("Press <RETURN> to exit and reset LEDs\n");
    getchar();

    delete image_gen;
    delete canvas;

    return 0;
}
