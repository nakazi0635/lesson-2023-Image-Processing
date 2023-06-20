// g++ dip03.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[])
{
    // ①カメラの初期化
    cv::VideoCapture capture("dance.mov"); // カメラ0番をオープン
    // カメラがオープンできたかどうかをチェック
    if (capture.isOpened() == 0)
    {
        printf("Camera not found\n");
        return -1;
    }

    // ②画像格納用インスタンス準備
    int width = 640, height = 360; // 処理画像サイズ
    cv::Vec3b s;
    cv::Vec3i s1;
    cv::Mat captureImage;                                           // キャプチャ用
    cv::Mat frameImage = cv::Mat(cv::Size(width, height), CV_8UC3); // 処理用
    cv::Mat hsvImage;                                               // 処理用
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
        // cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);
        cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);
        // cv::threshold(grayImage, thrImage, 100, 255, cv::THRESH_BINARY_INV);
        // 7ウィンドウへの画像の表示
        for (int y = 0; y < frameImage.rows; y++)
        {
            for (int x = 0; x < frameImage.cols; x++)
            {
                s = hsvImage.at<cv::Vec3b>(y, x); //"sourceImage"の画素(x,y)の画素値を読み込んで"s"に格納 s[0]=0; //"s[0]"に0を代入
                // s[0] < 47 && s[0] > 40
                if ((s[0] > 40 || s[0] < 50) && s[1] > 162 && s[2] > 153)
                {
                    s1[0] = 0;
                    s1[1] = 0;
                    s1[2] = 0;
                    s = s1;                           // int型をunsignedchar型に丸める
                    hsvImage.at<cv::Vec3b>(y, x) = s; //"resultImage"の画素(x,y)に画素値"s"を書き込み
                }
            }
        }
        cv::imshow("Frame", frameImage);
        //(Y)動画ファイル書き出し
        cv::cvtColor(hsvImage, recImage, cv::COLOR_HSV2BGR); // 動画用3チャンネル画像生成
        cv::imshow("Result", recImage);
        // cv::threshold(grayImage, recImage, 100, 255, cv::THRESH_BINARY);
        rec << recImage; // ビデオライタに画像出力
        // 8'キー入力待ち
        char key = cv::waitKey(20);
        if (key == 'q')
            break;
    }

    return 0;
}
