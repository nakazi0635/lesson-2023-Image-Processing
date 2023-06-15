// g++ dip04.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[])
{
    int width = 640, height = 480;

    // ①カメラの初期化
    cv::VideoCapture capture(0); // カメラ0番をオープン
    // カメラがオープンできたかどうかをチェック
    if (capture.isOpened() == 0)
    {
        printf("Camera not found\n");
        return -1;
    }

    // ②画像格納用インスタンス準備
    cv::Mat captureImage;                                           // キャプチャ用
    cv::Mat frameImage = cv::Mat(cv::Size(width, height), CV_8UC3); // 処理用
    cv::Mat hsvImage(cv::Size(width, height), CV_8UC3);             // 3チャンネル
    cv::Mat resultGImage(cv::Size(width, height), CV_8UC3);         // 1チャンネル
    cv::Mat recImage(cv::Size(width, height), CV_8UC3);             // 3チャンネル

    // ③ウィンドウの生成と移動
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Gray");
    cv::moveWindow("Gray", width, 0);
    cv::namedWindow("Result");
    cv::moveWindow("Result", width, height);

    // ④ルックアップテーブルの作成
    unsigned char lookupTable[256];
    unsigned char lookupTable_1[256];
    for (int i = 0; i < 256; i++)
    {
        lookupTable[i] = (255 - (255 - i)) / 63 * 63;
        lookupTable_1[i] = (i/(256/4))*(255/3);
        printf("%d , %d\n", lookupTable[i], lookupTable_1[i]);
    }

    // ⑤ビデオライタ生成(ファイル名，コーデック，フレームレート，フレームサイズ)
    cv::VideoWriter rec("rec.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 30, recImage.size());


    cv::Vec3b val;
    // ⑥動画像処理無限ループ
    while (1)
    {
        //(a)カメラから1フレームを" captureImage"に読み込み
        capture >> captureImage;
        if (captureImage.data == NULL)
            break;

        //(b)" captureImage"をリサイズして" frameImage"に格納
        cv::resize(captureImage, frameImage, frameImage.size());

        //(c)"frameImage"をグレースケールに変換して"grayImage"に格納
        cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);

        //(d)"grayImage"の各画像を走査して，ルックアップテーブルに基づいて画素値変換して"resultGImage"に格納
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                // 座標(i,j)の画素値"s"取得
                val = hsvImage.at<cv::Vec3b>(j, i);
                // ルックアップテーブルで画素値"s"を変換
                val[0] = lookupTable[val[0]];
                val[1] = lookupTable[val[1]];
                val[2] = lookupTable[val[2]];
                // 変換後の画素値"s"を"resultImage"の座標(i,j)に格納
                hsvImage.at<cv::Vec3b>(j, i) = val;
            }
        }
        cv::cvtColor(hsvImage, resultGImage, cv::COLOR_HSV2BGR);
        //(e)ウィンドウへの画像の表示
        cv::imshow("Frame", frameImage);
        cv::imshow("HSV", hsvImage);
        cv::imshow("Result", resultGImage);

        //(f)動画ファイル書き出し
        // cv::cvtColor(resultGImage, recImage, cv::COLOR_GRAY2BGR); // 動画用3チャンネル画像生成
        rec << resultGImage; // ビデオライタに画像出力

        //(g)キー入力待ち
        char key = cv::waitKey(20); // 20ミリ秒待機
        if (key == 'q')
            break;
    }

    return 0;
}
