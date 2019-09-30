#include "chapter_05.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using std::cout;
using std::endl;

void exercise1() //画圆
{
	Mat mat = Mat(100, 100, CV_8UC3, Scalar(0));  //CV_8U默认通道为1
	cv::circle(mat, Point(mat.cols / 2, mat.rows / 2), 30, Scalar(255)); //Scalar(255)表示第一个值赋值为255,其它为0;
	cv::namedWindow("exercise1");
	cv::imshow("exercise1", mat);
	waitKey(0);
	cv::destroyWindow("exercise1");
}

void exercise2() //Mat的元素访问函数.at修改像素
{
	Mat mat = Mat(100, 100, CV_8UC3, Scalar(0));
	for (int row = 0; row < mat.rows; row++)
	{
		for (int col = 0; col < mat.cols; col++)
		{
			//mat.at<Vec3b>(row, col) = (255, 255, 255); 这样赋值达不到效果，赋值给该元素的值其实为(255,0,0)
			mat.at<Vec3b>(row, col) = Vec3b(255, 255, 255);
			
			//另一种方案：单独赋值
			/*mat.at<Vec3b>(row, col)[0] = 255;
			mat.at<Vec3b>(row, col)[1] = 255;
			mat.at<Vec3b>(row, col)[2] = 255;*/
		}
	}
	cv::rectangle(mat, Rect(Point(20, 5), Point(40, 20)), Scalar(0, 255, 0));
	cv::namedWindow("exercise2");
	cv::imshow("exercise2", mat);
	cv::waitKey(0);
	cv::destroyWindow("exercise2");
}

void exercise3() //指针方式访问修改像素,不需要事先知道mat的数据类型
{
	Mat mat = Mat(100, 100, CV_8UC3,Scalar(0));
	for (int row = 0; row < mat.rows; row++)
	{
		uchar* ptr = mat.ptr(row);  //传入参数row,得到该行的指针,uchar* 类型
		for (int col = 0; col < mat.cols; col++)
		{
			//if (20 <= row <= 40 && 5 <= col <= 20)     //这种判断条件的写法是错误的
			if(row>=20 && row<=40 && col>=5 && col<=20)
			{
				ptr[col * 3 + 1] = 255;  //col*3 + 1则为g通道,如果是单通道,则不乘于3 
			}
		}
	}
	cv::namedWindow("exercise3");
	cv::imshow("exercise3", mat);
	cv::waitKey(0);
	cv::destroyWindow("exercise3");
}

void exercise4()  //roi中setTo()的使用,roi本质为mat
//注意：Rect对象传入两个Point对象,则为两个顶点,如果传入4个int,强两个代表左上角,后两个代表width和height
{
	Mat mat = Mat(210, 210, CV_8U, Scalar(0));
	for (int i = 0; i < 210 / 2; i += 10)
	{
		Mat roi = mat(Rect(Point(i, i), Point(210 - i, 210 - i)));   //通过传入Rect对象给mat对象,得到roi对象,roi对象本质为mat
		roi.setTo(i * 2);  //调用setTo函数对roi区域进行修改
	}
	cv::imshow("exercise4", mat);  //可以省略namedWindow(),直接imshow
	cv::waitKey(0);
	cv::destroyWindow("exercise4");
}

void exercise5()
{
	Mat image = cv::imread("images//2.jpg");
	Mat roi1 = image(Rect(5, 10, 20, 30));  //roi看起来似乎是一个新对象,但其实对它的操作都是影响原图像image的.
	Mat roi2 = image(Rect(50, 60, 20, 30));
	cv::bitwise_not(roi1, roi1);
	cv::bitwise_not(roi2, roi2);    
	cv::imshow("exercise5", image);
	cv::waitKey(0);
	cv::destroyWindow("exercise5");
}

void exercise6() //cv::compare()和cv::split()的使用
{
	/*
	整个函数的功能：将图像的绿色通道分离出来,然后求thresd值,
					对于绿色通道中大于thresd值的元素,减去thesd/2,然后存储到原来的那个绿色通道图像中.
	*/
	Mat image = cv::imread("images//1.jpg");
	for (int i = 0; i < 2; i++)  //1.jpg分辨率很高,这里对其进行4倍下采样
	{
		cv::pyrDown(image, image);
	}
	std::vector<Mat> chanels;
	cv::split(image, chanels);  //分离通道
	Mat g_chanel = chanels[1];
	cv::imshow("green", g_chanel);

	Mat clone1 = g_chanel.clone();
	Mat clone2 = g_chanel.clone();

	//求Mat对象中的最大和最小值
	double minPixel, maxPixel;
	int minPosition, maxPosition;
	cv::minMaxIdx(g_chanel, &minPixel, &maxPixel,&minPosition,&maxPosition);  //如果不要求位置,Position的参数是可以不传进去的  
    
	double thresh = (maxPixel - minPixel) / 2;

	clone1.setTo(thresh);
	clone2.setTo(0);

	cv::compare(g_chanel, clone1, clone2,cv::CMP_GE);
	/*
	compare()第四个参数的含义：
	CMP_EQ=0,    //相等
	CMP_GT=1,   //大于
	CMP_GE=2,   //大于等于
	CMP_LT=3,   //小于
	CMP_LE=4,   //小于等于
	*/
	//clone1的元素值为thresh,通过compare,clone2即为一个标识g_chanel中的值超过thresh的掩码图像

	cv::imshow("clone2", clone2);

	//做减法,这里第二个参数是inputArray,但传入一个数也是可以做减法的,第三个参数是输出,第四个参数是掩码图像
	cv::subtract(g_chanel, thresh / 2, g_chanel, clone2);

	cv::imshow("green_subtracted", g_chanel);

	cv::waitKey(0);
}












