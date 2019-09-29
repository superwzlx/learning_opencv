#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "chapter_02.h"
#include <Windows.h> //���ߺ���Sleep()Ҫ�õ��ÿ�

#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

//Exercise 2 in chapter 2
static void help()
{
	// print a welcome message, and the OpenCV version
	cout << "\nThis is a demo of Lukas-Kanade optical flow lkdemo(),\n"
		"Using OpenCV version " << CV_VERSION << endl;
	cout << "\nIt uses camera by default, but you can provide a path to video as an argument.\n";
	cout << "\nHot keys: \n"
		"\tESC - quit the program\n"
		"\tr - auto-initialize tracking\n"
		"\tc - delete all the points\n"
		"\tn - switch the \"night\" mode on/off\n"
		"To add/remove a feature point click it\n" << endl;
}

Point2f point;
bool addRemovePt = false;

static void onMouse(int event, int x, int y, int /*flags*/, void* /*param*/)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		point = Point2f((float)x, (float)y);
		addRemovePt = true;
	}
}

int exercise2(int argc, char** argv)
{
	VideoCapture cap;
	TermCriteria termcrit(TermCriteria::COUNT | TermCriteria::EPS, 20, 0.03);
	Size subPixWinSize(10, 10), winSize(31, 31);

	const int MAX_COUNT = 500;
	bool needToInit = false;
	bool nightMode = false;

	help();
	cv::CommandLineParser parser(argc, argv, "{@input|0|}");
	string input = parser.get<string>("@input");

	if (input.size() == 1 && isdigit(input[0]))
		cap.open(input[0] - '0');
	else
		cap.open(input);

	if (!cap.isOpened())
	{
		cout << "Could not initialize capturing...\n";
		return 0;
	}

	namedWindow("LK Demo", 1);
	setMouseCallback("LK Demo", onMouse, 0);

	Mat gray, prevGray, image, frame;
	vector<Point2f> points[2];

	for (;;)
	{
		cap >> frame;
		if (frame.empty())
			break;

		frame.copyTo(image);
		cvtColor(image, gray, COLOR_BGR2GRAY);

		if (nightMode)
			image = Scalar::all(0);

		if (needToInit)
		{
			// automatic initialization
			goodFeaturesToTrack(gray, points[1], MAX_COUNT, 0.01, 10, Mat(), 3, 3, 0, 0.04);
			cornerSubPix(gray, points[1], subPixWinSize, Size(-1, -1), termcrit);
			addRemovePt = false;
		}
		else if (!points[0].empty())
		{
			vector<uchar> status;
			vector<float> err;
			if (prevGray.empty())
				gray.copyTo(prevGray);
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
				3, termcrit, 0, 0.001);
			size_t i, k;
			for (i = k = 0; i < points[1].size(); i++)
			{
				if (addRemovePt)
				{
					if (norm(point - points[1][i]) <= 5)
					{
						addRemovePt = false;
						continue;
					}
				}

				if (!status[i])
					continue;

				points[1][k++] = points[1][i];
				circle(image, points[1][i], 3, Scalar(0, 255, 0), -1, 8);
			}
			points[1].resize(k);
		}

		if (addRemovePt && points[1].size() < (size_t)MAX_COUNT)
		{
			vector<Point2f> tmp;
			tmp.push_back(point);
			cornerSubPix(gray, tmp, winSize, Size(-1, -1), termcrit);
			points[1].push_back(tmp[0]);
			addRemovePt = false;
		}

		needToInit = false;
		imshow("LK Demo", image);

		char c = (char)waitKey(10);
		if (c == 27)
			break;
		switch (c)
		{
		case 'r':
			needToInit = true;
			break;
		case 'c':
			points[0].clear();
			points[1].clear();
			break;
		case 'n':
			nightMode = !nightMode;
			break;
		}

		std::swap(points[1], points[0]);
		cv::swap(prevGray, gray);
	}

	return 0;
}

