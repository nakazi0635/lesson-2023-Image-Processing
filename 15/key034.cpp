//(OpenCV4) g++ key045.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    // 動画ファイルを開く
    cv::VideoCapture cap("cave.mp4");
    
    if (!cap.isOpened())
    {
        std::cout << "動画ファイルを開けませんでした。\n";
        return -1;
    }
    
    cv::Mat frame;
    int frame_number = 0;
    
    while (true)
    {
        cap >> frame;  // フレームを取得

        if (frame.empty()) // フレームが空の場合（動画の終端）
        {
            break;
        }
        

        // 1200フレーム目と3200フレーム目を保存
        if (frame_number == 1200 || frame_number == 3200)
        {
            std::stringstream ss;
            ss << "key" << frame_number << ".png";
            cv::imwrite(ss.str(), frame);
        }
        frame_number++;
    }
    
    cap.release();

    return 0;
}
