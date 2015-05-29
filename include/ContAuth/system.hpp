#include <opencv2/opencv.hpp>
#define tset(t) gettimeofday(t, NULL)
typedef struct timeval TIME;

namespace ContAuth
{
	class system
	{
	public:
		cv::Mat background, standby, login, safe, unsafe, stageImg;
		cv::Mat im, gray;
		int imgCount;
		float pSafe, pAttacked, lastPSafe, lastPAttacked, k;
		float miSafe, sigmaSafe, miAttacked, sigmaAttacked;
		float dt, w, history[500];
		std::ofstream resultsFile;
		std::vector<int> comPressParam;
		cv::VideoWriter videoFile;
		TIME t, ct;
		std::stringstream videoPath, resultsPath, command;
		std::string commandStr, folderName, folderPath, videoName, resultsName;


		system(){
			imgCount = 0;
			background = cv::imread("../interface/background.png", 1);
			login = cv::imread("../interface/gray.png", 1);
			standby = cv::imread("../interface/yellow.png", 1);
			safe = cv::imread("../interface/green.png", 1);
			unsafe = cv::imread("../interface/red.png", 1);
			meanFace = cv::imread("../user/MeanFace.jpg");

			lastPSafe = 0.0f;
			lastPAttacked = 1.0f;
			k = -logf(2.0f)/15.0;
			miSafe = 20.2725f;
			sigmaSafe = 16.1414f;
			miAttacked = 43.4733f;
			sigmaAttacked = 7.3204f;

		};
		void init();
		void finish();
		void changeStage(STAGE newStage);
		int getStage();
		bool checkStage(STAGE lookStage);
		void calcPsafe();
		void printHistory();
		void createFolder();
		void saveVideo();
		void saveImage(std::string path);
		void saveResults();
		std::string getTime();
		~system();
	private:
		STAGES stage;
	};
}