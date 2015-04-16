///////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010, Jason Mora Saragih, all rights reserved.
//
// This file is part of FaceTracker.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//     * The software is provided under the terms of this licence stricly for
//       academic, non-commercial, not-for-profit purposes.
//     * Redistributions of source code must retain the above copyright notice, 
//       this list of conditions (licence) and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright 
//       notice, this list of conditions (licence) and the following disclaimer 
//       in the documentation and/or other materials provided with the 
//       distribution.
//     * The name of the author may not be used to endorse or promote products 
//       derived from this software without specific prior written permission.
//     * As this software depends on other libraries, the user must adhere to 
//       and keep in place any licencing terms of those libraries.
//     * Any publications arising from the use of this software, including but
//       not limited to academic journal and conference publications, technical
//       reports and manuals, must cite the following work:
//
//       J. M. Saragih, S. Lucey, and J. F. Cohn. Face Alignment through 
//       Subspace Constrained Mean-Shifts. International Conference of Computer 
//       Vision (ICCV), September, 2009.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED 
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
// EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////
#include <FaceTracker/Tracker.h>
#include <opencv/highgui.h>
#include <iostream>
#include <ctime>

/* Incluido para reconhecimento*/
#include "opencv2/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/highgui/highgui_c.h"
#include <opencv/cv.h>

#define KINECT 0
#define WRITEVIDEO 1

#if KINECT

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/threading.h>

bool protonect_shutdown = false;

void sigint_handler(int s)
{
  protonect_shutdown = true;
}

#endif

//=============================================================================
cv::Mat firstFace, meanFace, meanGray, normImg(1, 1, 16), normFace;
cv::Rect normRect(57, 150, 200, 200);
std::vector<std::vector<cv::Point2f> > normVec;
std::vector<cv::Point2f> pts;

static void read_csv(const std::string& filename, std::vector<cv::Mat>& images, std::vector<int>& labels, std::vector<std::string>& names, char separator = ';') {
    std::ifstream file(filename.c_str(), std::ifstream::in);
    cv::Mat dbImg, dbImageGray;
    if (!file) {
        std::string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    std::string line, path, name, classlabel;
    while (getline(file, line)) {
        std::stringstream liness(line);
        std::getline(liness, path, separator);
	    std::getline(liness, name, separator);
        std::getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            /* std::cout << path << std::endl;
             * As imagens do banco estão coloridas, é necessário converte-las! 
             * dbImg = cv::imread(path, 1);
             * cv::cvtColor(dbImg, dbImageGray,CV_BGR2GRAY); 
             */
            dbImageGray = cv::imread(path, 0);
            images.push_back(dbImageGray);
	        names.push_back(name);
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

std::string getTime(){
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[100];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,100,"%d-%m-%Y %I:%M:%S.avi",timeinfo);
  std::string str(buffer);

  return str;
}

void warpTextureFromTriangle(cv::Point2f srcTri[3], cv::Mat originalImage, cv::Point2f dstTri[3], cv::Mat warp_final){
    int x1 = 50, y1 = 100;
    cv::Mat warp_mat( 2, 3, CV_32FC1 );
    cv::Mat warp_dst, warp_mask;
    cv::Point trianglePoints[3];
	trianglePoints[0] = dstTri[0];
    trianglePoints[1] = dstTri[1];
    trianglePoints[2] = dstTri[2];
    warp_dst  = cv::Mat::zeros( originalImage.rows, originalImage.cols, originalImage.type() );
    warp_mask = cv::Mat::zeros( originalImage.rows, originalImage.cols, originalImage.type() );

    //std::cout << "Rows: " << originalImage.rows << " Cols: " << originalImage.cols << std::endl;

    // Get the Affine Transform
    for(int i=0;i<3;i++){
        srcTri[i].x -= x1;
        srcTri[i].y -= y1;
        dstTri[i].x -= x1;
        dstTri[i].y -= y1;

    }

    warp_mat = cv::getAffineTransform( srcTri, dstTri );

    /// Apply the Affine Transform just found to the src image
    cv::Rect roi(x1, y1, 550, 250);
    cv::Mat originalImageRoi= originalImage(roi);
    cv::Mat warp_dstRoi     = warp_dst(roi);
    cv::warpAffine( originalImageRoi, warp_dstRoi, warp_mat, warp_dstRoi.size() );
	cv::fillConvexPoly(warp_mask, trianglePoints, 3, CV_RGB(255, 255, 255), CV_AA, 0);
	warp_dst.copyTo(normImg, warp_mask);
	/* Caso a Face Normalizada não esteja aparecendo,  descomente a linha abaixo e veja qual o ROI utilizado */
	//cv::imshow("ROI", originalImageRoi);
 }

void meanDraw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
  int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

  //draw triangulation
  c = CV_RGB(0,0,0);
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
       visi.at<int>(tri.at<int>(i,1),0) == 0 ||
       visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    //cv::line(image,p1,p2,c);
	pts.push_back(p1);
	pts.push_back(p2);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    //cv::line(image,p1,p2,c);
	pts.push_back(p2);	
	normVec.push_back(pts);
    //std::cout << " Tri: " << pts.size() << std::endl;
	pts.clear();
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    //cv::line(image,p1,p2,c);
  } 
  return;
}
//=============================================================================
void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
  int i,n = shape.rows/2; cv::Point2f p1,p2; cv::Scalar c;
  cv::Point2f orig[3], normPts[3];
  cv::Mat origIm = image.clone();

  //draw triangulation
  c = CV_RGB(0,0,0);
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
       visi.at<int>(tri.at<int>(i,1),0) == 0 ||
       visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    //cv::line(image,p1,p2,c);
    orig[0] = p1;
    orig[1] = p2;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    //cv::line(image,p1,p2,c);
    orig[2] = p2;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    //cv::line(image,p1,p2,c);
    normPts[0] = normVec[i][0];
    normPts[1] = normVec[i][1];
    normPts[2] = normVec[i][2];
    warpTextureFromTriangle(orig, origIm, normPts, normImg);
  }
