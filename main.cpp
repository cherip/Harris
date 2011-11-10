#include "harris.h"
#include "utils.h"
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
    IplImage *img2 = cvLoadImage(argv[2]);
    float threshold;
    if (argc == 4)
        threshold = atof(argv[3]);
    else
        threshold = 0.03;

//  IplImage *derivX = derivateX(img);
//  IplImage *derivY = derivateY(img);
//    IplImage *harr = harris(img, threshold);
    IplImage *ans1 = harris_laplace(img, threshold);
    IplImage *ans2 = harris_laplace(img2, threshold);

    IplImage *print = stack_imgs(ans1, ans2);
    show_image(print, "two");
//  IplImage *img = cvLoadImage(argv[1]);
//  float threshold;
//  if (argc == 3)
//      threshold = atof(argv[2]);
//  else
//      threshold = 0.03;

//  IplImage *derivX = derivateX(img);
//  IplImage *derivY = derivateY(img);
//    IplImage *harr = harris(img, threshold);
//  harris_laplace(img, 0.03);
}
