#ifndef _HARRIS_H_
#define _HARRIS_H_

#include <cv.h>
IplImage *derivateX(IplImage *img);
IplImage *derivateY(IplImage *img);
IplImage *harris(IplImage *, float);
IplImage *harris_laplace(IplImage *img, float threshold);

#endif
