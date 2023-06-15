// g++ dip03.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[])
{
    // ①カメラの初期化
    cv::VideoCapture capture(0); // カメラ0番をオープン
    // カメラがオープンできたかどうかをチェック
    if (capture.isOpened() == 0)
    {
        printf("Camera not found\n");
        return -1;
    }

    // ②画像格納用インスタンス準備
    int width = 640, height = 360;                                  // 処理画像サイズ
    cv::Mat captureImage;                                           // キャプチャ用
    cv::Mat frameImage = cv::Mat(cv::Size(width, height), CV_8UC3); // 処理用
    cv::Mat grayImage;                                              // 処理用
    cv::Mat thrImage;
    cv::Mat recImage = cv::Mat(cv::Size(width, height), CV_8UC3); // ファイル出力用

    // ③ウィンドウの生成と移動
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Result");
    cv::moveWindow("Result", 0, height);
    //(X)ビデオライタ生成
    cv::VideoWriter rec("rec.mpg", cv::VideoWriter::fourcc('P', 'I', 'M', '1'), 30, recImage.size());

    // // ④カメラから1フレーム読み込んでcaptureImageに格納（CV_8UC3）
    // capture >> captureImage;

    // // ⑤captureImageをframeImageに合わせてサイズ変換して格納
    // cv::resize(captureImage, frameImage, frameImage.size());

    // // ⑥画像処理
    // cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);

    // // ⑦ウィンドウへの画像の表示
    // cv::imshow("Frame", frameImage);
    // cv::imshow("Result", grayImage);

    // // ⑧キー入力待ち
    // char key = cv::waitKey(0);

    // 動画像処理無限ループ
    while (1)
    {
        // 4カメラから 1 フレーム読み込んで captureImage に格納
        capture >> captureImage;
        if (captureImage.data == 0)
            break; // フレーム読み込みに失敗した場合には無限ループから脱出
        // 5captureImage を frameImage に合わせてサイズ変換して格納
        cv::resize(captureImage, frameImage, frameImage.size());
        // 6画像処理
        cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);
        cv::threshold(grayImage, thrImage, 100, 255, cv::THRESH_BINARY);
        // 7ウィンドウへの画像の表示
        cv::imshow("Frame", frameImage);
        cv::imshow("Result", thrImage);
        //(Y)動画ファイル書き出し
        cv::cvtColor(thrImage, recImage, cv::COLOR_GRAY2BGR); // 動画用3チャンネル画像生成
        // cv::threshold(grayImage, recImage, 100, 255, cv::THRESH_BINARY);
        rec << recImage; // ビデオライタに画像出力
        // 8'キー入力待ち
        char key = cv::waitKey(20);
        if (key == 'q')
            break;
    }

    return 0;
}
