/*
    g++ -std=c++11 dip11.cpp `pkg-config --cflags --libs opencv4`
 */
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

// 顔色の変更
void change_face_color(cv::Mat& faceImage, cv::Mat& hsvImage, cv::Rect rect)
{
    // 色解析しやすいようにHSV色空間に変換
    cv::cvtColor(faceImage, hsvImage, cv::COLOR_BGR2HSV);

    for(int j=rect.y; j<rect.y+rect.height; j++) {
		if(j<0 || j>= hsvImage.rows) continue;
        for(int i=rect.x; i<rect.x+rect.width; i++) {
			// if(i<0 || i>= hsvImage.cols) continue;
            cv::Vec3b s = hsvImage.at<cv::Vec3b>(j, i);
			// hsvImage.at<cv::Vec3b>(j, 2*rect.x+rect.width-i) = s;
            // 肌色領域のみ変換
            if(!(s[0]> 0 && s[0]< 45 && s[1]>50 && s[1]<255 && s[2]>50 && s[2]<255))
            {
                s[1] = 0;
                s[2] = 0;
                hsvImage.at<cv::Vec3b>(j, i) = s;
            }
        }
    }
    cv::cvtColor(hsvImage, faceImage, cv::COLOR_HSV2BGR);
}

//main関数
int main(int argc, char* argv[])
{
    //OpenCV初期設定処理
    //カメラキャプチャの初期化
    cv::VideoCapture capture = cv::VideoCapture(0);
    if (capture.isOpened()==0) {
        //カメラが見つからないときはメッセージを表示して終了
        printf("Camera not found\n");
        exit(1);
    }

    cv::Mat sourceImg = cv::imread("うさミミ.jpg");
    if (sourceImg.data==0) { //ファイルが見つからないときはメッセージを表示して終了
        printf("Source not found\n");
        exit(0);
    }
    cv::Mat source2Image = cv::Mat(sourceImg.size(), CV_8UC3);

    cv::Mat originalImage, frameImage, hsvImage, tempImage;
    cv::Size imageSize(720, 405);  // 画像サイズ
    cv::Size imageSize2(269, 171);  // 画像サイズ
    cv::Point2f test;
    double scale_image = 1.0;
    cv::CascadeClassifier faceClassifier;  // 顔認識用分類器
    cv::CascadeClassifier eyeClassifier; // 眼認識用分類器
    cv::Point2f earPoint(130, 190);
    

    //3チャンネル画像"hsvImage"と"tempImage"の確保（ビデオと同サイズ）
    hsvImage = cv::Mat(imageSize, CV_8UC3);
    tempImage = cv::Mat(imageSize, CV_8UC3);

    //OpenCVウィンドウ生成
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Face");
    cv::moveWindow("Face", imageSize.width, 0);

    // ①正面顔検出器の読み込み
    faceClassifier.load("haarcascades/haarcascade_frontalface_default.xml");
    eyeClassifier.load("haarcascades/haarcascade_mcs_eyepair_small.xml");
    
    cv::Point2f center = cv::Point2f(100.0, 50.0); //回転中心
    double angle = 0.0; //回転角度
    double scale = 1.0; //拡大率
    cv::Mat rotateMat = cv::getRotationMatrix2D(center, angle, scale);
    cv::Vec3b white, frame;
    // for (int j = 0; j < sourceImg.rows; j++){
    //     for (int i = 0; i < sourceImg.cols; i++){
    //         white = sourceImg.at<cv::Vec3b>(j, i);
    //         printf("white Size = (%d %d %d)\n", white[0], white[1], white[2]);
    //     }
    // }
    cv::Point2f original[4], translate[4];
    cv::Rect face;
    
    while(1){
        //ビデオキャプチャから1フレーム画像取得
        capture >> originalImage;
        cv::resize(originalImage, frameImage, imageSize);
        cv::Mat whiteImage(imageSize, CV_8UC3, cv::Scalar(255, 255, 255));


        // ②検出情報を受け取るための配列を用意する
        std::vector<cv::Rect> faces, eyes;

        // ③画像中から検出対象の情報を取得する
        faceClassifier.detectMultiScale(frameImage, faces, 1.1, 3, 0, cv::Size(20,20));
        eyeClassifier.detectMultiScale(frameImage, eyes, 1.1, 3, 0, cv::Size(10,10));

        // cv::warpAffine(sourceImg, whiteImage, rotateMat, imageSize,
        // cv::INTER_LINEAR, cv::BORDER_TRANSPARENT, cv::Scalar(255,255,255));

        // for (int j = 0; j < whiteImage.rows; j++){
        //     for (int i = 0; i < whiteImage.cols; i++){
        //         white = whiteImage.at<cv::Vec3b>(j, i);
        //         frame = frameImage.at<cv::Vec3b>(j, i);
        //         if (white[0] == 255 && white[1] == 255 && white[2] == 255 || white[0] < 30 && white[1] >= 200 && white[2] < 30){
        //             white[0] = frame[0];
        //             white[1] = frame[1];
        //             white[2] = frame[2];
        //         }else{
        //             // printf("%d %d %d\n", white[0], white[1], white[2]);
        //         }
        //         whiteImage.at<cv::Vec3b>(j, i) = white;
        //     }
        // }

        // ④顔領域の検出
        cv::circle(whiteImage, earPoint, 10, cv::Scalar(0,255,0), -1, 8);
        for (int i = 0; i < faces.size(); i++) {
            // 検出情報から顔の位置情報を取得
            face = faces[i];
            // 大きさによるチェック。
            if(face.width*face.height < 100*100){
                continue; // 小さい矩形は採用しない
            }
            
            // ⑤画像の加工
            // change_face_color(frameImage, hsvImage, face);
            
            //取得した顔の位置情報に基づき、矩形描画を行う
            // cv::rectangle(whiteImage,
            //     cv::Point(face.x, face.y),
            //     // cv::Point(face.x + face.width, face.y + face.height),
            //     cv::Point(face.x + face.width, face.y + face.height),
            //     CV_RGB(255, 0, 0),
            //     3, cv::LINE_AA);
            }

        original[0] = cv::Point2f(0, 0); //A(オリジナル左上)
        original[1] = cv::Point2f(0, frameImage.rows); //B(オリジナル右上)
        original[2] = cv::Point2f(frameImage.cols, frameImage.rows); //C(オリジナル右下)
        original[3] = cv::Point2f(frameImage.cols, 0); //D(オリジナル左下)

        // printf("%d, %d\n", shipImage.cols, shipImage.rows);
        translate[0] = cv::Point2d(earPoint.x - 135, earPoint.y - 85);
        translate[1] = cv::Point2d(earPoint.x - 135, earPoint.y + 85); 
        translate[2] = cv::Point2d(earPoint.x + 135, earPoint.y + 85); 
        translate[3] = cv::Point2d(earPoint.x + 135, earPoint.y - 85); 
        earPoint.x = face.x + 150;
        earPoint.y = face.y - 50;
        scale_image = face.width * 0.01;
        test.x = 269 * scale_image;
        test.y = 171 * scale_image;
        cv::resize(sourceImg, source2Image, cv::Size(test.x, test.y));


        cv::Mat persMat = cv::getPerspectiveTransform(original, translate); //行列生成
        cv::warpPerspective(source2Image, whiteImage, persMat, whiteImage.size(),cv::INTER_LINEAR, cv::BORDER_TRANSPARENT, cv::Scalar(0,0,0));
        for (int j = 0; j < whiteImage.rows; j++){
            for (int i = 0; i < whiteImage.cols; i++){
                white = whiteImage.at<cv::Vec3b>(j, i);
                frame = frameImage.at<cv::Vec3b>(j, i);
                if (white[0] >= 250 && white[1] >= 250 && white[2] >= 250 || white[0] < 30 && white[1] >= 200 && white[2] < 30){
                    white[0] = frame[0];
                    white[1] = frame[1];
                    white[2] = frame[2];
                }else{
                    // printf("%d %d %d\n", white[0], white[1], white[2]);
                }
                whiteImage.at<cv::Vec3b>(j, i) = white;
            }
        }
        
        //認識結果画像表示
        cv::imshow("face", frameImage);
        cv::imshow("image", source2Image);
        cv::imshow("Frame2", whiteImage);
            
        char key = cv::waitKey(10);
        if(key == 'q'){
            break;
        }
    }
    
    return 0;
}
