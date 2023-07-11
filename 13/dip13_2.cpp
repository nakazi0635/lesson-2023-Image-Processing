//g++ dip13.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[])
{
    cv::VideoCapture capture("senro.mov");
    if (capture.isOpened()==0) {
        printf("Capture not found\n");
        return -1;
    }

    int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::Size imageSize(width, height);

    printf("imageSize = (%d, %d)\n", width, height);

    cv::namedWindow("Frame");
    cv::namedWindow("Running Average");

    cv::Mat frameImage;
    cv::Mat sumImage = cv::Mat::zeros(height, width, CV_64FC3);  // double type, 3 channels
    cv::Mat avgImage;
    cv::VideoWriter writer("average_process.mp4", cv::VideoWriter::fourcc('M','J','P','G'), 30, imageSize, true);
    int frameCount = 0;

    while (1) {
        capture >> frameImage;
        if (frameImage.data==NULL) {
            break;
        }

        cv::Mat doubleImage;
        frameImage.convertTo(doubleImage, CV_64FC3);
        sumImage += doubleImage;
        avgImage = sumImage / ++frameCount;
        cv::Mat displayImage;
        avgImage.convertTo(displayImage, CV_8UC3);

        cv::imshow("Frame", frameImage);
        cv::imshow("Running Average", displayImage);

        writer.write(displayImage);

        int key = cv::waitKey(20);
        if (key=='q')
            break;
    }

    cv::imwrite("average_image.jpg", avgImage);

    capture.release();
    printf("Finished\n");
    return 0;
}
