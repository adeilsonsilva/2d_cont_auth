void saveLoginImage(){

		comPressParam.push_back(CV_IMWRITE_PNG_COMPRESSION);
    comPressParam.push_back(5); 
		std::string result;
	  std::stringstream sstm;
	  sstm << "user/" << imgCount << ".png";
	  result = sstm.str();
	  cv::imwrite(result, normFaceGray, comPressParam);
}

void train(){
	      try {
        std::cout << "Treinando imagens de usuário!" << std::endl;
        //faceRec->update(userImages, userLabels);
        //faceRec->train(userImages, userLabels);
        user.trainRec();
        std::cout << "Fim do treino!" << std::endl;
      }catch(cv::Exception& e){
        std::cerr << "Erro no Treino! Motivo: " << e.msg << std::endl;
        break;
      }
}

void pushParams(){
	Labels.push_back(USER_ID);
	Images.push_back(normFaceGray);
}

double recognize(){
  	int Prediction = -1;
  	double confidence = 0.0;
  	this->faceRec->predict(normFaceGray, Prediction, confidence);
  	return confidence;
}