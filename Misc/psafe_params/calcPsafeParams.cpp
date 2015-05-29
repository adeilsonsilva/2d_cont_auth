#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv/highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/face.hpp>
#include <iostream>
#include <fstream>
#include <cmath>

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

double media(int cont, std::vector<double>& confidence) { 
    double soma = 0; 
    for(int i = 0; i < cont; i++) { 
        soma += confidence.at(i); 
    } 
    return soma/cont; 
} 

double desvioPadrao(int cont, std::vector<double>& confidence, double media) {
    double var;
    for(int i = 0; i < cont; i++){
        /*var(x) = Sn ((xi - média)^2 ) /N*/
        var = (var + (pow(confidence.at(i) - media,2))/cont);
        /*s(x) = Sn ((xi - média)^2) / (N - 1)*/
        //var_amostra = (var_amostra + (pow(confidence.at(i) - media,2))/(cont-1));
    } 
    return sqrt(var);
}

int main()
{
    std::vector<cv::Mat> userImages;
    std::vector<int> userLabels;
    std::vector<double> safe, nonsafe;
    int Prediction = -1;
    double confidence = 0.0;
    std::ofstream resultsFile, scoreFile;
    double miSafe, sigmaSafe, miNonSafe, sigmaNonSafe;

    resultsFile.open("Results.txt");
    scoreFile.open("Score.txt");
    userImages.reserve(5);
    userLabels.reserve(5);

    /* Caminho do CSV: */
    std::string csvFilePath = "../dataset/csv.ext";
    std::string fn_csv = std::string(csvFilePath);

    /* Os seguintes vetores guardarão as imagens, os nomes dos personagens e as labels */
    std::vector<cv::Mat> images;
    std::vector<std::string> names;
    std::vector<int> labels;

    /* Leitura dos dados */
    try {
      read_csv(fn_csv, images, labels, names);
    }catch (cv::Exception& e) {
     std::cerr << "Erro ao abrir o arquivo \"" << fn_csv << "\" Motivo: " << e.msg << std::endl;
     // nothing more we can do
     exit(1);
    }


    cv::Ptr<cv::face::FaceRecognizer> faceRec = cv::face::createLBPHFaceRecognizer();

    for (int i = 0; i < images.size() - 5; i+=5)
    {
        for (int j = 0; (j < 5); j++)
        {
            try{
                userImages.push_back(images[i+j].clone());
                userLabels.push_back(labels[i+j]);
                std::cout << "Imagem: " << i+j << std::endl; 
            }catch (cv::Exception& e) {
                std::cerr << "Erro ao montar userImages. Motivo: " << e.msg << std::endl;
                break;
            }
        }

        try{
            std::cout << "Treinando!" <<std::endl;
            faceRec->train(userImages, userLabels);
            std::cout << "Fim do treino!" << std::endl;
        }catch (cv::Exception& e){
            std::cerr << "Erro no treino. Motivo: " << e.msg << std::endl;
            break;
        }

        for (int k = 0; k < images.size(); k++)
        {
            try{
                faceRec->predict(images[k].clone(), Prediction, confidence);
            }catch (cv::Exception& e){
                std::cerr << "Erro na predição. Motivo: " << e.msg << std::endl;
            }

            if (labels[k] == userLabels[0])
            {
                safe.push_back(confidence);
                if(scoreFile.is_open()) scoreFile << "Safe: " << confidence << " Prediction: " << Prediction << " User: " << userLabels[0] << " Labels [" << k << "]: " << labels[k] << std::endl;
            } else {
                nonsafe.push_back(confidence);
                if(scoreFile.is_open()) scoreFile << "Non-Safe: " << confidence << " Prediction: " << Prediction << " User: " << userLabels[0] << " Labels[" << k << "]: " << labels[k] << std::endl;
            }
        }
        std::cout << "Fim do reconhecimento!" << std::endl;
        userImages.clear();
        userLabels.clear();
    }

    std::cout << "Calculando média!" << std::endl;
    miSafe = media(safe.size(), safe);
    miNonSafe = media(nonsafe.size(), nonsafe);
    std::cout << "Calculando desvio padrão!" << std::endl;
    sigmaSafe = desvioPadrao(safe.size(), safe, miSafe);
    sigmaNonSafe = desvioPadrao(nonsafe.size(), nonsafe, miNonSafe);

    std::cout << "Salvando!" << std::endl;
    if(resultsFile.is_open()) resultsFile << "Safe Size: " << safe.size() << " Non-safe Size: " << nonsafe.size() << std::endl;
    if(resultsFile.is_open()) resultsFile << "MiSafe: " << miSafe << " MiNonSafe: " << miNonSafe << std::endl;
    if(resultsFile.is_open()) resultsFile << "SigmaSafe: " << sigmaSafe << " SigmaNonSafe: " << sigmaNonSafe << std::endl;
    scoreFile.close();
    resultsFile.close();
    return 0;
}