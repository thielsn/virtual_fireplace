// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-


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

static int usage(const char *progname) {
    fprintf(stderr, "usage: %s [-d]\n", progname);
    fprintf(stderr, "Options:\n"
            "\t-d            : run as daemon. Use this when starting in\n"
            "\t                /etc/init.d, but also when running without\n"
            "\t                terminal (e.g. cron).\n");

    return 1;
}

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

static RGBMatrix *matrix;

static Canvas *canvas;
static ThreadedCanvasManipulator *image_gen;
static GPIO io;

static int setupAndStartFire() {

    /*
     * Set up GPIO pins. This fails when not running as root.
     */

    if (!io.Init()) {

        fprintf(stderr, "io.Init() failed!\n");
        return 1;
    }

    /*
     * Set up the RGBMatrix. It implements a 'Canvas' interface.
     */
    int rows = 32; // A 32x32 display. Use 16 when this is a 16x32 display.
    int chain = 1; // Number of boards chained together.
    bool do_luminance_correct = true;
    // The matrix, our "frame buffer" and display updater.
    matrix = new RGBMatrix(&io, rows, chain);
    matrix->set_luminance_correct(do_luminance_correct);
    canvas = matrix;
    image_gen = new FireManipulator(canvas);
    // Now start the thread.

    image_gen->Start();

    return 0;
}

int main(int argc, char *argv[]) {


    bool as_daemon = false;
    int opt;


    while ((argc > 0) //skip option check in case called without command line arguments
            &&((opt = getopt(argc, argv, "dlD:t:r:p:c:m:L")) != -1)) {
        switch (opt) {

            case 'd':
                as_daemon = true;
                break;

            default: /* '?' */
                return usage(argv[0]);
        }
    }



    if (as_daemon) {
        pid_t pID = fork();

        if (pID != 0) {
            if (pID < 0) {
                fprintf(stderr, "error on becoming daemon: %i\n", pID);
                return 1;
            }
            fprintf(stdout, "%i\n", pID);
            return 0;
        }
        //fprintf(stderr, "daemon mode (child process)\n");
        if (setupAndStartFire() != 0) {
            fprintf(stderr, "error in setupAndStartFire\n");
            return 1;
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        for (;;) {
            pause();
        }

    } else {


        if (setupAndStartFire() != 0) {
            fprintf(stderr, "error in setupAndStartFire\n");
            return 1;
        }


        // Things are set up. Just wait for <RETURN> to be pressed.
        printf("Press <RETURN> to exit and reset LEDs\n");
        getchar();

        delete image_gen;
        delete canvas;

    }
    return 0;
}
