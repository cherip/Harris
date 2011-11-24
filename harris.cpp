#include "harris.h"
#include "utils.h"

#include <cv.h>
#include <highgui.h>


struct t_point {
    int x;
    int y;
    float val;
};

float** describe_feature(const IplImage *src, t_point *pts, int npts, int &featSize);

float mat[] = {-1.f, 0, 1.f};
int filterSize = 5;
float gauss[5][5];
float sigma = 2.5;
#define PRINT

int cmp(const t_point &a, const t_point &b) {
    return a.val - b.val;
}

int _cmp(const void *a, const void *b) {
    t_point *pa = (t_point *)a;
    t_point *pb = (t_point *)b;

    return pa->val - pb->val;
}

int is_extremun(float *val, int c, int r, int w, int h, int filter) {
    int cs = c - filter >= 0 ? c - filter : 0; 
    int rs = r - filter >= 0 ? r - filter : 0;
    int ce = c + filter <= w - 1 ? c + filter : w - 1;
    int re = r + filter <= h - 1 ? r + filter : h - 1;

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
    int size = 1;
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

IplImage *derivateX(const IplImage *src) {
    CvMat matrix;
    matrix = cvMat(1, 5, CV_32F, mat);
    
//    IplImage *img = get_gray(src);
    IplImage *dst = cvCloneImage(src);
    cvFilter2D(src, dst, &matrix);

//    cvReleaseImage(&img);
    
    return dst;
}

IplImage *derivateY(const IplImage *src) {
    CvMat matrix;
    matrix = cvMat(5, 1, CV_32F, mat);
    
    IplImage *dst = cvCloneImage(src);
    cvFilter2D(src, dst, &matrix);

//  cvReleaseImage(&img);
    
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
    int num =  w * h > 500 ? 500 : w * h * 3 / 4;
//  float thres = vals[w * h - 500];
    float thres = 5000;

    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w- filterSize; x++) {
//          if (cims[y * w + x] >= thres && is_extremun(cims, x, y, w, h, 10)) {
//          }
//          if (cims[y * w + x] < thres) {
//              cims[y * w + x] = 0;
//          }
        }
    }

    delete [] vals;
    cvReleaseImage(&deriX);
    cvReleaseImage(&deriY);
    cvReleaseImage(&deriXY);

    return 0; 
}

IplImage *harris(IplImage *img, float threshold, float ***ptsDes, int *npts, int *ndes, t_point **_pts) {
    IplImage *src = get_gray(img);
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

    IplImage *printImg = cvCloneImage(img);

    int w = src->width;
    int h = src->height;
    float *cims = new float[w * h];
    float *vals = new float[w * h];
    float k = 0.06;
    memset(vals, 0, sizeof(vals));
//  t_point *pts =new t_point[w * h + 1]();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float Ix = pixval32f(deriX, x, y);
            float Iy = pixval32f(deriY, x, y);
            float Ixy = pixval32f(deriXY, x, y);
                
            float det = Ix * Iy - Ixy * Ixy;
            float tr = Ix + Iy;

            float cim = det - k * tr * tr;
            cims[y * w + x] = cim;
            vals[y * w + x] = cim;

//          pts[y * w + x].x = x;
//          pts[y * w + x].y = y;
//          pts[y * w + x].val = cim;
        }
    }
    
//  cout << "ok\n";
//  //sort(pts, pts + w * h, cmp);
//  qsort(pts, sizeof(t_point), w * h, _cmp);
//  cout << "ok\n";
//  int num = w * h - 1;
//  int feat = 0;
//  int contentSize = filterSize;
//  while (num-- >= 0) {
//      int x = pts[num].x;
//      int y = pts[num].y;
//      if (x < contentSize || x > w - contentSize ||
//          y < contentSize || y > w - contentSize)
//          continue;
//      if (is_extremun(cims, pts[num].x, pts[num].y, w, h, filterSize * 4)) {
//          drawPoint(printImg, pts[num].x, pts[num].y);
//          feat++;
//          if (feat > 500)
//              break;
//      }
//  }

    sort(vals, vals + w * h);
    //float thres = 7000;
    int num =  w * h > 4000 ? 4000 : w * h * 3 / 4;
    float thres = vals[w * h - num];
    t_point *pts = new t_point[4000];
    int count = 0;

    for (int y = filterSize; y < h - filterSize; y++) {
        for (int x = filterSize; x < w- filterSize; x++) {
            if (cims[y * w + x] >= thres && is_extremun(cims, x, y, w, h, filterSize)) {
//                drawPoint(printImg, x, y);
                if (cims[y * w + x] == vals[w * h - 1]) {
                    drawPoint(printImg, x, y);
                }
                pts[count].x = x;
                pts[count++].y = y;
            }
        }
    }
    
    float **desc;
    int descSize;

    desc = describe_feature(src, pts, count, descSize);

