//g++ dip01.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
#define FILE_NAME "photo.jpg"

int main (int argc, const char* argv[])
{
    //①画像ファイルの読み込み
    cv::Mat src_img = cv::imread(FILE_NAME);
    if (src_img.empty()) { //入力失敗の場合
        fprintf(stderr, "Cannot read image file: %s.\n", FILE_NAME);
        return (-1);
    }
    
    //②画像格納用インスタンスの生成
    cv::Mat gray_img;
    
    //③ウィンドウの生成と移動
    
    
    //④画像処理
    gray_img = cv::Mat(src_img.size(), CV_8UC1);
    cv::Mat dst_img;
    cv::Mat thr_img;
    cv::cvtColor(src_img, dst_img, cv::COLOR_BGR2GRAY);
    cv::threshold(dst_img, thr_img, 60, 255, cv::THRESH_BINARY);
    
    //⑤ウィンドウへの画像の表示

    
    //⑥キー入力待ち
    cv::imshow("INPUT", src_img); //画像の表示
    cv::imshow("OUTPUT", dst_img);
    cv::imshow("OUTPUT2", thr_img);
    cv::waitKey(); //キー入力待ち (止める)
    
    //⑦画像の保存

    
    return 0;
}
