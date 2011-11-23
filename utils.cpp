#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int _min(int x, int y) {
    return x > y ? y : x;
}

int _max(int x, int y) {
    return x > y ? x : y;
}

float ABS(float x) {
    return x > 0 ? x : -x;
}

float pixval32f(const IplImage *img, int w, int h) {
    return ((float *)(img->imageData + img->widthStep * h))[w];
}

float pixval32f(IplImage *img, int w, int h) {
    return ((float *)(img->imageData + img->widthStep * h))[w];
}

inline void set_pixval32f(IplImage *img, int w, int h, float val) {
    ((float *)(img->imageData + img->widthStep * h))[w] = val;
}

int fRound(float x) {
    return (int)floor(x + 0.5f);
}

IplImage *get_resize(const IplImage *img, int width, int height) {
    CvSize dstSize;
    
    dstSize.width = width;
    dstSize.height = height;

    IplImage *dst;
    dst = cvCreateImage(dstSize, img->depth, img->nChannels);

    cvResize(img, dst, CV_INTER_LINEAR);

    return dst;
}

IplImage *get_gray(const IplImage *img) {
    if (!img) {
        return NULL;    
    }

    IplImage *gray8, *gray32;

    gray32 = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);
    if (img->nChannels == 1) {
        gray8 = (IplImage *)cvClone(img);
    } else {
        gray8 = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
        cvCvtColor(img, gray8, CV_BGR2GRAY);
    }

    cvConvertScale(gray8, gray32, 1.0 / 1.0, 0);
    cvReleaseImage(&gray8);

    return gray32;
}

void error(char *p) {
    cout << p << endl;
}

IplImage *stack(IplImage *img1, IplImage *img2) {
    IplImage *img = cvCreateImage(cvSize(img1->width + img2->width,
                _max(img1->height, img2->height)),
                IPL_DEPTH_8U, 3);
    cvZero(img);
    cvSetImageROI(img, cvRect(0, 0, img1->width, img1->height));
    cvAdd(img1, img, img, NULL);
    cvSetImageROI(img, cvRect(img1->width, 0, img2->width, img2->height));
    cvAdd(img2, img, img, NULL);
    cvResetImageROI(img);

    return img;
}

IplImage *integral(IplImage *source)
{
  // convert the image to single channel 32f
  IplImage *img = get_gray(source);
  IplImage *int_img = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);

  // set up variables for data access
  int height = img->height;
  int width = img->width;
  int step = img->widthStep/sizeof(float);
  float *data   = (float *) img->imageData;  
  float *i_data = (float *) int_img->imageData;  

  // first row only
  float rs = 0.0f;
  for(int j=0; j<width; j++) 
  {
    rs += data[j]; 
    i_data[j] = rs;
  }

  // remaining cells are sum above and to the left
  for(int i=1; i<height; ++i) 
  {
    rs = 0.0f;
    for(int j=0; j<width; ++j) 
    {
      rs += data[i*step+j]; 
      i_data[i*step+j] = rs + i_data[(i-1)*step+j];
    }
  }

  // release the gray image
  cvReleaseImage(&img);

  // return the integral image
  return int_img;
}

float box_integral(IplImage *img, int row, int col, int rows, int cols) 
{
  float *data = (float *) img->imageData;
  int step = img->widthStep/sizeof(float);

  // The subtraction by one for row/col is because row/col is inclusive.
  int r1 = std::min(row,          img->height) - 1;
  int c1 = std::min(col,          img->width)  - 1;
  int r2 = std::min(row + rows,   img->height) - 1;
  int c2 = std::min(col + cols,   img->width)  - 1;

  float A(0.0f), B(0.0f), C(0.0f), D(0.0f);
  if (r1 >= 0 && c1 >= 0) A = data[r1 * step + c1];
  if (r1 >= 0 && c2 >= 0) B = data[r1 * step + c2];
  if (r2 >= 0 && c1 >= 0) C = data[r2 * step + c1];
  if (r2 >= 0 && c2 >= 0) D = data[r2 * step + c2];
	
  //std::cout << r1 << " " << c1 << " " << r2 << " " << c2 << endl;
  //std::cout << A << " " << B << " " << C << " " << D << endl;
  return std::max(0.f, A - B - C + D);
}

void drawPoint(IplImage *src, int w, int h) {
//    cvCircle(src, cvPoint(w, h), 10, cvScalar(0, 255, 0), 1);
    cvLine(src, cvPoint(w - 5, h), cvPoint(w + 5, h), cvScalar(0, 0, 255), 1);
    cvLine(src, cvPoint(w, h - 5), cvPoint(w, h + 5), cvScalar(0, 0, 255), 1);
}

void show_image(IplImage *src, const char *name) {
    cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
    cvShowImage(name, src);
    cvWaitKey(0);
}

IplImage* stack_imgs(const IplImage* img1, const IplImage* img2 )
{
  IplImage* stacked = cvCreateImage( cvSize( img1->width + img2->width,
					     MAX(img1->height, img2->height) ),
				     IPL_DEPTH_8U, 3 );

  cvZero( stacked );
  cvSetImageROI( stacked, cvRect( 0, 0, img1->width, img1->height ) );
  cvAdd( img1, stacked, stacked, NULL );
  cvSetImageROI( stacked, cvRect(img1->width, 0, img2->width, img2->height) );
  cvAdd( img2, stacked, stacked, NULL );
  cvResetImageROI( stacked );

  return stacked;
}
