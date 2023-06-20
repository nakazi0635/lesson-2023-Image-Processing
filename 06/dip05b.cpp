//(OpenCV4) g++ dip06b.cpp -std=c++11 `pkg-config --cflags --libs opencv4`
#include <iostream>  //入出力関連ヘッダ
#include <opencv2/opencv.hpp>  //OpenCV関連ヘッダ

int main (int argc, const char * argv[])
{
	//ビデオファイル"movie.mov"を取り込み
    cv::VideoCapture capture("movie.mov");  //指定したビデオファイルをオープン
    //ビデオファイルがオープンできたかどうかをチェック
    if (capture.isOpened()==0) {
        printf("Specified video not found\n");
        return -1;
    }
    
    //フレームの大きさを取得
    int width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);
    printf("FRAME SIZE = (%d %d)\n", width, height);
		
    //画像格納用インスタンス準備
    cv::Mat frameImage;
    cv::Mat hsvImage;
    cv::Mat resultImage;
    cv::Mat grayImage(frameImage.size(), CV_8UC1);  //1チャンネル
    cv::Mat binImage(frameImage.size(), CV_8UC1);  //1チャンネル
    cv::Mat contourImage(frameImage.size(), CV_8UC3);  //3チャンネル
    cv::Mat whiteImage(height, width, CV_8UC3, cv::Scalar(20, 20, 20));  //3チャンネル
    cv::Mat hsvwhiteImage(height, width, CV_8UC3, cv::Scalar(20, 20, 20));  //3チャンネル
    
    
    //ウィンドウの生成と移動
    cv::namedWindow("Frame");
    cv::moveWindow("Frame", 0, 0);
    cv::namedWindow("Result");
    cv::moveWindow("Result", width, 0);
    int color = 0;
    
    //ビデオライタ生成(ファイル名，コーデック，フレームレート，フレームサイズ)
    cv::VideoWriter rec("rec.mpg", cv::VideoWriter::fourcc('P','I','M','1'), 30, cv::Size(width, height));
		
	//動画像処理無限ループ：「ビデオキャプチャから1フレーム取り込み」→「画像処理」→「表示」の繰り返し
	while (1) {
        //カメラから1フレーム読み込み（ストリーム入力）
        capture >> frameImage;
        if(frameImage.data == NULL) break;
		
		//色空間変換(BGR -> HSV)
        cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);
		
		//色の抽出
		for (int y=0; y<frameImage.rows; y++) {
			for (int x=0; x<frameImage.cols; x++) {
                cv::Vec3b s = hsvImage.at<cv::Vec3b>(y, x);
				//色相(H)と彩度(S)の値を用いてボール抽出
				if (s[0]>60 && s[0]<80 && s[1]>100) {
                    s[0] = 60; s[1] = 180; s[2] = 150;
				}
				else {
					s[0] = 0; s[1] = 0; s[2] = 0;
				}
                hsvImage.at<cv::Vec3b>(y, x) = s;
			}
		}
		
		//色空間変換(HSV -> BGR)
        cv::cvtColor(hsvImage, resultImage, cv::COLOR_HSV2BGR);
        frameImage.copyTo(contourImage);
        //"sourceImage"をグレースケール画像に変換して"grayImage"に出力
        cv::cvtColor(resultImage, grayImage, cv::COLOR_BGR2GRAY);
        //"grayImage"を2値化して"binImage"に出力
        cv::threshold(grayImage, binImage, 20, 255, cv::THRESH_BINARY);
        cv::erode(binImage, binImage, cv::Mat(), cv::Point(-1, -1), 3);
        cv::dilate(binImage, binImage, cv::Mat(), cv::Point(-1, -1), 4);
        //4"binImage"から領域輪郭検出・ラベリング
        std::vector< std::vector<cv::Point> > contours; //領域輪郭群の格納用
        //全領域を検出して各領域輪郭を"contours"に格納
        cv::findContours(binImage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
            //"contourImage"に領域群"contours"の i 番目の領域の輪郭を描画(白色，線幅 2，8 近傍)
            // cv::drawContours(resultImage, contours, i, cv::Scalar(255,255,255), 2, 8); 
        cv::cvtColor(contourImage, hsvImage, cv::COLOR_BGR2HSV);
        cv::circle(hsvImage, (contours[0][0] + contours[0][contours[0].size()/2])/2, 6, cv::Scalar(color,255,250), -1);
        color += 2;
        if (color > 180){
            color = 0;
        }
        printf("circular = %d\n\n", color);
        cv::cvtColor(whiteImage, hsvwhiteImage, cv::COLOR_BGR2HSV);
        for (int y=0; y<frameImage.rows; y++) {
			for (int x=0; x<frameImage.cols; x++) {
                cv::Vec3b s = hsvImage.at<cv::Vec3b>(y, x);
                cv::Vec3b w = hsvwhiteImage.at<cv::Vec3b>(y, x);
				//色相(H)と彩度(S)の値を用いてボール抽出
				if (s[1]>254 && s[2]> 100) {
                    w[0] = s[0]; w[1] = s[1]; w[2] = s[2];
				}
				else {
					s[0] = 0; s[1] = 0; s[2] = 0;
				}
                hsvImage.at<cv::Vec3b>(y, x) = s;
                hsvwhiteImage.at<cv::Vec3b>(y, x) = w;
			}
		}
        cv::cvtColor(hsvwhiteImage, whiteImage, cv::COLOR_HSV2BGR);
        cv::cvtColor(frameImage, hsvImage, cv::COLOR_BGR2HSV);
        for (int y=0; y<frameImage.rows; y++) {
			for (int x=0; x<frameImage.cols; x++) {
                cv::Vec3b s = hsvwhiteImage.at<cv::Vec3b>(y, x);
                cv::Vec3b w = hsvImage.at<cv::Vec3b>(y, x);
				//色相(H)と彩度(S)の値を用いてボール抽出
				if (s[1]>254 && s[2]> 100) {
                    w[0] = s[0]; w[1] = s[1]; w[2] = s[2];
				}
                hsvImage.at<cv::Vec3b>(y, x) = w;
                // hsvwhiteImage.at<cv::Vec3b>(y, x) = w;
			}
		}
		cv::cvtColor(hsvImage, frameImage, cv::COLOR_HSV2BGR);
        cv::cvtColor(hsvwhiteImage, whiteImage, cv::COLOR_HSV2BGR);
        cv::circle(frameImage, (contours[0][0] + contours[0][contours[0].size()/2])/2, 12, cv::Scalar(255,255,255), -1);
        //(動画ファイル書き出し
        rec << frameImage;  //ビデオライタに画像出力


        //ウィンドウへの画像の表示
        cv::imshow("Frame", frameImage);
        cv::imshow("Bin", hsvwhiteImage);
        cv::imshow("Result", hsvImage);
                
        //キー入力待ち
        char key = cv::waitKey(33);  //30ミリ秒待機
        if(key == 'q') break;
	}
	
	//メッセージを出力して終了
	printf("Finished\n");
	return 0;
}
