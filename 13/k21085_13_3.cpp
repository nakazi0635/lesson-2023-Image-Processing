//g++ dip13_3.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    cv::VideoCapture capture("colorful.mp4");
    if (!capture.isOpened()) {
        printf("Capture not found\n");
        return -1;
    }

    cv::Mat frameImage1, frameImage2, diffImage;
    int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::Size imageSize(width, height);
    printf("imageSize = (%d, %d)\n", width, height);
    cv::Vec3b frame_1, frame_2;

    cv::namedWindow("Frame at 1s");
    cv::namedWindow("Frame at 3.5s");
    cv::namedWindow("Final");

    capture.set(cv::CAP_PROP_POS_MSEC, 1000);
    capture >> frameImage1;
    if (frameImage1.empty()) {
        printf("Frame at 1s is empty\n");
        
        return -1;
    }
    cv::imshow("Frame at 1s", frameImage1);

    capture.set(cv::CAP_PROP_POS_MSEC, 4000);
    capture >> frameImage2;
    if (frameImage2.empty()) {
        printf("Frame at 4.0s is empty\n");
        return -1;
    }
    cv::imshow("Frame at 4.0s", frameImage2);

    for (int j = 0; j < frameImage1.rows; j++){
        for (int i = 0; i < frameImage1.cols; i++){
            frame_1 = frameImage1.at<cv::Vec3b>(j, i);
            frame_2 = frameImage2.at<cv::Vec3b>(j, i);
            if(!(abs(frame_1[0] - frame_2[0]) < 20 && abs(frame_1[1] - frame_2[1]) < 20 && abs(frame_1[2] - frame_2[2]) < 20)||i < 20||i > 1260){
                frame_1[0] = 100;
                frame_1[1] = 100;
                frame_1[2] = 100;
            }
            frameImage2.at<cv::Vec3b>(j, i) = frame_1;
        }
    }

    cv::imshow("Final", frameImage2);

    // Wait for user to press a key
    cv::waitKey(0);

    cv::imwrite("result.jpg", frameImage2);

    capture.release();

    printf("Finished\n");
    return 0;
}
