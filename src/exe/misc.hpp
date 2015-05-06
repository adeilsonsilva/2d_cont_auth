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

void createFolders() {
  std::string folderName = getTime();
  std::cout << "Pasta de saída: ../out/" << folderName << std::endl;
  std::vector<int> comPressParam;
  int imgCount = 0;
  std::string folderPath = "../out/";
  std::string commandStr;
  const char* commandStr_C;
  std::stringstream command;
  char resp;
  command << "cd ../out && mkdir " << folderName << " && cd " << folderName << " && mkdir faces && mkdir frames";
  commandStr = command.str();
  commandStr_C = commandStr.c_str();
  //std::cout << "command: " << commandStr << std::endl;
  if(std::system(commandStr_C) != 0){
         std::cout << "O diretorio " << folderName << " já existe! Sobrescrever? [s/n]: ";
         std::cin >> resp;
   		 if(resp == 'n' || resp == 'N')exit(1);
  }
  std::stringstream videoPath;
  videoPath << folderPath << folderName << "/video.avi";
  std::string videoName = videoPath.str();
  std::stringstream resultsPath;
  resultsPath << folderPath << folderName << "/results.txt";
  std::string resultsName = resultsPath.str();
  std::ofstream resultsFile;
  resultsFile.open(resultsName.c_str());
  return;
}
