//(OpenCV4) g++ dip15.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

int main(int argc, const char* argv[])
{
    //①画像ファイルの読み込み
    //画像ファイル"ferarri.jpg"を読み込んで，画像データ"sourceImage"に格納
    cv::Mat sourceImage = cv::imread("images/image01.png", cv::IMREAD_COLOR);
    cv::Mat sourceImage2 = cv::imread("images/image02.png", cv::IMREAD_COLOR);
    if (sourceImage.data==0 || sourceImage2.data==0) {  //画像ファイルが読み込めなかった場合
        printf("File not found\n");
        exit(0);
    }
    printf("Width=%d, Height=%d\n", sourceImage.cols, sourceImage.rows);
    
    //録画用
    // cv::VideoWriter rec("rec.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 30, sourceImage.size());
    
    //②画像表示用ウィンドウの生成
    cv::namedWindow("Translate");
    
    //③3チャンネル画像"translateImage"の確保（画像ファイルと同サイズ）
    //"sourceImage"と同サイズ・3チャンネル・ゼロで初期化
    //cv::Mat translateImage = cv::Mat::zeros(sourceImage.size(), CV_8UC3);
    cv::Mat translateImage(sourceImage.size(), CV_8UC3);
    cv::Mat tempImage(sourceImage.size(), CV_8UC3);
    cv::Mat resultImage(sourceImage.size(), CV_8UC3);
    
    //sourceImage2.copyTo(resultImage);
    sourceImage2.copyTo(translateImage);
    double angle = 60.0; //回転角度
    double scale = 1.0;  //拡大率
    
    
    
    while(1){
        //④回転移動行列"rotateMat"の生成
        cv::Point2f center = cv::Point2f(919, 216);  //回転中心
        cv::Mat rotateMat = cv::getRotationMatrix2D(center, angle, scale); //行列生成

        // Make a copy of sourceImage2 to translateImage
        sourceImage2.copyTo(translateImage);

        // Apply the rotation to sourceImage and store it in tempImage
        cv::warpAffine(sourceImage, tempImage, rotateMat, translateImage.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT, cv::Scalar(0,0,0));

        // Blend translateImage and tempImage with respective weights
        double alpha = 0.5; // Weight for translateImage
        double beta = 0.5; // Weight for tempImage
        cv::addWeighted(translateImage, alpha, tempImage, beta, 0.0, resultImage);

        //⑥"resultImage"の表示
        cv::imshow("Translate", resultImage);
        
        //⑦キー入力待ち
        int key = cv::waitKey(20);
        
        //'q'が押されたら無限ループ脱出
        if (key == 'q') {
            break;
        }
    }
    cv::imwrite("key1.png", resultImage);	
    
    
    return 0;
}