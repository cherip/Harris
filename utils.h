#ifndef _UTILS_H_
#define _UTILS_H_

#include <cv.h>
#include <vector>
#include <iostream>

using namespace std;

IplImage *get_resize(const IplImage *img, int, int);
IplImage *get_gray(const IplImage *img);
int fRound(float);
float ABS(float);
IplImage *stack(IplImage *, IplImage*);
void error(char *p);
float box_integral(IplImage *img, int row, int col, int rows, int cols); 
IplImage *integral(IplImage *source);
float pixval32f(IplImage *img, int w, int h);
void drawPoint(IplImage *src, int w, int h);
#endif
