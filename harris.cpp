#include "harris.h"
#include "utils.h"

#include <cv.h>
#include <highgui.h>

//IplImage *integral(IplImage *img) {
//  IplImage *ans = cvCreateImage(cvSize(img->width, img->height),
//                              img->depth, img->nChannels);
//}
float mat[] = {-1.f, 0, 1.f};
int filterSize = 5;

int is_extremun(float *val, int c, int r, int w, int h, int filter) {
    int cs = c - filter >= 0 ? c - filter : 0; 
    int rs = r - filter >= 0 ? r - filter : 0;
    int ce = c + filter <= w ? c + filter : w - filter;
    int re = r + filter <= w ? r + filter : h - filter;

    for (int i = rs; i < re; i++) {
        for (int j = cs; j < ce; j++) {
            if ((i != r || j != c) && val[i * w + j] >= val[r * w + c]) {
                return 0;
            }
        }
    }

    return 1;
}

IplImage *derivateX(IplImage *src) {
    CvMat matrix;
    matrix = cvMat(1, 5, CV_32F, mat);
    
    IplImage *img = get_gray(src);
    IplImage *dst = cvCloneImage(img);
    cvFilter2D(img, dst, &matrix);
//    cvCornerHarris(img, dst, 16);

//    cvMul(dst, dst, dst);
//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", dst);
//  cvWaitKey(0);

    cvReleaseImage(&img);
    
    return dst;
}

IplImage *derivateY(IplImage *src) {
    CvMat matrix;
    matrix = cvMat(5, 1, CV_32F, mat);
    
    IplImage *img = get_gray(src);
//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", img);
//  cvWaitKey(0);
    IplImage *dst = cvCloneImage(img);
    cvFilter2D(img, dst, &matrix);
//    cvCornerHarris(img, dst, 16);

//  cvMul(dst, dst, dst);
//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", dst);
//  cvWaitKey(0);

    cvReleaseImage(&img);
    
    return dst;
}

IplImage *harris(IplImage *src, float threshold) {
    IplImage *deriX = derivateX(src);
    IplImage *deriY = derivateY(src);
    IplImage *deriXY = cvCloneImage(deriX);
    //  
    cvMul(deriX, deriY, deriXY);
    cvMul(deriX, deriX, deriX);
    cvMul(deriY, deriY, deriY);

//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", deriX);
//  cvWaitKey(0);

    cvSmooth(deriX, deriX);
    cvSmooth(deriY, deriY);
    cvSmooth(deriXY, deriXY);

//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", deriX);
//  cvWaitKey(0);

    IplImage *printImg = cvCloneImage(src);

    int w = src->width;
    int h = src->height;
    float *cims = new float[w * h];

    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w - filterSize; x++) {
            float Ix = pixval32f(deriX, x, y);
            float Iy = pixval32f(deriY, x, y);
            float Ixy = pixval32f(deriXY, x, y);
                
            float up = Ix * Iy - Ixy * Ixy;
            float down = Ix + Iy;

            float cim = up / down; 
//          cout << "Ix : " << Ix << endl;
//          cout << "Iy : " << Iy << endl;
//          cout << Ixx << " " << Iyy << " " << Ixy << endl;
//          cout << Ixx * Iyy << endl;
//          cout << up << " " << down << endl;
//          cout << cim << endl;
            if (cim > threshold) {
                cims[y * w + x] = cim;
            } else 
                cims[y * w + x] = 0.0;
        }
    }

    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w- filterSize; x++) {
            if (cims[y * w + x] != 0 && is_extremun(cims, x, y, w, h, filterSize)) {
                drawPoint(printImg, x, y);
            }
        }
    }

    cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
    cvShowImage("1", printImg);
    cvWaitKey(0);

    return printImg;
    
}
