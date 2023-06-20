// g++ dip04.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

int main(int argc, const char *argv[])
{
    int width = 640, height = 480;
    int width_re = width / 4, height_re = height / 4;

    int bayerpattern[4][4] = {
        {0, 8, 2, 10},
        {12, 4, 14, 6},
        {3, 11, 1, 9},
        {15, 7, 13, 5},
    };

    // ①カメラの初期化
    cv::VideoCapture capture(0); // カメラ0番をオープン
    // カメラがオープンできたかどうかをチェック
    if (capture.isOpened() == 0)
    {
        printf("Camera not found\n");
        return -1;
    }

    // ②画像格納用インスタンス準備
    cv::Mat captureImage;                                                   // キャプチャ用
    cv::Mat frameImage = cv::Mat(cv::Size(width / 4, height / 4), CV_8UC3); // 処理用
    cv::Mat grayImage(cv::Size(width / 4, height / 4), CV_8UC1);            // 1チャンネル
    cv::Mat resultGImage(cv::Size(width, height), CV_8UC1);                 // 1チャンネル
    cv::Mat recImage(cv::Size(width, height), CV_8UC3);                     // 3チャンネル

    // ③ウィンドウの生成と移動
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Gray");
    cv::moveWindow("Gray", width, 0);
    cv::namedWindow("Result");
    cv::moveWindow("Result", width, height);

    // ④ルックアップテーブルの作成
    unsigned char lookupTable[256];
    for (int i = 0; i < 256; i++)
    {
        lookupTable[i] = (255 - i) / 16;
        printf("%d\n", lookupTable[i]);
    }
    // ⑤ビデオライタ生成(ファイル名，コーデック，フレームレート，フレームサイズ)
    cv::VideoWriter rec("rec.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 30, recImage.size());

    int x, y, t;
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
        cv::cvtColor(frameImage, grayImage, cv::COLOR_BGR2GRAY);

        //(d)"grayImage"の各画像を走査して，ルックアップテーブルに基づいて画素値変換して"resultGImage"に格納
        for (int j = 0; j < height / 4; j++)
        {
            for (int i = 0; i < width / 4; i++)
            {
                // 座標(i,j)の画素値"s"取得
                unsigned char s = grayImage.at<unsigned char>(j, i);
                s = lookupTable[s];
                for (x = 0; x < 4; x++)
                {
                    for (y = 0; y < 4; y++)
                    {
                        if (bayerpattern[x][y] < s)
                        {
                            t = 0;
                        }
                        else
                        {
                            t = 255;
                        }
                        // 変換後の画素値"s"を"resultImage"の座標(i,j)に格納
                        resultGImage.at<unsigned char>(j * 4 + x, i * 4 + y) = t;
                    }
                }
            }
        }
        //(e)ウィンドウへの画像の表示
        cv::imshow("Frame", frameImage);
        cv::imshow("Gray", grayImage);
        cv::imshow("Result", resultGImage);

        //(f)動画ファイル書き出し
        cv::cvtColor(resultGImage, recImage, cv::COLOR_GRAY2BGR); // 動画用3チャンネル画像生成
        rec << recImage;                                          // ビデオライタに画像出力

        //(g)キー入力待ち
        char key = cv::waitKey(20); // 20ミリ秒待機
        if (key == 'q')
            break;
    }

    return 0;
}
