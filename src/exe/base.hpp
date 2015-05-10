#include <FaceTracker/Tracker.h>
#include <opencv/highgui.h>
#include <iostream>
#include <ctime>
#include <cmath>
#include <sys/time.h>

/* Incluido para reconhecimento*/
#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv/cv.h>

/* Flags */
#define KINECT 0
#define WRITEVIDEO 1
#define M_WIDTH 960
#define M_HEIGHT 540
#define R_WIDTH 200
#define R_HEIGHT 200
#define R_X 57
#define R_Y 150
#define ROI_X 960
#define ROI_Y 540
#define USER_ID 999
#define SQRT_2 1.41421356237

typedef enum {LOGIN, CONT_AUTH} STAGES;

cv::Mat firstFace, meanFace, meanGray, normImg(1, 1, 16), normFace, stageImg;
cv::Rect normRect(R_X, R_Y, R_WIDTH, R_HEIGHT);
std::vector<std::vector<cv::Point2f> > normVec;
std::vector<cv::Point2f> pts;
STAGES stage = LOGIN;
float dt, w, history[500];
float pSafe, pAttacked, lastPSafe = 0.0f, lastPAttacked = 1.0f, k = -logf(2.0f)/15.0;
float miSafe = 82.5, sigmaSafe = 13.2, miAttacked = 122.4, sigmaAttacked = 16.2;


#include "misc.hpp"
#include "normalization.hpp"
