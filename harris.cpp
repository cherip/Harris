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

int is_extremum_3D(float *up, float *mid, float *down, int c, int r,
                int w, int h) {
    float val = mid[w * r + c];
    int size = 3;
    for (int rr = -size; rr <= size; rr++) {
        for (int cc = -size; cc <= size; cc++) {
            if (up[w * (rr + r) + c + cc] >= val ||
                down[w * (rr + r) + c + cc] >=val ||
                ((rr != 0 || cc != 0) && mid[w * (rr + r) + cc + c] >= val)) {
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

int _harris(IplImage *src, float threshold, float *_cims) {
    IplImage *deriX = derivateX(src);
    IplImage *deriY = derivateY(src);
    IplImage *deriXY = cvCloneImage(deriX);
    //  
    cvMul(deriX, deriY, deriXY);
    cvMul(deriX, deriX, deriX);
    cvMul(deriY, deriY, deriY);

    cvSmooth(deriX, deriX, CV_GAUSSIAN, 5);
    cvSmooth(deriY, deriY, CV_GAUSSIAN, 5);
    cvSmooth(deriXY, deriXY, CV_GAUSSIAN, 5);

    int w = src->width;
    int h = src->height;
    float *cims = _cims;
    float k = 0.06;
    float *vals = new float[w * h];
    memset(vals, 0, w * h);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float Ix = pixval32f(deriX, x, y);
            float Iy = pixval32f(deriY, x, y);
            float Ixy = pixval32f(deriXY, x, y);
                
            float det = Ix * Iy - Ixy * Ixy;
            float tr = Ix + Iy;

            float cim = det - k * tr * tr;
//          if (cim > threshold) {
//              cims[y * w + x] = cim;
//          } else 
//              cims[y * w + x] = 0.0;
            cims[y * w + x] = cim;
            vals[y * w + x] = cim;
        }
    }

    sort(vals, vals + w * h);
    float thres = vals[w * h * 2 / 3];
    
    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w- filterSize; x++) {
            if (cims[y * w + x] >= thres && is_extremun(cims, x, y, w, h, 10)) {
            }
        }
    }

    delete [] vals;
    cvReleaseImage(&deriX);
    cvReleaseImage(&deriY);
    cvReleaseImage(&deriXY);

    return 0; 
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

    cvSmooth(deriX, deriX, CV_GAUSSIAN, 5);
    cvSmooth(deriY, deriY, CV_GAUSSIAN, 5);
    cvSmooth(deriXY, deriXY, CV_GAUSSIAN, 5);

//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", deriX);
//  cvWaitKey(0);

    IplImage *printImg = cvCloneImage(src);

    int w = src->width;
    int h = src->height;
    float *cims = new float[w * h];
    float *vals = new float[w * h];
    float k = 0.06;
    memset(vals, 0, sizeof(vals));

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float Ix = pixval32f(deriX, x, y);
            float Iy = pixval32f(deriY, x, y);
            float Ixy = pixval32f(deriXY, x, y);
                
            float det = Ix * Iy - Ixy * Ixy;
            float tr = Ix + Iy;

            float cim = det - k * tr * tr;
//            float cim = det / ( tr); 
//          cout << "Ix : " << Ix << endl;
//          cout << "Iy : " << Iy << endl;
//          cout << Ixx << " " << Iyy << " " << Ixy << endl;
//          cout << Ixx * Iyy << endl;
//          cout << up << " " << down << endl;
//          cout << cim << endl;
//          if (cim > threshold) {
//              cims[y * w + x] = cim;
//          } else 
//              cims[y * w + x] = 0.0;
            cims[y * w + x] = cim;
            vals[y * w + x] = cim;
        }
    }

    sort(vals, vals + w * h);
    float thres = vals[w * h  * 4 / 5];
    
    cout << thres << endl;

    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w- filterSize; x++) {
            if (cims[y * w + x] >= thres && is_extremun(cims, x, y, w, h, 10)) {
                drawPoint(printImg, x, y);
            }
        }
    }

    cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
    cvShowImage("1", printImg);
    cvWaitKey(0);

    return printImg;
    
}

IplImage *harris_laplace(IplImage *img, float threshold) {
    const int intervals = 5;
//  float sigma[intervals];
//  sigma[0] = 1.5;
//  for (int i = 1; i < intervals; i++) {
//      sigma[i] = sigma[i - 1] * 1.2;
//  }
    float sigma = 1.5;
    float k = 1.2;

    float *cims[intervals];
    int width = img->width;
    int height = img->height;
    IplImage *gauss_img = cvCloneImage(img);
    for (int i = 0; i < intervals; i++) {
        cims[i] = new float[img->width * img->height]; 
        //show_image(gauss_img, "nonono");
        _harris(gauss_img, threshold, cims[i]);
        harris(gauss_img, threshold);

        cvSmooth(gauss_img, gauss_img, CV_GAUSSIAN, 0, 0, sigma, sigma);
        sigma = k * sigma;
    }

    IplImage *printImg = cvCloneImage(img);

    for (int i = 1; i < 4; i++) {
        for (int r = filterSize; r < height - filterSize; r++) {
            for (int c = filterSize; c < width - filterSize; c++) {
                if (is_extremum_3D(cims[i -1], cims[i], cims[i + 1],
                            c, r, width, height)) {
                    drawPoint(printImg, c, r); 
                }
            }
        }
    }

//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", printImg);
//  cvWaitKey(0);

    //cvReleaseImage(&printImg);
    cvReleaseImage(&gauss_img);

    return printImg;
}
