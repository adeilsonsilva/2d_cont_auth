#include <FaceTracker/Tracker.h>
#include <opencv/highgui.h>
#include <iostream>
#include <ctime>

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
#define ROI_X 550
#define ROI_Y 250

cv::Mat firstFace, meanFace, meanGray, normImg(1, 1, 16), normFace;
cv::Rect normRect(R_X, R_Y, R_WIDTH, R_HEIGHT);
std::vector<std::vector<cv::Point2f> > normVec;
std::vector<cv::Point2f> pts;

#include "misc.hpp"
#include "normalization.hpp"