#if 0
  //draw connections
  c = CV_RGB(0,0,255);
  for(i = 0; i < con.cols; i++){
    if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
       visi.at<int>(con.at<int>(1,i),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
		   shape.at<double>(con.at<int>(0,i)+n,0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
		   shape.at<double>(con.at<int>(1,i)+n,0));
    //cv::line(image,p1,p2,c,1);
  }
  //draw points
  for(i = 0; i < n; i++){    
    if(visi.at<int>(i,0) == 0)continue;
    p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
    //c = CV_RGB(255,0,0); cv::circle(image,p1,2,c);
  }
#endif
  return;
}
//=============================================================================
int parse_cmd(int argc, const char** argv,
	      char* ftFile,char* conFile,char* triFile,
	      bool &fcheck,double &scale,int &fpd)
{
  int i; fcheck = false; scale = 1; fpd = -1;
  for(i = 1; i < argc; i++){
    if((std::strcmp(argv[i],"-?") == 0) ||
       (std::strcmp(argv[i],"--help") == 0)){
      std::cout << "track_face:- Written by Jason Saragih 2010" << std::endl
	   << "Performs automatic face tracking" << std::endl << std::endl
	   << "#" << std::endl 
	   << "# usage: ./face_tracker [options]" << std::endl
	   << "#" << std::endl << std::endl
	   << "Arguments:" << std::endl
	   << "-m <string> -> Tracker model (default: ../model/face2.tracker)"
	   << std::endl
	   << "-c <string> -> Connectivity (default: ../model/face.con)"
	   << std::endl
	   << "-t <string> -> Triangulation (default: ../model/face.tri)"
	   << std::endl
	   << "-s <double> -> Image scaling (default: 1)" << std::endl
	   << "-d <int>    -> Frames/detections (default: -1)" << std::endl
	   << "--check     -> Check for failure" << std::endl;
      return -1;
    }
  }
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"--check") == 0){fcheck = true; break;}
  }
  if(i >= argc)fcheck = false;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-s") == 0){
      if(argc > i+1)scale = std::atof(argv[i+1]); else scale = 1;
      break;
    }
  }
  if(i >= argc)scale = 1;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-d") == 0){
      if(argc > i+1)fpd = std::atoi(argv[i+1]); else fpd = -1;
      break;
    }
  }
  if(i >= argc)fpd = -1;
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-m") == 0){
      if(argc > i+1)std::strcpy(ftFile,argv[i+1]);
      else strcpy(ftFile,"../model/face2.tracker");
      break;
    }
  }
  if(i >= argc)std::strcpy(ftFile,"../model/face2.tracker");
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-c") == 0){
      if(argc > i+1)std::strcpy(conFile,argv[i+1]);
      else strcpy(conFile,"../model/face.con");
      break;
    }
  }
  if(i >= argc)std::strcpy(conFile,"../model/face.con");
  for(i = 1; i < argc; i++){
    if(std::strcmp(argv[i],"-t") == 0){
      if(argc > i+1)std::strcpy(triFile,argv[i+1]);
      else strcpy(triFile,"../model/face.tri");
      break;
    }
  }
  if(i >= argc)std::strcpy(triFile,"../model/face.tri");
  return 0;
}
//=============================================================================
int main(int argc, const char** argv)
{

  #if KINECT
      libfreenect2::Freenect2 freenect2;
      libfreenect2::Freenect2Device *dev = freenect2.openDefaultDevice();

      if(dev == 0)
      {
        std::cout << "no device connected or failure opening the default one!" << std::endl;
        return -1;
      }

      signal(SIGINT, sigint_handler);
      protonect_shutdown = false;

      //libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
      libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color);
      libfreenect2::FrameMap frames;

      dev->setColorFrameListener(&listener);
      //dev->setIrAndDepthFrameListener(&listener);
      dev->start();
  #endif
  
  //parse command line arguments
  char ftFile[256],conFile[256],triFile[256];
  bool fcheck = false; double scale = 1; int fpd = -1; bool show = true;
  if(parse_cmd(argc,argv,ftFile,conFile,triFile,fcheck,scale,fpd)<0)return 0;

  //set other tracking parameters
  std::vector<int> wSize1(1); wSize1[0] = 7;
  std::vector<int> wSize2(3); wSize2[0] = 11; wSize2[1] = 9; wSize2[2] = 7;
  int nIter = 5; double clamp=3,fTol=0.01; 
  FACETRACKER::Tracker model(ftFile);
  cv::Mat tri=FACETRACKER::IO::LoadTri(triFile);
  cv::Mat con=FACETRACKER::IO::LoadCon(conFile);

  /* Parâmetros para salvar a imagem */
  std::vector<int> comPressParam;
  int imgCount = 0;
  std::string imgOut = "../out/Img";

  /* Caminho do CSV: */
  std::string csvFilePath = "../dataset/csv.ext";
  std::string fn_csv = std::string(csvFilePath);

  // Os seguintes vetores guardarão as imagens, os nomes dos personagens e as labels
  std::vector<cv::Mat> images;
  std::vector<std::string> names;
  std::vector<int> labels;

  // Leitura dos dados
  try {
	  read_csv(fn_csv, images, labels, names);
  }catch (cv::Exception& e) {
    std::cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << std::endl;
	// nothing more we can do
	exit(1);
  }

  #if WRITEVIDEO
    std::string videoName = getTime();
    std::cout << videoName << std::endl;
    cv::VideoWriter videoFile(videoName, CV_FOURCC('M','J','P','G'), 5, cv::Size(960,540), true);
  #endif
  
  //initialize camera and display window
  cv::Mat frame,gray,im; double fps=0; char sss[256]; std::string text; 
  #if KINECT == 0
        cv::VideoCapture video(0);
  #endif

  meanFace = cv::imread("MeanFace.jpg");
  int64 t1,t0 = cvGetTickCount(); int fnum=0;
  cvNamedWindow("Face Tracker",1);
  std::cout << "Hot keys: "        << std::endl
	    << "\t ESC - quit"     << std::endl
	    << "\t d   - Redetect" << std::endl;

  cv::cvtColor(meanFace, meanGray,CV_BGR2GRAY);
  std::vector<int> wSize0; wSize0 = wSize2; 
  if(model.Track(meanGray,wSize0,fpd,nIter,clamp,fTol,fcheck) == 0){
    int idx = model._clm.GetViewIdx();
    meanDraw(meanFace,model._shape,con,tri,model._clm._visi[idx]); 
  }else{
    if(show){cv::Mat R(meanFace,cvRect(0,0,150,50)); R = cv::Scalar(0,0,255);}
    model.FrameReset();
  }
  //cv::imshow("MeanFace", meanFace); 


  /* Pega a altura da primeira imagem. Nós precisaremos disso
  * depois para redimensionar as imagens para o seu tamanho original
  * E precisaremos redimensionar as imagens de entrada para esse tamanho. 
  int im_width = images[0].cols;
  int im_height = images[0].rows;

  Mat grayFaceResized;
  cv::resize(grayFace, grayFaceResized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);

  * USAR ESSA PARTE CASO AS IMAGENS DO BANCO NÃO TENHAM O MESMO TAMANHO DA IMAGEM NORMALIZADA
  */

  // The following lines simply get the last images from
  // your dataset and remove it from the vector. This is
  // done, so that the training data (which we learn the
  // cv::FaceRecognizer on) and the test data we test
  // the model with, do not overlap.
  cv::Mat testSample = images[images.size() - 1];
  int testLabel = labels[labels.size() - 1];
  images.pop_back();
  labels.pop_back();

  /* Cria um FaceRecognizer e treina em cima dele: */
  cv::Ptr<cv::face::FaceRecognizer> faceRec = cv::face::createFisherFaceRecognizer();
  try{
    std::cout << "Treinando.." << std::endl;
    faceRec->train(images, labels);
    std::cout << "Fim do treino!" << std::endl;
  }catch(cv::Exception& e){
	std::cerr << "Erro no Treino! Motivo: " << e.msg << std::endl;
	/* Nothing more we can do */
	exit(1);
  }

  //loop until quit (i.e user presses ESC)
  bool failed = true;
  while(1){ 
    #if KINECT
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        cv::Mat video(rgb->height, rgb->width, CV_8UC3, rgb->data);
    #endif

    video >> frame;
    
    //grab image, resize and flip
    cv::resize(frame,im, cv::Size(960,540));
    cv::flip(im,im,1); 
    cv::cvtColor(im,gray,CV_BGR2GRAY); 
        
    #if WRITEVIDEO
        videoFile.write(im);
    #endif

    //track this image
	//model.FrameReset(); failed = true;
    std::vector<int> wSize; if(failed)wSize = wSize2; else wSize = wSize1; 
    if(model.Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0){
      int idx = model._clm.GetViewIdx(); failed = false;
      Draw(im,model._shape,con,tri,model._clm._visi[idx]); 
    }else{
      if(show){cv::Mat R(im,cvRect(0,0,150,50)); R = cv::Scalar(0,0,255);}
      model.FrameReset(); failed = true;
    }     

    //draw framerate on display image 
    if(fnum >= 9){      
      t1 = cvGetTickCount();
      fps = 10.0/((double(t1-t0)/cvGetTickFrequency())/1e+6); 
      t0 = t1; fnum = 0;
    }else fnum += 1;
    if(show){
      sprintf(sss,"%d frames/sec",(int)round(fps)); text = sss;
      cv::putText(im,text,cv::Point(10,20),
		  CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    }

    cv::Mat normFaceGray;
    normFace = normImg(normRect);
    cv::cvtColor(normFace, normFaceGray,CV_BGR2GRAY);
    //cv::resize(normFaceGray,normFaceGray, cv::Size(92,112));
    cv::imshow("Face Normalizada",  normFaceGray);
    imgCount++;

    /* Now perform the prediction, see how easy that is: */
    int Prediction = -1;
    double confidence = 0.0;
    int position = -1;
    std::string nameFound;
    faceRec->predict(normFaceGray, Prediction, confidence);
    position = find(labels.begin(), labels.end(), Prediction) - labels.begin();
    nameFound = names[position];

    /* Create the text we will annotate the box with: */
    std::string grayBoxText = cv::format("Prediction = %d || Confidence = %.2f || Name = %s", Prediction, confidence, nameFound.c_str());          

    /* And now put it into the images (BGR AND GRAY): */
    putText(im, grayBoxText, cv::Point(300, 300), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,255), 2.0);


    //show image and check for user input
    cv::imshow("Face Tracker", im); 


    /* Writes image on the disk */
    comPressParam.push_back(CV_IMWRITE_PNG_COMPRESSION);
    comPressParam.push_back(5);
    std::string result;
    std::stringstream sstm;
    sstm << imgOut << imgCount << ".png";
    result = sstm.str();
    cv::imwrite(result, normFaceGray, comPressParam);

    int c = cvWaitKey(1);
    if(char(c) == 's')break; else if(char(c) == 'd')model.FrameReset();

    #if KINECT
        listener.release(frames);
    #endif
  }
  // TODO: restarting ir stream doesn't work!
  // TODO: bad things will happen, if frame listeners are freed before dev->stop() :(
  #if KINECT
    dev->stop();
    dev->close();
  #endif
  
  return 0;
}
//=============================================================================
