#include <opencv2/opencv.hpp>

namespace ContAuth
{
	class user
	{
	public:
		cv::Mat averageFace;
		std::ofstream resultsFile;
		std::vector<cv::Mat> Images;
		std::vector<int> Labels;
		cv::Ptr<cv::face::FaceRecognizer> faceRec;

		user(){
			faceRec = cv::face::createLBPHFaceRecognizer();
		};
		void pushLabel();
		~user();
	private:
	};
}