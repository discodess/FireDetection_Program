//#include <opencv2/opencv.hpp>
//#include <iostream>
//using namespace std;
//using namespace cv;
//
//void func_Lyn(Mat img_frame) {
//	static Mat prev_frame;
//	Mat img_hsv;
//	cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
//
//	if (!prev_frame.empty())
//	{
//		Mat diff_frame;
//
//		//이전 프레임과 현재 프레임 간의 절대 차이 계산
//		absdiff(prev_frame, img_frame, diff_frame);
//
//		//차이 이미지를 그레이스케일로 변환
//		cvtColor(diff_frame, diff_frame, COLOR_BGR2GRAY);
//
//		//이진화
//		threshold(diff_frame, diff_frame, 30, 255, THRESH_BINARY);
//
//		//움직임이 있는 픽셀 수 계산
//		int num_motion_pixels = countNonZero(diff_frame);
//
//		//움직임이 있는지 확인
//		if (num_motion_pixels > 10000) // 적절한 임계값을 설정
//		{
//			//객체 식별 코드 추가
//			//색상 범위 설정
//			Mat img_mask_red, img_mask_red1, img_mask_orange, img_mask_yellow;
//			Vec3b lower_red = Vec3b(0, 70, 50);
//			Vec3b upper_red = Vec3b(10, 255, 255);
//			Vec3b lower_red1 = Vec3b(170, 70, 50);
//			Vec3b upper_red1 = Vec3b(180, 255, 255);
//			Vec3b lower_orange = Vec3b(10, 100, 100);
//			Vec3b upper_orange = Vec3b(25, 255, 255);
//			Vec3b lower_yellow = Vec3b(20, 100, 100);
//			Vec3b upper_yellow = Vec3b(30, 255, 255);
//
//			//해당 색상의 객체를 감지
//			//inrange: 색상 범위에 해당하면 1값을, 그 외에는 0값을 가지는 각 색상에 대한 이진마스크 이미지 생성
//			inRange(img_hsv, lower_red, upper_red, img_mask_red);
//			inRange(img_hsv, lower_red1, upper_red1, img_mask_red1);
//			inRange(img_hsv, lower_orange, upper_orange, img_mask_orange);
//			inRange(img_hsv, lower_yellow, upper_yellow, img_mask_yellow);
//
//			//움직임이 있는 객체만을 식별
//			//bitwise_or: 마스크를 합침
//			//두 이진 이미지를 입력받아 둘중 하나라도 1이면 결과 이미지의 해당 위치를 1로 설정
//			Mat img_mask_motion;
//			bitwise_or(img_mask_red, img_mask_red1, img_mask_motion);
//			bitwise_or(img_mask_motion, img_mask_orange, img_mask_motion);
//			bitwise_or(img_mask_motion, img_mask_yellow, img_mask_motion);
//
//			//객체의 ROI를 찾음
//			Mat labels, stats, centroids;
//			int nlabels = connectedComponentsWithStats(img_mask_motion, labels, stats, centroids);
//
//			for (int i = 0; i < nlabels; i++)
//			{
//				if (i < 1)    continue;
//
//				int area = stats.at<int>(i, CC_STAT_AREA);
//				int left = stats.at<int>(i, CC_STAT_LEFT);
//				int top = stats.at<int>(i, CC_STAT_TOP);
//				int width = stats.at<int>(i, CC_STAT_WIDTH);
//				int height = stats.at<int>(i, CC_STAT_HEIGHT);
//
//				if (area > 10000) {
//					rectangle(img_frame, Point(left, top), Point(left + width, top + height), Scalar(0, 0, 255), 3);
//					putText(img_frame, "fire", Point(left + 20, top + 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 3);
//				}
//			}
//		}
//
//	}
//	// 현재 프레임을 이전 프레임으로 설정
//	prev_frame = img_frame.clone();
//
//	imshow("Lyn", img_frame);
//
//}
//void func_Joon(Mat img_frame) {
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
//	// 물체 외곽에 빨간색 선을 그림
//	//drawContours(img_frame, contours, -1, Scalar(0, 0, 255), 2);
//
//	// BGR에서 HSV로 변환
//	Mat img_hsv;
//	cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
//
//	// 빨간색의 HSV 색상 범위 정의
//	Scalar lower_red1 = Scalar(10, 120, 70); // 색상, 채도, 명도 
//	Scalar upper_red1 = Scalar(20, 255, 255);
//	Scalar lower_red2 = Scalar(170, 120, 70);
//	Scalar upper_red2 = Scalar(180, 255, 255);
//
//	// 빨간색 범위에 해당하는 마스크 생성
//	Mat mask1, mask2, red_mask;
//	inRange(img_hsv, lower_red1, upper_red1, mask1);
//	inRange(img_hsv, lower_red2, upper_red2, mask2);
//	red_mask = mask1 | mask2; // 두개의 마스크를 비트단위 OR 연산
//
//	// 빨간색 픽셀의 수 계산
//	int red_count = countNonZero(red_mask);
//
//	// 빨간색 픽셀이 충분한 경우 "RED" 표시
//	if (red_count > 500) { // 500은 임의의 값으로, 필요에 따라 조정하세요.
//		putText(img_frame, "RED", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1.2, Scalar(0, 0, 255), 3);
//	}
//
//	imshow("frame", img_frame);
//	imshow("mask", img_mask);
//	imshow("red mask", red_mask);
//}
//
//int func_opencv() {
//	VideoCapture cap(0);
//	if (!cap.isOpened())
//	{
//		cout << "카메라를 열 수 없습니다." << endl;
//		return -1;
//	}
//
//	Mat img_frame;
//	while (1) {
//		bool ret = cap.read(img_frame);
//		if (!ret)
//		{
//			cout << "캡쳐 실패" << endl;
//			break;
//		}
//
//		func_Joon(img_frame);
//		func_Lyn(img_frame);
//
//
//		int key = waitKey(1);
//		if (key == 27)
//			break;
//	}
//	cap.release();
//	return 0;
//}
//
//int main(void) {
//	func_opencv();
//}