#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "chapter_02.h"
#include <Windows.h> //休眠函数Sleep()要用到该库

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
题目：读取视频文件并保存下采样后的文件（自己修改的,原题目不是这个）
*/
static void downSampleAndSave(const string src_path,const string dst_path)
{
	cv::Mat frame;
	cv::Mat frame1;
	cv::namedWindow("video_player");
	//读取视频,建一个VideoCapture对象,然后调用open函数,然后使用cap>>frame读入帧
	cv::VideoCapture cap;
	if (cap.open(src_path))
	{
		//新建VideoWriter对象,调用open(),传入dst_path,视频编码格式,fps,size
		double fps = cap.get(cv::CAP_PROP_FPS);
		//获取视频总帧数
		double frames_num = cap.get(cv::CAP_PROP_FRAME_COUNT);
		/*cv::Size size(
			(int)cap.get(CV_CAP_PROP_FRAME_WIDTH),
			(int)cap.get(CAP_PROP_FRAME_HEIGHT)       
			//注意是frame width,不要写成其它的width
			//注意：在这个例子中,我们修改了frame的size,所以size不能用原视频的尺寸,如果size与写入的frame不对应,会导致视频写入不成功!
			//要注意size与写入的帧大小对应.
		);*/
		cv::VideoWriter writer;
		//遍历帧
		cap >> frame; //取第一帧，方便我们得到width和height
		cv::pyrDown(frame, frame1);
		cv::Size size(
			(int)frame1.cols,  //列数：宽width
			(int)frame1.rows   //行数：高height
		);
		writer.open(dst_path, CV_FOURCC('M', 'J', 'P', 'G'), fps, size);
		writer << frame1;
		int i = 1;
		for (;;)
		{
			cap >> frame;
			if (frame.empty()) break;
			//对帧进行操作,这里进行下采样,调用pyrDown(),该函数在进行下采样之前进行了高斯模糊
			cv::pyrDown(frame, frame1);
			cv::imshow("video_palyer", frame1);
			cv::waitKey(20);
			//对处理过的帧进行写入
			writer << frame1;
			//显示写入进度
			cout << "写入视频进度：" << ((i++) / frames_num)*100 << "%" << "\r";  //使用\r代替endl,覆盖输出
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
题目：对视频文件进行下采样,添加滑动条,使得用户可以动态控制金字塔的降采样等级
并将处理结果显示出来,不要求保存处理后的视频
注1; pyrdown()函数里面的第三个参数size,不要去指定它,使用默认值即可,因为该函数就是将图像下采样为2倍
	 如果要下采样为4倍或8倍,则通过多次下采样来实现
注2: 在调整滑动条位置的时候,会重新执行回调函数,所以要组织好代码实现的逻辑,该用全局变量的用全局变量
注3: 处理的过程最好在on_Trackbar()中实现
*/

const int g_MaxValue = 10;  //滑动条最大值
int g_SliderValue;  //滑动条对应变量
VideoCapture cap;
Mat frame;

//响应滑动条的回调函数
void on_Trackbar(int, void*)  //回调函数不能再含有其他参数,否则系统不认为这是一个回调函数
{
	Mat frame1;
	while (true)
	{
		if (frame.empty()) break;
		//通过多次调用pyrDown()实现下采样,g_SliderValue为下采样等级,value=3表示下采样4倍,倍数：2^(value-1)
		for (int i = 0; i < g_SliderValue-1; i++)
		{
			cv::pyrDown(frame, frame1);
			frame = frame1;
		}
		cv::imshow("下采样视图", frame1);
		cv::waitKey(20);
		cap >> frame;
	}
}
void downSampleWithBar(const string path)
{
	g_SliderValue = 2;  //设置滑动条的初始位置
	char TrackbarName[50] = "下采样等级";
	if (cap.open(path))
	{
		cap >> frame;
		cout << "打开视频文件成功！" << endl;
	}
	else
	{
		cout << "打开视频文件出错！" << endl;
	}
	cv::namedWindow("下采样视图",0); //在创建滑动条之前应该建窗体
	cv::resizeWindow("下采样视图", cv::Size(frame.cols, frame.rows));
	createTrackbar(TrackbarName, "下采样视图", &g_SliderValue, g_MaxValue, on_Trackbar); //创建滑动条需要滑动条名字,窗口名字,滑动条的值,最大值和回调函数
	//显示调用回调函数,传入默认值,如果没有显示调用回调函数,则需要点击滑动条才能出发回调函数
	on_Trackbar(g_SliderValue, 0);
	cv::waitKey(0);
}
void exercise5(const string & src_path)
{
	downSampleWithBar(src_path);
}