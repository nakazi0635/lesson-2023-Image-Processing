//g++ dip13_1.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

int main(int argc, char* argv[])
{
    cv::VideoCapture capture("pantora.mp4");
    if (capture.isOpened()==0) {
        printf("Capture not found\n");
        return -1;
    }
    
    cv::Mat frameImage, grayImage, binImage, overImage;
    int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::Size imageSize(width, height);
    printf("imageSize = (%d, %d)\n", width, height);

    cv::namedWindow("Frame");
    cv::namedWindow("Bin");
    cv::namedWindow("Over");
    cv::VideoWriter rec("rec.mov", cv::VideoWriter::fourcc('m','p','4','v'), 30, cv::Size(width, height));

    std::vector< std::vector<cv::Point> > contours;

    while (1) {
        capture >> frameImage;
        if (frameImage.data==NULL) {
            break;
        }

        frameImage.copyTo(overImage);

        cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);
        cv::threshold(grayImage, binImage, 188, 255, cv::THRESH_BINARY_INV);
        cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3,3));
        cv::erode(binImage,binImage,element,cv::Point(-1,-1),10);
        cv::dilate(binImage,binImage,element,cv::Point(-1,-1),10);

        cv::Mat hsvImage;
        cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);
        cv::Scalar lower_yellow = cv::Scalar(20, 100, 100); // Adjust these values
        cv::Scalar upper_yellow = cv::Scalar(40, 255, 255); // Adjust these values
        cv::Mat yellowMask;
        cv::inRange(hsvImage, lower_yellow, upper_yellow, yellowMask);

        cv::Scalar lower_brown = cv::Scalar(8, 100, 20); // Adjust these values for brown color
        cv::Scalar upper_brown = cv::Scalar(18, 255, 100); // Adjust these values for brown color
        cv::Mat brownMask;
        cv::inRange(hsvImage, lower_brown, upper_brown, brownMask);

        cv::Scalar lower_skin = cv::Scalar(0, 58, 89); // Adjust these values for skin color
        cv::Scalar upper_skin = cv::Scalar(25, 173, 229); // Adjust these values for skin color
        cv::Mat skinMask;
        cv::inRange(hsvImage, lower_skin, upper_skin, skinMask);

        for (int j = 0; j < binImage.rows; j++){
            for (int i = 0; i < binImage.cols; i++){
                if(i < 200 || i > 1100){
                    binImage.at<uchar>(j, i) = 0;
                }
            }
        }

        cv::findContours(binImage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        for (int i=0; i<contours.size(); i++) {
            double area = cv::contourArea(contours[i]); // Compute the area of contour
            if(area > 20000) { // If area is greater than threshold
                cv::RotatedRect ellipse = cv::fitEllipse(contours[i]);
                double ratio = std::min(ellipse.size.width, ellipse.size.height) / std::max(ellipse.size.width, ellipse.size.height);
                
                std::cout << "Contour " << i << " ratio: " << ratio << std::endl; // Debug output

                cv::Mat contourMask = cv::Mat::zeros(binImage.size(), CV_8UC1);
                cv::drawContours(contourMask, contours, i, cv::Scalar(255), -1);
                
                cv::Mat yellowInContour;
                cv::bitwise_and(yellowMask, contourMask, yellowInContour);
                double yellowArea = cv::countNonZero(yellowInContour);

                cv::Mat brownInContour;
                cv::bitwise_and(brownMask, contourMask, brownInContour);
                double brownArea = cv::countNonZero(brownInContour);

                cv::Mat skinInContour;
                cv::bitwise_and(skinMask, contourMask, skinInContour);
                double skinArea = cv::countNonZero(skinInContour);

                // Compute min enclosing triangle for this contour
                std::vector<cv::Point> triangle;
                cv::minEnclosingTriangle(contours[i], triangle);
                double triangleArea = cv::contourArea(triangle);

                if (ratio > 0.85 && yellowArea / area > 0.5) { 
                    cv::drawContours(overImage, contours, i, cv::Scalar(0,0,255), 2, 8); // red
                    cv::circle(overImage, cv::Point(30,30), 20, cv::Scalar(0,0,255), -1, 8, 0);
                }else if (ratio > 0.85){
                    cv::drawContours(overImage, contours, i, cv::Scalar(255,0,0), 2, 8); // blue
                    cv::circle(overImage, cv::Point(70,30), 20, cv::Scalar(255,0,0), -1, 8, 0);
                }else if (area / triangleArea > 0.7 && skinArea / area > 0.5) {
                    cv::drawContours(overImage, contours, i, cv::Scalar(220,200,100), 2, 8); // cyan
                    cv::circle(overImage, cv::Point(190,30), 20, cv::Scalar(220,200,100), -1, 8, 0);
                } else if (ratio <= 0.85 && skinArea / area > 0.5) {
                    cv::drawContours(overImage, contours, i, cv::Scalar(250,70,220), 2, 8); // pink
                    cv::circle(overImage, cv::Point(150,30), 20, cv::Scalar(250,70,220), -1, 8, 0);
                }else{
                    cv::drawContours(overImage, contours, i, cv::Scalar(0,255,0), 2, 8); // green
                    cv::circle(overImage, cv::Point(110,30), 20, cv::Scalar(0,255,0), -1, 8, 0);
                }
            }
        }

        cv::imshow("Frame", frameImage);
        cv::imshow("Bin", binImage);
        cv::imshow("Over", overImage);

        int key = cv::waitKey(20);
        if (key=='q')
            break;
        rec << overImage;
    }
    
    capture.release();
    printf("Finished\n");
    return 0;
}
