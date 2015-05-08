//void warpTextureFromTriangle(cv::Point2f srcTri[3], cv::Mat originalImage, cv::Point2f dstTri[3], cv::Mat warp_final){
void warpTextureFromTriangle(cv::Point2f srcTri[3], cv::Mat originalImage, cv::Point2f dstTri[3]){
    int x1 = 0, y1 = 0;
    cv::Mat warp_mat( 2, 3, CV_32FC1 );
    cv::Mat warp_dst, warp_mask;
    cv::Point trianglePoints[3];
	trianglePoints[0] = dstTri[0];
    trianglePoints[1] = dstTri[1];
    trianglePoints[2] = dstTri[2];
    warp_dst  = cv::Mat::zeros( originalImage.rows, originalImage.cols, originalImage.type() );
    warp_mask = cv::Mat::zeros( originalImage.rows, originalImage.cols, originalImage.type() );

    //std::cout << "Rows: " << originalImage.rows << " Cols: " << originalImage.cols << std::endl;

    // Get the Affine Transform
    for(int i=0;i<3;i++){
        srcTri[i].x -= x1;
        srcTri[i].y -= y1;
        dstTri[i].x -= x1;
        dstTri[i].y -= y1;

    }

    warp_mat = cv::getAffineTransform( srcTri, dstTri );

    /// Apply the Affine Transform just found to the src image
    cv::Rect roi(x1, y1, ROI_X, ROI_Y);
    cv::Mat originalImageRoi = originalImage(roi);
    cv::Mat warp_dstRoi = warp_dst(roi);
    cv::warpAffine( originalImageRoi, warp_dstRoi, warp_mat, warp_dstRoi.size() );
	cv::fillConvexPoly(warp_mask, trianglePoints, 3, CV_RGB(255, 255, 255), CV_AA, 0);
	warp_dst.copyTo(normImg, warp_mask);
	/* Caso a Face Normalizada não esteja aparecendo,  descomente a linha abaixo e veja qual o ROI utilizado */
	//cv::imshow("ROI", originalImageRoi);
 }

void meanDraw(cv::Mat &image, cv::Mat &shape,cv::Mat &tri,cv::Mat &visi)
{
  int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

  //draw triangulation
  c = CV_RGB(0,0,0);
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
       visi.at<int>(tri.at<int>(i,1),0) == 0 ||
       visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    cv::line(image,p1,p2,c);
	pts.push_back(p1);
	pts.push_back(p2);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    cv::line(image,p1,p2,c);
	pts.push_back(p2);	
	normVec.push_back(pts);
    //std::cout << " Tri: " << pts.size() << std::endl;
	pts.clear();
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    cv::line(image,p1,p2,c);
  } 
  return;
}

void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &tri,cv::Mat &visi)
{
  int i,n = shape.rows/2; cv::Point2f p1,p2; cv::Scalar c;
  cv::Point2f orig[3], normPts[3];
  cv::Mat origIm = image.clone();

  //draw triangulation
  c = CV_RGB(255,255,255);
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
       visi.at<int>(tri.at<int>(i,1),0) == 0 ||
       visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    //cv::line(image,p1,p2,c);
    orig[0] = p1;
    orig[1] = p2;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    //cv::line(image,p1,p2,c);
    orig[2] = p2;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    //cv::line(image,p1,p2,c);
    normPts[0] = normVec[i][0];
    normPts[1] = normVec[i][1];
    normPts[2] = normVec[i][2];
    //warpTextureFromTriangle(orig, origIm, normPts, normImg);
    warpTextureFromTriangle(orig, origIm, normPts);
  }
#if 0
  //draw connections
  c = CV_RGB(0,0,255);
  for(i = 0; i < con.cols; i++){
    if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
       visi.at<int>(con.at<int>(1,i),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
		   shape.at<double>(con.at<int>(0,i)+n,0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
		   shape.at<double>(con.at<int>(1,i)+n,0));
    //cv::line(image,p1,p2,c,1);
  }
  //draw points
  for(i = 0; i < n; i++){    
    if(visi.at<int>(i,0) == 0)continue;
    p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
    c = CV_RGB(255,255,255); cv::circle(image,p1,2,c);
  }
#endif
  return;
}
