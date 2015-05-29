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
#include "ContAuth/base.hpp"
//=============================================================================
int main(int argc, const char** argv)
{
  ContAuth::system sys();
  #if KINECT
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device *dev = freenect2.openDefaultDevice();

    if(dev == 0){
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

  /* Parâmetros para salvar os resultados */
  sys.init();


  //initialize camera and display window
  cv::Mat frame,gray,im, background, smallImage; double fps=0; char sss[256]; std::string text;
  #if KINECT == 0
    cv::VideoCapture video("video.avi");
  #endif

  int64 t1,t0 = cvGetTickCount();
  //cvNamedWindow("Face Tracker",1);
  std::cout << "Atalhos: "        << std::endl
	    << "s\t-\tSAIR"     << std::endl
	    << "d\t-\tREDETECTAR" << std::endl;

  cv::cvtColor(meanFace, meanGray,CV_BGR2GRAY);
  std::vector<int> wSize0; wSize0 = wSize2; 
  if(model.Track(meanGray,wSize0,fpd,nIter,clamp,fTol,fcheck) == 0){
    int idx = model._clm.GetViewIdx();
    meanDraw(meanFace,model._shape,tri,model._clm._visi[idx]); 
  }else{
    if(show){cv::Mat R(meanFace,cvRect(0,0,150,50)); R = cv::Scalar(0,0,255);}
    model.FrameReset();
  }
  //cv::imshow("MeanFace", meanFace); 


  bool failed = true;
  while(1){ 
	  model.FrameReset();
    #if KINECT
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        cv::Mat video(rgb->height, rgb->width, CV_8UC3, rgb->data);
    #endif

    #if KINECT == 0
        video >> frame;
    #else
        frame = video.clone();
    #endif

	if(frame.empty()) { std::cout << "Fim do vídeo!" << std::endl; break;}

    //grab image, resize, and flip
    sys.setImage(frame);
    // cv::resize(frame,im, cv::Size(M_WIDTH,M_HEIGHT));
    // cv::flip(im,im,1); 
    // cv::cvtColor(im,gray,CV_BGR2GRAY);

    //track this image
    std::vector<int> wSize; if(failed)wSize = wSize2; else wSize = wSize1; 
    if(model.Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0){
      int idx = model._clm.GetViewIdx(); failed = false;
      Draw(im,model._shape,tri,model._clm._visi[idx]); 
	    //model.FrameReset();
    }else{
      if(show){cv::Mat R(im,cvRect(0,0,150,50)); R = cv::Scalar(0,0,255);}
      model.FrameReset(); failed = true;
    }

    #if WRITEVIDEO
        videoFile.write(im);
    #endif

    //draw framerate on display image 
    // if(fnum >= 9){      
    //   t1 = cvGetTickCount();
    //   fps = 10.0/((double(t1-t0)/cvGetTickFrequency())/1e+6); 
    //   t0 = t1; fnum = 0;
    // }else fnum += 1;
    // if(show){
    //   sprintf(sss,"%d frames/sec",(int)round(fps)); text = sss;
    //   cv::putText(im,text,cv::Point(10,20),
		//   CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
    // }

    /* Framerate na tela */
    t1 = cvGetTickCount();
    fps = 10.0/((double(t1-t0)/cvGetTickFrequency())/1e+6); 
    t0 = t1;
    sprintf(sss,"%d frames/sec",(int)round(fps)); text = sss;
    cv::putText(im,text,cv::Point(10,20),
    CV_FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));


    cv::Mat normFaceGray;
    normFace = normImg(normRect);
    //cv::resize(normFace,normFaceGray, cv::Size(200,200));    
    cv::cvtColor(normFace, normFaceGray, CV_BGR2GRAY);
    cv::equalizeHist(normFaceGray, normFaceGray);
    cv::cvtColor(normFaceGray, normFace, CV_GRAY2RGB);
    //cv::imshow("Face Normalizada",  normFaceGray);
    sys->imgCount++;

	if(sys.checkStage(LOGIN)) {
    user.saveLoginImage();
		user.pushParams();
    user.pushNormImg();
		if(imgCount == 5) {
      sys.setTime();
      user.trainRec();
      sys.changeStage(CONT_AUTH);
      sys.clearHistory();
		}
	}

  sys.loadBackground();

	if(sys.checkStage(CONT_AUTH)) {
    double score = user.recognize();
    sys.calcPsafe(score);
		// pSafe = 1.0-0.5*(1.0+erf((confidence-miSafe)/(sigmaSafe*SQRT_2)));
		// pAttacked = 0.5*(1+erf((confidence-miAttacked)/(sigmaAttacked*SQRT_2)));
		// dt = tget(&t,&ct);
		// w = expf(k*dt);
		// lastPSafe *= w;
		// lastPAttacked *= w;
		// lastPSafe += pSafe;
		// lastPAttacked += pAttacked;
		// t = ct;
  //   dt = tget(&t,&ct);
		// w = expf(k*dt);
		// for(int i=1; i < 500; i++)
		// 	history[i-1] = history[i];
		// history[499] = (lastPSafe*w)/(lastPSafe+lastPAttacked);
    sys.drawBackground();
    // Timeline window (259,463) / 512x128
		// cv::Mat win=background(cv::Range(463,591), cv::Range(259,771));
		// win = cv::Scalar(255,255,255,255);

		// for(int i=0; i < 499; i++) {
		// 	cv::line(win, cv::Point(i,114-(int)(history[i]*100.0)), cv::Point(i+1,114-(int)(history[i+1]*100.0)), CV_RGB(0,255,0), 2, 8, 0);
		// }
		// cv::circle(win, cv::Point(499, 114-(int)(history[499]*100.0)), 3, cv::Scalar(255,0,0,0), 1, 8, 0);

    sys.saveResults();
	}

  sys.showBackground();

  sys.saveImage("/frames/");
  sys.saveImage("/faces/");

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
  
  resultsFile.close();
  return 0;
}
//=============================================================================
