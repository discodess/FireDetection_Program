#include <cpprest/ws_client.h>
#include <iostream>
#include <functional>
#include <opencv2/opencv.hpp>
#define CVUI_IMPLEMENTATION
#include "cvui.h"

using namespace std;
using namespace web;
using namespace web::websockets::client;
using namespace cv;
String msg;
void message_handler(websocket_client& client) {
	client.receive().then([&client](websocket_incoming_message msg) {
		return msg.extract_string();
		}).then([&client](string body) {
			wcout << U("Received message: ") << body.c_str() << endl;
			
			msg = body;

			// 메시지를 처리한 후 다시 메시지 핸들러를 호출하여 다음 메시지를 대기
			message_handler(client);
			}).then([](pplx::task<void> t) {
				try {
					t.get(); // 예외 처리
				}
				catch (const websocket_exception& e) {
					wcout << U("Websocket exception: ") << e.what() << endl;
				}
				});
}
void func_Lyn(Mat img_frame) {
	static Mat prev_frame;
	Mat img_hsv;
	cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);

	if (!prev_frame.empty())
	{
		Mat diff_frame;

		//이전 프레임과 현재 프레임 간의 절대 차이 계산
		absdiff(prev_frame, img_frame, diff_frame);

		//차이 이미지를 그레이스케일로 변환
		cvtColor(diff_frame, diff_frame, COLOR_BGR2GRAY);

		//이진화
		threshold(diff_frame, diff_frame, 30, 255, THRESH_BINARY);

		//움직임이 있는 픽셀 수 계산
		int num_motion_pixels = countNonZero(diff_frame);

		//움직임이 있는지 확인
		if (num_motion_pixels > 10000) // 적절한 임계값을 설정
		{
			//객체 식별 코드 추가
			//색상 범위 설정
			Mat img_mask_red, img_mask_red1, img_mask_orange, img_mask_yellow;
			Vec3b lower_red = Vec3b(0, 70, 50);
			Vec3b upper_red = Vec3b(10, 255, 255);
			Vec3b lower_red1 = Vec3b(170, 70, 50);
			Vec3b upper_red1 = Vec3b(180, 255, 255);
			Vec3b lower_orange = Vec3b(10, 100, 100);
			Vec3b upper_orange = Vec3b(25, 255, 255);
			Vec3b lower_yellow = Vec3b(20, 100, 100);
			Vec3b upper_yellow = Vec3b(30, 255, 255);

			//해당 색상의 객체를 감지
			//inrange: 색상 범위에 해당하면 1값을, 그 외에는 0값을 가지는 각 색상에 대한 이진마스크 이미지 생성
			inRange(img_hsv, lower_red, upper_red, img_mask_red);
			inRange(img_hsv, lower_red1, upper_red1, img_mask_red1);
			inRange(img_hsv, lower_orange, upper_orange, img_mask_orange);
			inRange(img_hsv, lower_yellow, upper_yellow, img_mask_yellow);

			//움직임이 있는 객체만을 식별
			//bitwise_or: 마스크를 합침
			//두 이진 이미지를 입력받아 둘중 하나라도 1이면 결과 이미지의 해당 위치를 1로 설정
			Mat img_mask_motion;
			bitwise_or(img_mask_red, img_mask_red1, img_mask_motion);
			bitwise_or(img_mask_motion, img_mask_orange, img_mask_motion);
			bitwise_or(img_mask_motion, img_mask_yellow, img_mask_motion);

			//객체의 ROI를 찾음
			Mat labels, stats, centroids;
			int nlabels = connectedComponentsWithStats(img_mask_motion, labels, stats, centroids);

			for (int i = 0; i < nlabels; i++)
			{
				if (i < 1)    continue;

				int area = stats.at<int>(i, CC_STAT_AREA);
				int left = stats.at<int>(i, CC_STAT_LEFT);
				int top = stats.at<int>(i, CC_STAT_TOP);
				int width = stats.at<int>(i, CC_STAT_WIDTH);
				int height = stats.at<int>(i, CC_STAT_HEIGHT);

				if (area > 10000) {
					rectangle(img_frame, Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 3);
					putText(img_frame, "fire", Point(left + 20, top + 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 3);
				}
			}
		}

	}
	// 현재 프레임을 이전 프레임으로 설정
	prev_frame = img_frame.clone();

}
void func_Joon(Mat img_frame) {
	Ptr<BackgroundSubtractorMOG2> foregroundBackground = createBackgroundSubtractorMOG2(500, 350, false);
	GaussianBlur(img_frame, img_frame, Size(5, 5), 0);

	Mat img_mask;
	foregroundBackground->apply(img_frame, img_mask, 0);

	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
	morphologyEx(img_mask, img_mask, MORPH_CLOSE, kernel);

	vector<vector<Point>> contours;
	findContours(img_mask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// 물체 외곽에 빨간색 선을 그림
	//drawContours(img_frame, contours, -1, Scalar(0, 0, 255), 2);

	// BGR에서 HSV로 변환
	Mat img_hsv;
	cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);

	// 빨간색의 HSV 색상 범위 정의
	Scalar lower_red1 = Scalar(10, 120, 70); // 색상, 채도, 명도 
	Scalar upper_red1 = Scalar(20, 255, 255);
	Scalar lower_red2 = Scalar(170, 120, 70);
	Scalar upper_red2 = Scalar(180, 255, 255);

	// 빨간색 범위에 해당하는 마스크 생성
	Mat mask1, mask2, red_mask;
	inRange(img_hsv, lower_red1, upper_red1, mask1);
	inRange(img_hsv, lower_red2, upper_red2, mask2);
	red_mask = mask1 | mask2; // 두개의 마스크를 비트단위 OR 연산

	// 빨간색 픽셀의 수 계산
	int red_count = countNonZero(red_mask);

	// 빨간색 픽셀이 충분한 경우 "RED" 표시
	if (red_count > 500) { // 500은 임의의 값으로, 필요에 따라 조정하세요.
		putText(img_frame, "RED", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 0, 255), 3);
	}

}
void func_guna1(Mat img_frame)
{
	static CascadeClassifier fire_cascade;
	// 화재 감지기 XML 파일 로드
	if (fire_cascade.empty())
	{
		if (!fire_cascade.load("guna1.xml"))
		{
			cerr << "Error: 화재 감지기 XML 파일을 불러올 수 없습니다." << endl;
			return;
		}
	}

	// 입력 이미지를 그레이스케일로 변환
	Mat frame_gray;
	cvtColor(img_frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	// 화재 감지
	vector<Rect> fires;
	fire_cascade.detectMultiScale(frame_gray, fires, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

	// 감지된 화재에 사각형 그리기
	for (size_t i = 0; i < fires.size(); i++) {
		Point pt1(fires[i].x, fires[i].y);
		Point pt2(fires[i].x + fires[i].width, fires[i].y + fires[i].height);
	}

	// 화면에 프레임 표시
	/*imshow("xml", img_frame);*/
}

//void func_guna(Mat img_frame)
//{
//	Ptr<BackgroundSubtractorMOG2> foregroundBackground = createBackgroundSubtractorMOG2(500, 350, false);
//	GaussianBlur(img_frame, img_frame, Size(5, 5), 0);
//
//	Mat img_mask;
//	foregroundBackground->apply(img_frame, img_mask, 0);
//
//	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
//	morphologyEx(img_mask, img_mask, MORPH_CLOSE, kernel);
//
//	vector<vector<Point>> contours;
//	findContours(img_mask.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//
//	Mat img_hsv;
//	cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
//
//	// 라이터 불꽃의 확장된 색상 범위 정의
//	Scalar lower_flame1 = Scalar(0, 100, 100); // 극단적인 빨강
//	Scalar upper_flame1 = Scalar(10, 255, 255);
//	Scalar lower_flame2 = Scalar(10, 100, 100); // 황색에서 주황색
//	Scalar upper_flame2 = Scalar(25, 255, 255);
//	Scalar lower_flame3 = Scalar(170, 100, 100); // 극단적인 빨강
//	Scalar upper_flame3 = Scalar(180, 255, 255);
//
//	// 세 가지 범위에 해당하는 마스크 생성
//	Mat flame_mask1, flame_mask2, flame_mask3, flame_mask;
//	inRange(img_hsv, lower_flame1, upper_flame1, flame_mask1);
//	inRange(img_hsv, lower_flame2, upper_flame2, flame_mask2);
//	inRange(img_hsv, lower_flame3, upper_flame3, flame_mask3);
//
//	// 세 개의 마스크를 비트단위 OR 연산으로 합침
//	flame_mask = flame_mask1 | flame_mask2 | flame_mask3;
//
//	// 라이터 불꽃 픽셀의 수 계산
//	int flame_count = countNonZero(flame_mask);
//
//	// 라이터 불꽃 픽셀이 충분한 경우 "FLAME" 표시
//	if (flame_count > 100)
//	{ // 100은 임의의 값으로, 필요에 따라 조정하세요.
//		putText(img_frame, "FLAME", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 0, 255), 3);
//		detected_fires.push_back(Rect(0, 0, 100, 100)); // 화재로 판단된 영역을 저장
//	}
//
//	/*  imshow("frame1", img_frame);
//	  imshow("mask1", img_mask);
//	  imshow("flame mask1", flame_mask);*/
//}

enum buttonState {
	none, func1, func2, func3, func4
};

int func_opencv() {
	cv::Mat frame = cv::Mat(480, 860, CV_8UC3);
	cv::Mat img_frame;
	VideoCapture cap(0);
	buttonState bs = none;
	cvui::init("Fire Detect");
	while (1) {
		frame = cv::Scalar(255, 252, 249);
		cap >> img_frame;

		if (cvui::button(frame, 650, 40, 200, 60, "OpenCV")) {
			bs = func1;
		}
		if (cvui::button(frame, 650, 140, 200, 60, "OpenCV+Sensor")) {
			bs = func2;
		}
		if (cvui::button(frame, 650, 240, 200, 60, "OpenCV+Sensor+XML")) {
			bs = func3;
		}
		if (cvui::button(frame, 650, 340, 200, 60, "Exit")) {
			destroyAllWindows();
			bs = func4;
			break;
		}

		if (bs == func1) {
			func_Joon(img_frame);
			func_Lyn(img_frame);
		}
		else if (bs == func2) {
			if (msg =="true") {
				func_Joon(img_frame);
				func_Lyn(img_frame);
			}
		}
		else if (bs == func3) {
			if (msg =="true") {
				func_Joon(img_frame);
				func_Lyn(img_frame);
				func_guna1(img_frame);				
			}
		}
		img_frame.copyTo(frame(cv::Rect(0, 0, img_frame.cols, img_frame.rows)));
		cvui::update();
		cv::imshow("Fire Detect", frame);
		waitKey(1);
	}
	cap.release();
	return 0;
}

int main() {
	wcout.imbue(locale("korean"));
	websocket_client client;

	try {
		client.connect(U("ws://192.168.0.205:8080/ws")).wait();
		cout << "서버 연결 완료" << endl;

		websocket_outgoing_message msg;
		msg.set_utf8_message("hello");
		client.send(msg).wait();
		cout << "메시지 전송 완료" << endl;

		message_handler(client);
		func_opencv();

	}
	catch (const websocket_exception& e) {
		wcout << U("Websocket exception: ") << e.what() << endl;
	}
	catch (const exception& e) {
		cout << "Standard exception: " << e.what() << endl;
	}
	catch (...) {
		cout << "Unknown exception occurred" << endl;
	}

	// 클라이언트 종료 전에 연결을 닫습니다.
	client.close().wait();
	return 0;
}
