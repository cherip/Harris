#include "harris.h"
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        cout << "need parameter!\n";
        return 1;
    }

    IplImage *img = cvLoadImage(argv[1]);
    float threshold;
    if (argc == 3)
        threshold = atof(argv[2]);
    else
        threshold = 0.03;

//  IplImage *derivX = derivateX(img);
//  IplImage *derivY = derivateY(img);
    IplImage *harr = harris(img, threshold);
}
