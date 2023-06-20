//(OpenCV4) g++ dip08-1.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

int main (int argc, const char * argv[])
{
    //ビデオファイル"colball.mov"を取り込み
    cv::VideoCapture capture("colball.mov");
    if (capture.isOpened()==0) {
        printf("No video\n");
        return -1;
    }
    //フレームサイズ取得
    int width = capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    printf("Frame Size = (%d %d)\n", width, height);
    
    //合成用画像"face.jpg"の読み込み
    cv::Mat compImage = cv::imread("face.jpg", cv::IMREAD_COLOR);
    if (compImage.data==0) {
        printf("No image\n");
        exit(0);
    }
    printf("Image Size = (%d, %d)\n", compImage.cols, compImage.rows);

    //画像格納用インスタンス準備
    cv::Mat frameImage, hsvImage;
    cv::Mat binImage(cv::Size(width, height), CV_8UC1);;  //領域用
    
    //ウィンドウの生成と移動
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Image");
    cv::moveWindow("Image", width, 0);
    cv::namedWindow("Bin");
    cv::moveWindow("Bin", 0, height);

    //領域膨張収縮用構造要素
    cv::Mat element1 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
    cv::Mat element2 = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));

    //合成用画像の表示
    cv::imshow("Image", compImage);

    cv::Point2f original[4], translate[4];
    cv::Point2f xyContours;

        //4"binImage"から領域輪郭検出・ラベリング
    std::vector< std::vector<cv::Point> > contours; //領域輪郭群の格納用

    //ビデオライタ生成(ファイル名，コーデック(mp4v/mov)，フレームレート，フレームサイズ)
    cv::VideoWriter rec("rec.mov", cv::VideoWriter::fourcc('m','p','4','v'), 30, cv::Size(width, height));

    //動画像処理無限ループ
    while (1) {
        //カメラから1フレーム読み込み
        capture >> frameImage;
        if(frameImage.data == NULL) break;
        cv::Mat whiteImage(cv::Size(width, height), CV_8UC3, cv::Scalar(255, 255, 255));
        
        //色空間変換(BGR -> HSV)
        cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);
        
        //ボール領域画像リセット
        binImage = 0;
        int count = 0;
        //ボール領域抽出
        for (int y=0; y<height; y++) {
            for (int x=0; x<width; x++) {
                //"hsvImage"の画素(x,y)の画素値s取得
                cv::Vec3b s = hsvImage.at<cv::Vec3b>(y, x);
                
                //HSVの値を用いてボール抽出
                if (s[0]>90 && s[0]<110 && s[1]>120 && s[2]>64) {  //Blue
                    binImage.at<unsigned char>(y, x) = 255;
                    if (count == 300){
                        translate[0] = cv::Point2d(x, y); //A'(変換後左上) 
                    }
                    count++;
                }
                else if (s[0]>60 && s[0]<80 && s[1]>120 && s[2]>64) {  //Green
                    binImage.at<unsigned char>(y, x) = 255;
                    if (count == 200){
                        translate[1] = cv::Point2d(x, y); //A'(変換後左上) 
                    }
                    count++;
                }
                else if (((s[0]>0 && s[0]<5) || (s[0]>160 && s[0]<180)) && s[1]>110 && s[2]>64) {  //Red
                    binImage.at<unsigned char>(y, x) = 255;
                    translate[2] = cv::Point2d(x, y);
                    count++;
                }
                else if (s[0]>20 && s[0]<30 && s[1]>160 && s[2]>64) {  //Yellow
                    binImage.at<unsigned char>(y, x) = 255;
                    translate[3] = cv::Point2d(x, y);
                    count++;
                }
            }
        }
        printf("%d\n", count);
        count= 0;
        
        //収縮膨張による各ボール領域のノイズ・穴除去
        cv::erode(binImage, binImage, element1, cv::Point(-1,-1), 1);  //収縮
        cv::dilate(binImage, binImage, element2, cv::Point(-1,-1), 2);  //膨張
        


        cv::findContours(binImage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
        printf("%lu\n", contours.size());

        original[0] = cv::Point2f(0, 0); //A(オリジナル左上)
        original[1] = cv::Point2f(frameImage.cols, 0); //B(オリジナル右上)
        original[2] = cv::Point2f(frameImage.cols, frameImage.rows); //C(オリジナル右下)
        original[3] = cv::Point2f(0, frameImage.rows); //D(オリジナル左下)
        for (int i = 0; i < contours.size(); i++){
            xyContours = contours[i][0] + contours[i][contours[0].size()/2]/2;
            // printf("%d %f %f\n",i , xyContours.y, xyContours.x);
            // translate[i] = cv::Point2f(xyContours.y, xyContours.x); //A'(変換後左上) 
        }

        cv::Mat persMat = cv::getPerspectiveTransform(original, translate); //行列生成
        cv::warpPerspective(compImage , whiteImage, persMat, frameImage.size(),cv::INTER_LINEAR, cv::BORDER_TRANSPARENT, cv::Scalar(0,0,0));

        cv::Vec3b white;
        cv::Vec3b frame;
        for (int j = 0; j < whiteImage.rows; j++){
            for (int i = 0; i < whiteImage.cols; i++){
                white = whiteImage.at<cv::Vec3b>(j, i);
                frame = frameImage.at<cv::Vec3b>(j, i);
                if (white[0] == 255 && white[1] == 255 && white[2] == 255){
                    white[0] = frame[0];
                    white[1] = frame[1];
                    white[2] = frame[2];
                }else{
                    white[0] = frame[0]/2 + white[0]/2;
                    white[1] = frame[1]/2 + white[1]/2;
                    white[2] = frame[2]/2 + white[2]/2;
                }
                whiteImage.at<cv::Vec3b>(j, i) = white;
            }
        }

        //フレーム画像および領域画像の表示
        cv::imshow("Frame", frameImage);
        cv::imshow("Bin", whiteImage);

        //キー入力待ち
        char key = cv::waitKey(20);  //20ミリ秒待機
        if(key == 'q') break;
        
        //動画ファイル書き出し
        rec << frameImage;  //ビデオライタに画像出力
    }
    
    //メッセージを出力して終了
    printf("Finished\n");
    return 0;
}
