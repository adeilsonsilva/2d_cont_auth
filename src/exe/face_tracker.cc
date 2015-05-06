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
#include "base.hpp"

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
    cv::VideoWriter videoFile(videoName, CV_FOURCC('M','J','P','G'), 5, cv::Size(M_WIDTH,M_HEIGHT), true);
  #endif
  
  //initialize camera and display window
  cv::Mat frame,gray,im; double fps=0; char sss[256]; std::string text; 
  #if KINECT == 0
        cv::VideoCapture video("vid1.avi");
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
    meanDraw(meanFace,model._shape,tri,model._clm._visi[idx]); 
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
  

  // The following lines simply get the last images from
  // your dataset and remove it from the vector. This is
  // done, so that the training data (which we learn the
  // cv::FaceRecognizer on) and the test data we test
  // the model with, do not overlap.
  cv::Mat testSample = images[images.size() - 1];
  int testLabel = labels[labels.size() - 1];
  images.pop_back();
  labels.pop_back();

  */

  /* Cria um FaceRecognizer e treina em cima dele: */
  cv::Ptr<cv::face::FaceRecognizer> faceRec = cv::face::createLBPHFaceRecognizer();
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
    
    //grab image, resize, flip and equalize histogram
    cv::resize(frame,im, cv::Size(M_WIDTH,M_HEIGHT));
    cv::flip(im,im,1); 
    cv::cvtColor(im,gray,CV_BGR2GRAY); 
    cv::equalizeHist(gray, gray);    

    #if WRITEVIDEO
        videoFile.write(im);
    #endif

    //track this image
	//model.FrameReset(); failed = true;
    std::vector<int> wSize; if(failed)wSize = wSize2; else wSize = wSize1; 
    if(model.Track(gray,wSize,fpd,nIter,clamp,fTol,fcheck) == 0){
      int idx = model._clm.GetViewIdx(); failed = false;
      Draw(im,model._shape,tri,model._clm._visi[idx]); 
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