/*
Exercise 3 in chapter 2
��Ŀ����ȡ��Ƶ�ļ��������²�������ļ����Լ��޸ĵ�,ԭ��Ŀ���������
*/
static void downSampleAndSave(const string src_path,const string dst_path)
{
	cv::Mat frame;
	cv::Mat frame1;
	cv::namedWindow("video_player");
	//��ȡ��Ƶ,��һ��VideoCapture����,Ȼ�����open����,Ȼ��ʹ��cap>>frame����֡
	cv::VideoCapture cap;
	if (cap.open(src_path))
	{
		//�½�VideoWriter����,����open(),����dst_path,��Ƶ�����ʽ,fps,size
		double fps = cap.get(cv::CAP_PROP_FPS);
		//��ȡ��Ƶ��֡��
		double frames_num = cap.get(cv::CAP_PROP_FRAME_COUNT);
		/*cv::Size size(
			(int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
			(int)cap.get(CAP_PROP_FRAME_HEIGHT)       
			//ע����frame width,��Ҫд��������width
			//ע�⣺�����������,�����޸���frame��size,����size������ԭ��Ƶ�ĳߴ�,���size��д���frame����Ӧ,�ᵼ����Ƶд�벻�ɹ�!
			//Ҫע��size��д���֡��С��Ӧ.
		);*/
		cv::VideoWriter writer;
		//����֡
		cap >> frame; //ȡ��һ֡���������ǵõ�width��height
		cv::pyrDown(frame, frame1);
		cv::Size size(
			(int)frame1.cols,  //��������width
			(int)frame1.rows   //��������height
		);
		writer.open(dst_path, CV_FOURCC('M', 'J', 'P', 'G'), fps, size);
		writer << frame1;
		int i = 1;
		for (;;)
		{
			cap >> frame;
			if (frame.empty()) break;
			//��֡���в���,��������²���,����pyrDown(),�ú����ڽ����²���֮ǰ�����˸�˹ģ��
			cv::pyrDown(frame, frame1);
			cv::imshow("video_palyer", frame1);
			cv::waitKey(20);
			//�Դ������֡����д��
			writer << frame1;
			//��ʾд�����
			cout << "д����Ƶ���ȣ�" << ((i++) / frames_num)*100 << "%" << "\r";  //ʹ��\r����endl,�������
		}
		writer.release();
	}
	cv::destroyWindow("video_p	layer");
	cap.release();
}
void exercise3(const string &src_path,const string &dst_path)
{
	downSampleAndSave(src_path,dst_path);
}

/*
exercise 5 in chapter2
��Ŀ������Ƶ�ļ������²���,��ӻ�����,ʹ���û����Զ�̬���ƽ������Ľ������ȼ�
������������ʾ����,��Ҫ�󱣴洦������Ƶ
ע1; pyrdown()��������ĵ���������size,��Ҫȥָ����,ʹ��Ĭ��ֵ����,��Ϊ�ú������ǽ�ͼ���²���Ϊ2��
	 ���Ҫ�²���Ϊ4����8��,��ͨ������²�����ʵ��
ע2: �ڵ���������λ�õ�ʱ��,������ִ�лص�����,����Ҫ��֯�ô���ʵ�ֵ��߼�,����ȫ�ֱ�������ȫ�ֱ���
ע3: ����Ĺ��������on_Trackbar()��ʵ��
*/

const int g_MaxValue = 10;  //���������ֵ
int g_SliderValue;  //��������Ӧ����
VideoCapture cap;
Mat frame;

//��Ӧ�������Ļص�����
void on_Trackbar(int, void*)  //�ص����������ٺ�����������,����ϵͳ����Ϊ����һ���ص�����
{
	Mat frame1;
	while (true)
	{
		if (frame.empty()) break;
		//ͨ����ε���pyrDown()ʵ���²���,g_SliderValueΪ�²����ȼ�,value=3��ʾ�²���4��,������2^(value-1)
		for (int i = 0; i < g_SliderValue-1; i++)
		{
			cv::pyrDown(frame, frame1);
			frame = frame1;
		}
		cv::imshow("�²�����ͼ", frame1);
		cv::waitKey(20);
		cap >> frame;
	}
}
void downSampleWithBar(const string path)
{
	g_SliderValue = 2;  //���û������ĳ�ʼλ��
	char TrackbarName[50] = "�²����ȼ�";
	if (cap.open(path))
	{
		cap >> frame;
		cout << "����Ƶ�ļ��ɹ���" << endl;
	}
	else
	{
		cout << "����Ƶ�ļ�����" << endl;
	}
	cv::namedWindow("�²�����ͼ",0); //�ڴ���������֮ǰӦ�ý�����
	cv::resizeWindow("�²�����ͼ", cv::Size(frame.cols, frame.rows));
	createTrackbar(TrackbarName, "�²�����ͼ", &g_SliderValue, g_MaxValue, on_Trackbar); //������������Ҫ����������,��������,��������ֵ,���ֵ�ͻص�����
	//��ʾ���ûص�����,����Ĭ��ֵ,���û����ʾ���ûص�����,����Ҫ������������ܳ����ص�����
	on_Trackbar(g_SliderValue, 0);
	cv::waitKey(0);
}
void exercise5(const string & src_path)
{
	downSampleWithBar(src_path);
}