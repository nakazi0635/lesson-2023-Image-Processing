//(OpenCV4) g++ -std=c++11 dip07a.cpp `pkg-config --cflags --libs opencv4`
//(OpenCV3) g++ dip07a.cpp `pkg-config --cflags --libs opencv`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

int main (int argc, char* argv[])
{
    //①ビデオキャプチャの初期化
    cv::VideoCapture capture("walking.mp4");  //ビデオファイルをオープン
    if (capture.isOpened()==0) {
        printf("Camera not found\n");
        return -1;
    }
    
    //②画像格納用インスタンス準備
    cv::Size imageSize(720, 405);
    cv::Mat originalImage;
    cv::Mat frameImage(imageSize, CV_8UC3);
    cv::Mat ansImage(imageSize, CV_8UC3);
    cv::Mat backImage(imageSize, CV_8UC3);
    cv::Mat subImage(imageSize, CV_8UC3);
    cv::Mat subBinImage(imageSize, CV_8UC1);
    cv::Mat resultImage(imageSize, CV_8UC3);
    cv::Mat mask = cv::Mat::zeros(ansImage.size(), CV_8UC1);
    
    //③画像表示用ウィンドウの生成
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Back");
    cv::moveWindow("Back", 50, 50);
    cv::namedWindow("Subtraction");
    cv::moveWindow("Subtraction", 100, 100);
    cv::namedWindow("Result");
    cv::moveWindow("Result", 150, 150);
    int count = 0;
    cv::VideoWriter rec("rec.mp4", cv::VideoWriter::fourcc('M', 'P', '4', 'V'), 30, frameImage.size());
    cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3,3));
    
    //④動画処理用無限ループ
    while (1) {
        frameImage.copyTo(ansImage);
        //(a)ビデオキャプチャから1フレーム"originalImage"を取り込んで，"frameImage"を生成
        capture >> originalImage;
        //ビデオが終了したら無限ループから脱出
        if (originalImage.data==NULL) break;
        //"originalImage"をリサイズして"frameImage"生成
        cv::resize(originalImage, frameImage, imageSize);
        
        //(b)"frameImage"と"backImage"との差分画像"subImage"の生成
        cv::absdiff(frameImage, backImage, subImage);
        //(b'))"subImage"
        cv::cvtColor(subImage, subBinImage, cv::COLOR_BGR2GRAY);
        cv::threshold(subBinImage, subBinImage, 30, 255, cv::THRESH_BINARY);
        //(b")"frameImage"を"subBinImage"マスク付きで"resultImage"にコピー
        resultImage = cv::Scalar(0);
        frameImage.copyTo(resultImage, subBinImage);
        // cv::dilate(subBinImage, subBinImage, element, cv::Point(-1, -1), 2);
        cv::dilate(subBinImage, subBinImage, element, cv::Point(-1, -1), 10);
        cv::erode(subBinImage, subBinImage, element, cv::Point(-1, -1), 10);
        // cv::dilate(subBinImage, subBinImage, element, cv::Point(-1, -1), 10);

        //4"binImage"から領域輪郭検出・ラベリング
        std::vector< std::vector<cv::Point> > contours; //領域輪郭群の格納用
        //全領域を検出して各領域輪郭を"contours"に格納
        cv::findContours(subBinImage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        //i 番目の領域の j 番目の頂点:contours[i][j]
        for (int i=0; i<contours.size(); i++) { //検出された領域数だけループ
        double area = cv::contourArea(contours[i]); //領域"contous[i]"の面積
            if(area > 3000 && area < 12000){
                // 領域の輪郭を囲む最小矩形を計算
                cv::Rect boundingRect = cv::boundingRect(contours[i]);

                // 矩形内の画像を切り出す
                cv::Mat regionImage = ansImage(boundingRect);

                // 切り出した画像にぼかしを適用
                cv::blur(regionImage, regionImage, cv::Size(15, 15), cv::Point(-1, -1), cv::BORDER_DEFAULT);

                // ぼかしを適用した画像を元の画像の対応する領域にコピーする
                regionImage.copyTo(ansImage(boundingRect));
                cv::Point center(contours[i][0].x, contours[i][0].y + 15);
                cv::circle(ansImage, center, 25, cv::Scalar(255,0,0), 1);
                std::cout << area << std::endl;
            }
        }

        count = 0;
        //(c)"frameImage"，"backImage"，"subImage"の表示
        cv::imshow("Frame", frameImage);
        cv::imshow("Back", ansImage);
        cv::imshow("Subtraction", subBinImage);
        cv::imshow("Result", resultImage);
        
        //(d)"frameImage"で"backImage"を更新
        frameImage.copyTo(backImage);

        rec << ansImage;     
        //(e)キー入力待ち
        int key = cv::waitKey(20);
        //[Q]が押されたら無限ループ脱出
        if (key=='q')
            break;
        //[C]が押されたら"frameImage"で"backImage"を更新
        if (key=='c')frameImage.copyTo(backImage);
    }
    
    //⑤終了処理
    //カメラ終了
    capture.release();
    //メッセージを出力して終了
    printf("Finished\n");
    return 0;
}