//  cout << "\n\n****************\n";
//  for (int i = 0; i < count; i++) {
//      for (int j = 0; j < descSize; j++) {
//          cout << desc[i][j] << "\t";
//      }
//      cout << endl;
//  }
    
    /*return the result*/
    *ptsDes = desc;
    *npts = count;
    *ndes = descSize;
    *_pts = pts;

    cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
    cvShowImage("1", printImg);
    cvWaitKey(0);

    delete [] vals;
    delete [] cims; 
    cvReleaseImage(&src);
    cvReleaseImage(&deriX);
    cvReleaseImage(&deriY);
    cvReleaseImage(&deriXY);

    return printImg;
}

IplImage *harris_laplace(IplImage *img, float threshold) {
    const int intervals = 17;
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
    IplImage *gauss_img = get_gray(img);
    for (int i = 0; i < intervals; i++) {
        cims[i] = new float[img->width * img->height]; 
        //show_image(gauss_img, "nonono");
        _harris(gauss_img, threshold, cims[i]);
//        harris(gauss_img, threshold);

        cvSmooth(gauss_img, gauss_img, CV_GAUSSIAN, 0, 0, sigma, sigma);
        sigma = k * sigma;
    }

    IplImage *printImg = cvCloneImage(img);

    for (int i = 1; i < intervals - 1; i++) {
//        cout << "*****************\n";
        for (int r = filterSize; r < height - filterSize; r++) {
            for (int c = filterSize; c < width - filterSize; c++) {
//                cout << cims[i][r * width + c] << " ";
                if (cims[i][r * width + c] > 10 && is_extremum_3D(cims[i -1], cims[i], cims[i + 1],
                            c, r, width, height)) {
                    drawPoint(printImg, c, r); 
                  //  cout << cims[i][r * width + c] << endl;
                }
            }
//            cout << endl;
        }
    }

//  cvNamedWindow("1", CV_WINDOW_AUTOSIZE);
//  cvShowImage("1", printImg);
//  cvWaitKey(0);

    //cvReleaseImage(&printImg);
    cvReleaseImage(&gauss_img);

    return printImg;
}

void print_point(int x, int y, float val) {
    cout << x << " " << y << " " << val << endl;
}

int calc_grad_mag_ori(const IplImage *src, int x, int y, float &mag, float &ori) {
    float dx, dy;

    if (x > 0 && x < src->width - 1 &&
        y > 0 && y < src->height - 1) {
        dy = pixval32f(src, x, y - 1) - pixval32f(src, x, y + 1);
        dx = pixval32f(src, x + 1, y) - pixval32f(src, x - 1, y);

        mag = sqrt(dx * dx + dy * dy);
        ori = atan2(dy, dx);
    
//      print_point(x, y - 1, pixval32f(src, x, y - 1));
//      print_point(x, y + 1, pixval32f(src, x, y + 1));
//      cout << dy << " " << dx << " " << ori << endl;
//      cout << cvRound(8 * (ori + CV_PI) / (2 * CV_PI)) << endl;
//      cout << "-------------\n";
//      cout << mag << endl;
        return 1;
    }

    return 0;
}

