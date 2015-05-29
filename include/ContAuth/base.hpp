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
/* Constantes para o recognizer (1, 10, 8, 8, 400.0) 
float miSafe = 22.9548f, sigmaSafe = 18.002f, miAttacked = 47.5475f, sigmaAttacked = 7.26058f;  */
/* Constantes para o recognizer (2, 16, 8, 8, 400.0) 
float miSafe = 50.0103f, sigmaSafe = 37.5769f, miAttacked = 91.292f, sigmaAttacked = 9.14347f;*/
/* Constantes para o recognizer () */
float miSafe = 20.2725f, sigmaSafe = 16.1414f, miAttacked = 43.4733f, sigmaAttacked = 7.3204f;

#include "misc.hpp"
#include "normalization.hpp"
#include "lbp/lbp.hpp"

#if KINECT
	#include <libfreenect2/libfreenect2.hpp>
	#include <libfreenect2/frame_listener_impl.h>
	#include <libfreenect2/threading.h>
#endif