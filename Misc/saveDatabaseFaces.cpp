#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/highgui.h>
#include <cstdlib>

#define KINECT 0
#define WRITEVIDEO 1
#define M_WIDTH 960
#define M_HEIGHT 540

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

int main(int argc, char *argv[]) {
	cv::Mat faceGray, faceColor, faceIr, faceDepth, imgColor, imgIr, imgDepth;
    cv::Rect normRect(350, 200, 200, 200);
	char resp = 'n';

    if(argc < 2) {
        std::cout << "Usage: ./a.out <PERSON'S NAME>" << std::endl;
        exit(1);
    }

	std::cout << "\t[Q || q] to quit." << std::endl << "\t[S || s] to save the image inside the red box." << std::endl;

    std::string name = argv[1];
    std::string commandStr;
    const char* commandStr_C;
    std::stringstream command;
    command << "cd dataset && mkdir " << name;
    commandStr = command.str();
    commandStr_C = commandStr.c_str();
    //std::cout << "command: " << commandStr << std::endl;
    if(std::system(commandStr_C) != 0){
        std::cout << "The directory " << name << " already exists! Overwrite contents [y/n]: ";
        std::cin >> resp;
		if(resp == 'n' || resp == 'N')exit(1);
    }

	if(resp != 'y' && resp != 'Y') {
		command.str("");
		command << "cd dataset/" << name << " && mkdir Gray && mkdir Color";
    	commandStr = command.str();
    	commandStr_C = commandStr.c_str();
		std::system(commandStr_C);
	}

    /* Parâmetros para salvar a imagem */
    std::vector<int> compressParamGray, compressParamColor;
    int imgCount = 0;
    std::stringstream imgGrayOut, imgColorOut;
    #if KINECT
        std::vector<int> compressParamIr, compressParamDepth;
        std::stringstream imgIrOut, imgDepthOut;
    #endif
    imgGrayOut << "dataset/" << name << "/GrayScale/Img";
    imgColorOut << "dataset/" << name << "/Color/Img";
    #if KINECT
        imgIrOut << "dataset/" << name << "/Ir/Img";
        imgDepthOut << "dataset/" << name << "Depth/Img";
    #endif
    std::string imgGrayOutStr = imgGrayOut.str();
	std::string imgColorOutStr = imgColorOut.str();
    #if KINECT
        std::string imgIrOutStr = imgIrOut.str();
        std::string imgDepthOutStr = imDepthOutStr.str();
    #endif	

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

        libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color | libfreenect2::Frame::Ir | libfreenect2::Frame::Depth);
        //libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color);
        libfreenect2::FrameMap frames;

        dev->setColorFrameListener(&listener);
        dev->setIrAndDepthFrameListener(&listener);
        dev->start();
    #endif

    #if WRITEVIDEO
        std::stringstream Filename;
        Filename << "dataset/" << name << "/" << getTime();
        std::string videoName = Filename.str();
        std::cout << videoName << std::endl;
        cv::VideoWriter videoFile(videoName, CV_FOURCC('M','J','P','G'), 5, cv::Size(M_WIDTH,M_HEIGHT), true);
    #endif

    #if KINECT == 0
        cv::VideoCapture Color(0);
    #endif

	while(1) {
        #if KINECT
            listener.waitForNewFrame(frames);
            libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
            libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
            libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];
            cv::Mat Color(rgb->height, rgb->width, CV_8UC3, rgb->data);
            cv::Mat Ir = cv::Mat(ir->height, ir->width, CV_32FC1, ir->data) / 20000.0f;
            cv::Mat Depth = cv::Mat(depth->height, depth->width, CV_32FC1, depth->data) / 4500.0f;
        #endif

		Color >> imgColor; 
        #if KINECT
            Ir >> imgIr;
            Depth >> imgDepth;
        #endif
        cv::resize(imgColor,imgColor, cv::Size(M_WIDTH,M_HEIGHT));
        cv::flip(imgColor, imgColor, 1);
        #if KINECT
            cv::resize(imgIr,imgIr, cv::Size(M_WIDTH,M_HEIGHT));
            cv::flip(imgIr, imgIr, 1);
            cv::resize(imgDepth,imgDepth, cv::Size(M_WIDTH,M_HEIGHT));
            cv::flip(imgDepth, imgDepth, 1);
        #endif
        cv::Mat imgC = imgColor.clone();
        #if KINECT
            cv::Mat imgI = imgIr.clone();
            cv::Mat imgD = imgDepth.clone();
        #endif
        faceColor = imgColor(normRect);
        cv::cvtColor(faceColor, faceGray, CV_BGR2GRAY);
        cv::imshow("FaceColor", faceColor);
		cv::imshow("FaceGray", faceGray);
        #if KINECT
            faceIr = imgIr(normRect);
            faceDepth = imgDepth(normRect);
            cv::imshow("FaceIr", faceIr);
            cv::imshow("FaceDepth", faceDepth);
        #endif
		cv::rectangle(imgC, normRect, cv::Scalar(0, 51, 250));
		cv::imshow("Image", imgC);

        #if WRITEVIDEO
            videoFile.write(imgColor);
        #endif

        int c = cv::waitKey(1);
        if(char(c) == 'q' || char(c) == 'Q')break;
		else if(char(c) == 's' || char(c) == 'S') {
			/* Writes image on the disk */
        	compressParamGray.push_back(CV_IMWRITE_PNG_COMPRESSION);
        	compressParamGray.push_back(5);
        	compressParamColor.push_back(CV_IMWRITE_PNG_COMPRESSION);
			compressParamColor.push_back(5);
            #if KINECT
            	compressParamIr.push_back(CV_IMWRITE_PNG_COMPRESSION);
            	compressParamIr.push_back(5);
            	compressParamDepth.push_back(CV_IMWRITE_PNG_COMPRESSION);
			    compressParamDepth.push_back(5);
            #endif
			std::string resultGray,  resultColor;
       		std::stringstream sstmGray,  sstmColor;
            #if KINECT
                std::string resultIr,  resultDepth;
       		    std::stringstream sstmIr,  sstmDepth;
            #endif
        	sstmGray << imgGrayOutStr << imgCount << ".png";
			sstmColor << imgColorOutStr << imgCount << ".png";
            #if KINECT
                sstmIr << imgIrOutStr << imgCount << ".png";
			    sstmDepth << imgDepthOutStr << imgCount << ".png";
            #endif
        	resultGray = sstmGray.str();
			resultColor = sstmColor.str();
            #if KINECT
                resultIr = sstmIr.str();
                resultDepth = sstmDepth.str();
            #endif
        	cv::imwrite(resultGray, faceGray, compressParamGray);
			cv::imwrite(resultColor, faceColor, compressParamColor);
            #if KINECT
                cv::imwrite(resultIr, faceIr, compressParamIr);
			    cv::imwrite(resultDepth, faceDepth, compressParamDepth);
            #endif
			std::cout << "Images saved!" << std::endl;
        	imgCount++;
		}
	}
}
