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
//		//���� �����Ӱ� ���� ������ ���� ���� ���� ���
//		absdiff(prev_frame, img_frame, diff_frame);
//
//		//���� �̹����� �׷��̽����Ϸ� ��ȯ
//		cvtColor(diff_frame, diff_frame, COLOR_BGR2GRAY);
//
//		//����ȭ
//		threshold(diff_frame, diff_frame, 30, 255, THRESH_BINARY);
//
//		//�������� �ִ� �ȼ� �� ���
//		int num_motion_pixels = countNonZero(diff_frame);
//
//		//�������� �ִ��� Ȯ��
//		if (num_motion_pixels > 10000) // ������ �Ӱ谪�� ����
//		{
//			//��ü �ĺ� �ڵ� �߰�
//			//���� ���� ����
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
//			//�ش� ������ ��ü�� ����
//			//inrange: ���� ������ �ش��ϸ� 1����, �� �ܿ��� 0���� ������ �� ���� ���� ��������ũ �̹��� ����
//			inRange(img_hsv, lower_red, upper_red, img_mask_red);
//			inRange(img_hsv, lower_red1, upper_red1, img_mask_red1);
//			inRange(img_hsv, lower_orange, upper_orange, img_mask_orange);
//			inRange(img_hsv, lower_yellow, upper_yellow, img_mask_yellow);
//
//			//�������� �ִ� ��ü���� �ĺ�
//			//bitwise_or: ����ũ�� ��ħ
//			//�� ���� �̹����� �Է¹޾� ���� �ϳ��� 1�̸� ��� �̹����� �ش� ��ġ�� 1�� ����
//			Mat img_mask_motion;
//			bitwise_or(img_mask_red, img_mask_red1, img_mask_motion);
//			bitwise_or(img_mask_motion, img_mask_orange, img_mask_motion);
//			bitwise_or(img_mask_motion, img_mask_yellow, img_mask_motion);
//
//			//��ü�� ROI�� ã��
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
//	// ���� �������� ���� ���������� ����
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
//	// ��ü �ܰ��� ������ ���� �׸�
//	//drawContours(img_frame, contours, -1, Scalar(0, 0, 255), 2);
//
//	// BGR���� HSV�� ��ȯ
//	Mat img_hsv;
//	cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);
//
//	// �������� HSV ���� ���� ����
//	Scalar lower_red1 = Scalar(10, 120, 70); // ����, ä��, �� 
//	Scalar upper_red1 = Scalar(20, 255, 255);
//	Scalar lower_red2 = Scalar(170, 120, 70);
//	Scalar upper_red2 = Scalar(180, 255, 255);
//
//	// ������ ������ �ش��ϴ� ����ũ ����
//	Mat mask1, mask2, red_mask;
//	inRange(img_hsv, lower_red1, upper_red1, mask1);
//	inRange(img_hsv, lower_red2, upper_red2, mask2);
//	red_mask = mask1 | mask2; // �ΰ��� ����ũ�� ��Ʈ���� OR ����
//
//	// ������ �ȼ��� �� ���
//	int red_count = countNonZero(red_mask);
//
//	// ������ �ȼ��� ����� ��� "RED" ǥ��
//	if (red_count > 500) { // 500�� ������ ������, �ʿ信 ���� �����ϼ���.
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
//		cout << "ī�޶� �� �� �����ϴ�." << endl;
//		return -1;
//	}
//
//	Mat img_frame;
//	while (1) {
//		bool ret = cap.read(img_frame);
//		if (!ret)
//		{
//			cout << "ĸ�� ����" << endl;
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