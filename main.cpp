#include "harris.h"
#include "utils.h"
#include <cv.h>
#include <highgui.h>
#include <iostream>

using namespace std;

//#define METHOD

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        cout << "need parameter!\n";
        return 1;
    }

#ifdef METHOD
    IplImage *img = cvLoadImage(argv[1]);
    IplImage *img2 = cvLoadImage(argv[2]);
    float threshold;
    if (argc == 4)
        threshold = atof(argv[3]);
    else
        threshold = 0.03;

//  IplImage *derivX = derivateX(img);
//  IplImage *derivY = derivateY(img);
    IplImage *ans1 = harris_laplace(img, threshold);
    IplImage *ans2 = harris_laplace(img2, threshold);

    IplImage *print = stack_imgs(ans1, ans2);
    show_image(print, "two");
#else
    IplImage *img = cvLoadImage(argv[1]);
    IplImage *img2 = cvLoadImage(argv[2]);
    float threshold;
    if (argc == 3)
        threshold = atof(argv[2]);
    else
        threshold = 0.03;

//  IplImage *derivX = derivateX(img);
//  IplImage *derivY = derivateY(img);
//  float **des1, **des2;
//  int npts1, npts2;
//  int ndes;

    cmp_two_image(img, img2);

//  IplImage *ans1 = harris(img, threshold, &des1, &npts1, &ndes);
//  IplImage *ans2 = harris(img2, threshold, &des2, &npts2, &ndes);

//  IplImage *print = stack_imgs(ans1, ans2);
//  show_image(print, "two");

//  for (int i = 0; i < npts1; i++) {
//      for (int j = 0; j < npts2; j++) {
//          float dist = 0.0f;
//          for (int k = 0; k < ndes; k++) {
//              dist += ABS(des1[i][k] - des2[j][k]);
//          }
//          cout << i << "---->" << j << " " << dist << endl;
//      }
//  }
#endif
}
