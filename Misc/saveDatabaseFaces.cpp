#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <cstdlib>

int main(int argc, char *argv[]) {
	cv::Mat img, face, gray;
    cv::Rect normRect(200, 200, 200, 200);
	cv::VideoCapture video(0);

    if(argc < 2) {
        std::cout << "Usage: ./a.out <PERSON'S NAME>" << std::endl;
        exit(1);
    }

    std::string name = argv[1];
    std::string commandStr;
    const char* commandStr_C;
    std::stringstream command;
    command << "cd dataset && mkdir " << name;
    commandStr = command.str();
    commandStr_C = commandStr.c_str();
    //std::cout << "command: " << commandStr << std::endl;
    if(std::system(commandStr_C) != 0){
        std::cout << "Mkdir error!" << std::endl;
        exit(1);
    }

    /* Parâmetros para salvar a imagem */
    std::vector<int> comPressParam;
    int imgCount = 0;
    std::stringstream imgOut;
    imgOut << "dataset/" << name << "/Img";
    std::string imgOutStr = imgOut.str();
    std::cout << imgOutStr << std::endl;
	
	while(1) {
		video >> img; 
        cv::resize(img,img, cv::Size(960,540));
        cv::flip(img,img,1); 
        cv::cvtColor(img,gray,CV_BGR2GRAY);
		cv::imshow("Image", gray);
        face = gray(normRect);
        cv::imshow("Face", face);
        imgCount++;

        /* Writes image on the disk */
        comPressParam.push_back(CV_IMWRITE_PNG_COMPRESSION);
        comPressParam.push_back(5);
        std::string result;
        std::stringstream sstm;
        sstm << imgOutStr << imgCount << ".png";
        result = sstm.str();
        cv::imwrite(result, face, comPressParam);
        int c = cv::waitKey(1);
        if(char(c) == 's')break;
	}
}
