//(OpenCV4) g++ dip07-2.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
//(OpenCV3) g++ dip07-2.cpp `pkg-config --cflags --libs opencv`

#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ
#include <cmath>

int main (int argc, char *argv[])
{
    //ビデオキャプチャを初期化して，映像を取り込む
    cv::VideoCapture capture("water1.mov");  //指定したビデオファイルをオープン
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
    //フレームサイズ取得
    int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    printf("Frame Size = (%d %d)\n", width, height);
    cv::Size ship2Size(100, 100);
    cv::Size imageSize(width, height);  //フレームと同じ画像サイズ定義
    

    //船画像"face.jpg"の読み込み
    cv::Mat shipImage = cv::imread("ship.jpg", cv::IMREAD_COLOR);
    // cv::Mat ship2Image(shipImage.size(), CV_8UC3);
    cv::Mat ship2Image(80, 100, CV_8UC3, cv::Scalar(0, 255, 0));

    //画像格納用インスタンス準備
    cv::Mat frameImage;
    cv::Mat recImage = cv::Mat(cv::Size(width/2, height/2), CV_8UC3);
    
    //オプティカルフロー準備
    cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::MAX_ITER|cv::TermCriteria::EPS, 30, 0.01);  //終了条件
    cv::Mat presentImage(imageSize, CV_8UC1), priorImage(imageSize, CV_8UC1);  //現フレーム濃淡画像，前フレーム濃淡画像
    std::vector<cv::Point2f> presentFeature, priorFeature;  //現フレーム対応点，前フレーム追跡点
    std::vector<unsigned char> status;  //処理用
    std::vector<float> errors;  //処理用

    //船の初期位置
    cv::Point2f shipPoint(130, 190);
    cv::Point2f before_shipPoint(130, 190);

    //ウィンドウの生成と移動
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Ship");
    cv::moveWindow("Ship", width, 0);

    //船画像の表示
    
    //追跡点の設定（適当に決めた5つの点）
    // priorFeature.push_back(cv::Point2f(width/2.0-20, height/2.0));
    // priorFeature.push_back(cv::Point2f(width/2.0-10, height/2.0));
    // priorFeature.push_back(cv::Point2f(width/2.0, height/2.0));
    // priorFeature.push_back(cv::Point2f(width/2.0+10, height/2.0));
    // priorFeature.push_back(cv::Point2f(width/2.0+20, height/2.0));

    //ビデオライタ生成(ファイル名，コーデック(mp4v/mov)，フレームレート，フレームサイズ)
    cv::VideoWriter rec("rec.mov", cv::VideoWriter::fourcc('m','p','4','v'), 30, cv::Size(width, height));

    //動画像処理無限ループ
    int fid = 0;
    int count = 0;
    double angle = 0;  //回転角度
    double scale = 1.0;  //拡大率
    double degrees = 45;
    cv::Vec3b frame;
    cv::Vec3b white;
    cv::Point2f original[4], translate[4];
    cv::Point2f center = cv::Point2f(shipImage.cols/2, shipImage.rows/2);  //回転中心
    while (1) {
        //===== カメラから1フレーム読み込み =====
        capture >> frameImage;
        if(frameImage.data == NULL) break;

        cv::Mat whiteImage(frameImage.size(), CV_8UC3, cv::Scalar(255, 255, 255));
        cv::Mat white2Image(whiteImage.size(), CV_8UC3);
        for (int j = 0; j < shipImage.rows; j++){
            for (int i = 0; i < shipImage.cols; i++){
                white = shipImage.at<cv::Vec3b>(j, i);
                frame = ship2Image.at<cv::Vec3b>(j, i);
                white[0] = frame[0];
                white[1] = frame[1];
                white[2] = frame[2];
                ship2Image.at<cv::Vec3b>(j, i) = white;
            }
        }
        // printf("white Size = (%d %d)\n", white2Image.cols, white2Image.rows);

        whiteImage.copyTo(white2Image);
        
        //===== オプティカルフロー =====
        cv::cvtColor(frameImage, presentImage, cv::COLOR_BGR2GRAY);  //現フレーム濃淡画像"presentImage"を生成
        cv::goodFeaturesToTrack(priorImage, priorFeature, 1000, 0.01, 1);  //前フレーム追跡点"priorFeature"生成
        int opCnt = priorFeature.size();  //追跡点の個数
        //オプティカルフローの計算と描画
        if (opCnt>0) {  //追跡点が存在する場合
            //"priorImage"と"presentImage"を用いて，追跡点"priorFeature"に対応する現フレーム点"presentFeature"を取得
            cv::calcOpticalFlowPyrLK(priorImage, presentImage, priorFeature, presentFeature, status, errors, cv::Size(10,10), 4, criteria);
            //オプティカルフロー描画
            for(int i=0; i<opCnt; i++){
                cv::Point pt1 = cv::Point(priorFeature[i]);  //前フレーム追跡点
                cv::Point pt2 = cv::Point(presentFeature[i]);  //現フレーム対応点
                // cv::line(frameImage, pt1, pt2, cv::Scalar(0,0,255), 2, 8);  //pt1とpt2を結ぶ直線を描画
                if(pt1.x > shipPoint.x - 70 && pt1.y > shipPoint.y - 70 && pt1.x < shipPoint.x + 70 && pt1.y < shipPoint.y + 70){
                    shipPoint.x += abs(pt1.x - pt2.x);
                    shipPoint.y += abs(pt1.y - pt2.y);
                }
            }
        }
        shipPoint.y -= 0.5;
        presentImage.copyTo(priorImage);  //"priorImage"を"presentImage"で更新
        //船の位置に円を表示
        cv::circle(frameImage, shipPoint, 10, cv::Scalar(0,255,0), -1, 8);
        
        // printf("%f, %f\n", shipPoint.x, shipPoint.y);

        
        double radians = std::atan2(abs(shipPoint.y - before_shipPoint.y), abs(shipPoint.x - before_shipPoint.x));
        if (radians * (180.0 / M_PI) < 80 && radians * (180.0 / M_PI) > 10){
            degrees = radians * (180.0 / M_PI);
            degrees -= 45;
        }

        printf("%f\n", degrees);


        cv::Mat rotateMat = cv::getRotationMatrix2D(center, degrees, scale); //行列生成

        // for (int i = 0; i < rotateMat.rows; ++i) {
        //     for (int j = 0; j < rotateMat.cols; ++j) {
        //         printf("%f ", rotateMat.at<double>(i, j));
        //     }
        //     printf("\n");
        // }
        // printf("\n");



        cv::warpAffine(shipImage, ship2Image, rotateMat, ship2Image.size(),
        cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0,255,0));

        cv::imshow("Ship", ship2Image);


        original[0] = cv::Point2f(0, 0); //A(オリジナル左上)
        original[1] = cv::Point2f(0, shipImage.rows); //B(オリジナル右上)
        original[2] = cv::Point2f(shipImage.cols, shipImage.rows); //C(オリジナル右下)
        original[3] = cv::Point2f(shipImage.cols, 0); //D(オリジナル左下)

        // printf("%d, %d\n", shipImage.cols, shipImage.rows);

        translate[0] = cv::Point2d(shipPoint.x - 50, shipPoint.y - 32);
        translate[1] = cv::Point2d(shipPoint.x - 50, shipPoint.y + 32); 
        translate[2] = cv::Point2d(shipPoint.x + 50, shipPoint.y + 32); 
        translate[3] = cv::Point2d(shipPoint.x + 50, shipPoint.y - 32); 

        cv::Mat persMat = cv::getPerspectiveTransform(original, translate); //行列生成
        cv::warpPerspective(ship2Image, white2Image, persMat, white2Image.size(),cv::INTER_LINEAR, cv::BORDER_TRANSPARENT, cv::Scalar(0,0,0));

        for (int j = 0; j < white2Image.rows; j++){
            for (int i = 0; i < white2Image.cols; i++){
                white = white2Image.at<cv::Vec3b>(j, i);
                frame = frameImage.at<cv::Vec3b>(j, i);
                if (white[0] == 255 && white[1] == 255 && white[2] == 255 || white[0] < 30 && white[1] >= 200 && white[2] < 30){
                    white[0] = frame[0];
                    white[1] = frame[1];
                    white[2] = frame[2];
                }else{
                    // printf("%d %d %d\n", white[0], white[1], white[2]);
                }
                white2Image.at<cv::Vec3b>(j, i) = white;
            }
        }

        before_shipPoint.x = shipPoint.x;
        before_shipPoint.y = shipPoint.y;

        //ウィンドウに画像表示
        cv::imshow("Frame", frameImage);
        cv::imshow("white", white2Image);

        //キー入力待ち
        char key = cv::waitKey(20);  //20ミリ秒待機
        if(key == 'q') break;

        //動画ファイル書き出し
        rec << white2Image;  //ビデオライタに画像出力
    }
    
    //メッセージを出力して終了
    printf("Finished\n");
    return 0;
}

