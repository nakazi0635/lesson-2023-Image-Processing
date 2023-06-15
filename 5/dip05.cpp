//(OpenCV4) g++ -std=c++11 dip05.cpp `pkg-config --cflags --libs opencv4`
//(OpenCV3) g++ dip05.cpp `pkg-config --cflags --libs opencv`
#include <iostream>           //入出力関連ヘッダ
#include <opencv2/opencv.hpp> //OpenCV関連ヘッダ

// アニメ調色変換関数の宣言
void myAnimeColor(const cv::Mat &src, cv::Mat &dst);
unsigned char lookupTable[3][256];

int main(int argc, char *argv[])
{
    // アニメ変換用ルックアップテーブル
    for (int i = 0; i < 256; i++)
    {
        // H
        lookupTable[0][i] = i;
        // S
        lookupTable[1][i] = i;
        // V
        if (i < 64)
            lookupTable[2][i] = 0;
        else if (i < 128)
            lookupTable[2][i] = 85;
        else if (i < 196)
            lookupTable[2][i] = 170;
        else
            lookupTable[2][i] = 255;
    }

    // ①ビデオキャプチャを初期化して，映像を取り込む
    //    cv::VideoCapture capture(0);  //内臓カメラをオープン
    cv::VideoCapture capture("scene.mov"); // 指定したビデオファイルをオープン
    // ビデオファイルがオープンできたかどうかをチェック
    if (capture.isOpened() == 0)
    {
        printf("Camera not found\n");
        return -1;
    }

    // ②画像格納用インスタンス準備
    int width = 720, height = 405;
    cv::Mat frameImage;
    cv::Mat originalImage(cv::Size(width, height), CV_8UC3);
    cv::Mat originalImage2(cv::Size(width, height), CV_8UC3);
    cv::Mat grayImage(cv::Size(width, height), CV_8UC1);
    cv::Mat medianImage(cv::Size(width, height), CV_8UC1);
    cv::Mat resultImage(cv::Size(width, height), CV_8UC1);
    cv::Mat recImage(cv::Size(width, height), CV_8UC3);

    // ③画像表示用ウィンドウの生成
    cv::namedWindow("Original");
    cv::moveWindow("Original", 0, 50);
    cv::namedWindow("Gray");
    cv::moveWindow("Gray", 0, 200);
    cv::namedWindow("Result");
    cv::moveWindow("Result", 200, 200);

    // ④線形空間フィルタの生成
    // 線形空間フィルタの係数を要素とする1次元配列の作成(5x5の平均値フィルタ用)
    float fdata[] = {
        1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
        1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
        1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
        1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0,
        1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0, 1.0 / 25.0};
    float fdata2[] = {-1.0, 0.0, 1.0,
                    -2.0, 0.0, 2.0,
                    -1.0, 0.0, 1.0};
    // 一次元配列の要素に基づき，線形空間フィルタをCvMat型の5x5行列"kernel"として生成
    cv::Mat kernel(cv::Size(5, 5), CV_32F, fdata);
    cv::Mat kernel2(cv::Size(3, 3), CV_32F, fdata2);

    // ⑤ビデオライタ生成(ファイル名，コーデック，フレームレート，フレームサイズ)
    cv::VideoWriter rec("rec.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 30, recImage.size());

    // ⑤動画像処理無限ループ：「ビデオキャプチャから1フレーム取り込み」→「画像処理」→「表示」の繰り返し
    while (1)
    {
        //(a)ビデオキャプチャ"capture"から1フレームを取り込んで，"frameImage"に格納
        capture >> frameImage;
        // ビデオが終了したら無限ループから脱出
        if (frameImage.data == NULL)
            break;

        //(b)"frameImage"をリサイズして"originalImage"に格納
        cv::resize(frameImage, originalImage, originalImage.size());

        myAnimeColor(originalImage, originalImage2);

        //(c)"frameImage"をグレースケールに変換して"grayImage"に格納
        cv::cvtColor(originalImage, grayImage, cv::COLOR_BGR2GRAY);

        //(d)"grayImage"に線形空間フィルタ"kernel"を適用して"resultImage"を出力
        // cv::filter2D(grayImage, resultImage, grayImage.depth(), kernel);
        cv::medianBlur(grayImage, medianImage, 5);
        cv::filter2D(medianImage, grayImage, grayImage.depth(), kernel2);
        // cv::filter2D(originalImage2, resultImage, grayImage.depth(), kernel2);
        cv::threshold(grayImage, resultImage, 100, 255, cv::THRESH_BINARY);

        unsigned char k1;
        cv::Vec3b k2;
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                // ルックアップテーブルで各画素値変換
                k1 = resultImage.at<unsigned char>(j, i);
                if (k1 > 250){
                    k2[0] = 255;
                    k2[1] = 0;
                    k2[2] = 0;
                    originalImage2.at<cv::Vec3b>(j, i) = k2;
                }
            }
        }


        //(e)ウィンドウに画像表示
        cv::imshow("Original", originalImage);
        cv::imshow("Gray", originalImage2);
        cv::imshow("Result", resultImage);

        // cv::cvtColor(resultImage, recImage, cv::COLOR_GRAY2BGR); // 動画用3チャンネル画像生成
        rec << originalImage2;                                          // ビデオライタに画像出力

        //(f)[q]キーが押されたら無限ループから脱出
        int key = cv::waitKey(10);
        if (key == 'q')
            break;
    }

    // ⑥メッセージを出力して終了
    printf("Finished\n");
    return 0;
}

// アニメ調色変換関数（src：入力画像，dst：出力画像）
void myAnimeColor(const cv::Mat &src, cv::Mat &dst)
{
    // 作業用画像生成
    cv::Mat cImage(src.size(), CV_8UC3); // 3チャンネル

    // 色変換
    cv::cvtColor(src, cImage, cv::COLOR_BGR2HSV); // RGB→HSV
    cv::Vec3b s;
    for (int j = 0; j < src.rows; j++)
    {
        for (int i = 0; i < src.cols; i++)
        {
            // ルックアップテーブルで各画素値変換
            s = cImage.at<cv::Vec3b>(j, i);
            s[0] = lookupTable[0][s[0]];
            s[1] = lookupTable[1][s[1]];
            s[2] = lookupTable[2][s[2]];
            cImage.at<cv::Vec3b>(j, i) = s;
        }
    }

    cv::cvtColor(cImage, dst, cv::COLOR_HSV2BGR); // HSV→RGB
}