float* describe_point(const IplImage *src, t_point *pt, int &size) {

    int offsetX[] = {1, 1, -1, -1};
    int offsetY[] = {1, -1, 1, -1};
    int radius = 2;
//  float mag[(radius + 1) * (radius + 1) + 1];
//  float ori[(radius + 1) * (radius + 1) + 1];
    int _x = pt->x;
    int _y = pt->y;
    int x, y;
    float _mag, _ori;
    int bin_num = 8;
    float *_hist = new float[bin_num * 4];
    float *hist = _hist;
    float sum;
    //memset(hist, 0, sizeof(hist));
    for (int i = 0;  i < bin_num * 4; i++) hist[i] = 0;

    for (int o = 0; o < 4; o++) {
        x = offsetX[o] * radius + _x;
        y = offsetY[o] * radius + _y;
        hist = _hist + o * bin_num;        

        if (x < 0 || x >= src->width ||
            y < 0 || y >= src->height)
            continue;

        for (int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                //cout << x + i << " " << y + i << endl;
                if (calc_grad_mag_ori(src, x + i, y + j, _mag, _ori)) {
                    int bin = cvRound(bin_num * (_ori + CV_PI) / (2 * CV_PI)); 
                    bin = bin < bin_num ? bin : 0;
                    int ai = ABS(i);
                    int aj = ABS(j);
                    hist[bin] += gauss[ai][aj] * _mag;

//                 cout << bin << endl; 
//                 cout << gauss[ai][aj] * _mag << endl;
                }
            } 
        }

    //    cout << "----\n";
    } 
    //cout << "****\n";

    sum = 0.0;
    for (int j = 0; j < bin_num * 4; j++) {
        sum += _hist[j] * _hist[j];
    }
    for (int i = 0; i < bin_num * 4; i++) {
        _hist[i] = _hist[i] / sum;
    }

    size = bin_num * 4; 

    return _hist;
}

float** describe_feature(const IplImage *src, t_point *pts, int npts, int &ndes) {
    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            gauss[x][y] = (1.0f / (2.0f * CV_PI * sigma * sigma)) *
                        exp(-(x * x + y * y) / (2 * sigma * sigma));
//            cout << gauss[x][y] << endl;
        }
    }
    float **des = new float*[npts];     
    int hist_size;
    float *hist;

    for (int i = 0; i < npts; i++) {
        hist = describe_point(src, &pts[i], hist_size);

        des[i] = hist;
//      for (int j = 0; j < hist_size; j++) {
//          des[i][j] = hist[j];
//      }
    }

    ndes = hist_size;

    return des;
}

IplImage *cmp_two_image(IplImage *src1, IplImage *src2) {
    float **des1, **des2;
    int npts1, npts2;
    int ndes;
    t_point *pts1, *pts2;

    IplImage *img1 = harris(src1, 0.01, &des1, &npts1, &ndes, &pts1);
    IplImage *img2 = harris(src2, 0.01, &des2, &npts2, &ndes, &pts2);
    IplImage *print = stack_imgs(src1, src2);

    /*print des of img1 and img2*/
#ifdef PRINT
    for (int i = 0; i < npts1; i++) {
        for (int j = 0; j < ndes; j++) {
            cout << des1[i][j] << " ";
        }
        cout << endl;
    }
    cout << "\n";
    for (int i = 0; i < npts2; i++) {
        for (int j = 0; j < ndes; j++) {
            cout << des2[i][j] << " ";
        }
        cout << endl;
    }
    cout << "\n";
#endif

    int mark = 0;
    float mdist = 10000;
    for (int i = 0; i < npts1; i++) {
        mdist = 10000;
        for (int j = 0; j < npts2; j++) {
            float dist = 0;

            for (int k = 0; k < ndes; k++) {
                dist += ABS(des1[i][k] - des2[j][k]);
            }
#ifdef PRINT
            cout << dist << " " << i << " " << j << endl;
#endif
            if (dist < mdist) {
                mdist = dist;
                mark = j;
            }
        }
        if (mdist < 0.02) {
            cvLine(print, 
                    cvPoint(pts1[i].x, pts1[i].y), 
                    cvPoint(pts2[mark].x + src1->width, pts2[mark].y), 
                    cvScalar(0, 255, 0), 1);
        }
    }
    
    show_image(print, "match");
}
