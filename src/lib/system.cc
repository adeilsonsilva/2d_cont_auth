#include <ContAuth/system.hpp>

std::string getTime(){
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[100];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,100,"%d-%m-%Y_%I:%M:%S",timeinfo);
  std::string str(buffer);

  return str;
}

void init()
{
	const char* commandStr_C;
	char resp;

	folderName = ContAuth::getTime();
	std::cout << "Pasta de saída: ../out/" << folderName << std::endl;
	folderPath = "../out/";
	command << "cd ../out && mkdir " << folderName << " && cd " << folderName << " && mkdir faces && mkdir frames";
	commandStr = command.str();
	commandStr_C = commandStr.c_str();
	if(std::system(commandStr_C) != 0){
	       std::cout << "O diretorio " << folderName << " já existe! Sobrescrever? [s/n]: ";
	       std::cin >> resp;
	 		 if(resp == 'n' || resp == 'N')exit(1);
	}

	videoPath << folderPath << folderName << "/video.avi";
	videoName = videoPath.str();
	this->videoFile.open(videoName, CV_FOURCC('M','J','P','G'), 5, cv::Size(M_WIDTH,M_HEIGHT), true);
	if (!this->videoFile.isOpened()){
        std::cout  << "Could not open the output video for write: " << videoName << std::endl;
        exit(1);
    }

	resultsPath << folderPath << folderName << "/results.txt";
	resultsName = resultsPath.str();
	this->resultsFile.open(resultsName.c_str());
	if(this->resultsFile.is_open()){
		std::cout << "Could not open results file" << resultsName << std::endl;
		exit(1);
	}
}

void finish()
{
	this->resultsFile.close();
}

void changeStage(STAGE newStage){
	this->stage = newStage;
}

void checkStage(STAGE lookStage){
	return this->stage == lookStage;
}

void clearHistory(){
  for(int i=0; i < 500; i++)
		this->history[i] = 1.0f;
}

void tSetTime(){
	tset(&this->ct);
	this->t = this->ct;
}

double tGetTime(TIME *t0, TIME *t1)  {
	return t1->tv_sec-t0->tv_sec+(t1->tv_usec-t0->tv_usec)*0.000001;
}

void loadBackground() {
	  /* Copia imagens para o background */
	  cv::resize(im,smallImage, cv::Size(320,240));
	  cv::Rect ImRoi( cv::Point( 120, 186 ), smallImage.size() );
	  smallImage.copyTo( background( ImRoi ) );
	  cv::resize(normFace,normFace, cv::Size(195,150));
	  cv::Rect NormRoi(cv::Point( 451, 185 ), normFace.size() );
	  normFace.copyTo( background(NormRoi ) );
		cv::Rect StageRoi(cv::Point( 655, 187), stageImg.size() );
		stageImg.copyTo(background(StageRoi));	
}

void saveResults(){
	std::string grayBoxText;
	grayBoxText = cv::format("Frame: %d || Confidence: %.2f || PSafe: %.2f \n", this->imgCount, this->confidence, this->history[499]);
	if(resultsFile.is_open())
		resultsFile << grayBoxText;
}

void saveImage(std::string path) {
	std::string result;
  	std::stringstream sstm;
	comPressParam.push_back(CV_IMWRITE_PNG_COMPRESSION);
	comPressParam.push_back(5);

	sstm << folderPath << folderName << path << this->imgCount << ".png";
	result = sstm.str();
	cv::imwrite(result, im, comPressParam);
}

void showBackground(){
	cv::imshow("Autenticação Facial 2D Contínua", this->background);
}

void setImage(cv::Mat& frame){
	cv::resize(frame,this->im, cv::Size(M_WIDTH,M_HEIGHT));
	cv::flip(this->im,this->im,1);
	cv::cvtColor(this->im,this->gray,CV_BGR2GRAY);
}