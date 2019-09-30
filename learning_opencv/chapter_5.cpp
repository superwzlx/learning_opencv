#include "chapter_05.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using std::cout;
using std::endl;

void exercise1() //��Բ
{
	Mat mat = Mat(100, 100, CV_8UC3, Scalar(0));  //CV_8UĬ��ͨ��Ϊ1
	cv::circle(mat, Point(mat.cols / 2, mat.rows / 2), 30, Scalar(255)); //Scalar(255)��ʾ��һ��ֵ��ֵΪ255,����Ϊ0;
	cv::namedWindow("exercise1");
	cv::imshow("exercise1", mat);
	waitKey(0);
	cv::destroyWindow("exercise1");
}

void exercise2() //Mat��Ԫ�ط��ʺ���.at�޸�����
{
	Mat mat = Mat(100, 100, CV_8UC3, Scalar(0));
	for (int row = 0; row < mat.rows; row++)
	{
		for (int col = 0; col < mat.cols; col++)
		{
			//mat.at<Vec3b>(row, col) = (255, 255, 255); ������ֵ�ﲻ��Ч������ֵ����Ԫ�ص�ֵ��ʵΪ(255,0,0)
			mat.at<Vec3b>(row, col) = Vec3b(255, 255, 255);
			
			//��һ�ַ�����������ֵ
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

void exercise3() //ָ�뷽ʽ�����޸�����,����Ҫ����֪��mat����������
{
	Mat mat = Mat(100, 100, CV_8UC3,Scalar(0));
	for (int row = 0; row < mat.rows; row++)
	{
		uchar* ptr = mat.ptr(row);  //�������row,�õ����е�ָ��,uchar* ����
		for (int col = 0; col < mat.cols; col++)
		{
			//if (20 <= row <= 40 && 5 <= col <= 20)     //�����ж�������д���Ǵ����
			if(row>=20 && row<=40 && col>=5 && col<=20)
			{
				ptr[col * 3 + 1] = 255;  //col*3 + 1��Ϊgͨ��,����ǵ�ͨ��,�򲻳���3 
			}
		}
	}
	cv::namedWindow("exercise3");
	cv::imshow("exercise3", mat);
	cv::waitKey(0);
	cv::destroyWindow("exercise3");
}

void exercise4()  //roi��setTo()��ʹ��,roi����Ϊmat
//ע�⣺Rect����������Point����,��Ϊ��������,�������4��int,ǿ�����������Ͻ�,����������width��height
{
	Mat mat = Mat(210, 210, CV_8U, Scalar(0));
	for (int i = 0; i < 210 / 2; i += 10)
	{
		Mat roi = mat(Rect(Point(i, i), Point(210 - i, 210 - i)));   //ͨ������Rect�����mat����,�õ�roi����,roi������Ϊmat
		roi.setTo(i * 2);  //����setTo������roi��������޸�
	}
	cv::imshow("exercise4", mat);  //����ʡ��namedWindow(),ֱ��imshow
	cv::waitKey(0);
	cv::destroyWindow("exercise4");
}

void exercise5()
{
	Mat image = cv::imread("images//2.jpg");
	Mat roi1 = image(Rect(5, 10, 20, 30));  //roi�������ƺ���һ���¶���,����ʵ�����Ĳ�������Ӱ��ԭͼ��image��.
	Mat roi2 = image(Rect(50, 60, 20, 30));
	cv::bitwise_not(roi1, roi1);
	cv::bitwise_not(roi2, roi2);    
	cv::imshow("exercise5", image);
	cv::waitKey(0);
	cv::destroyWindow("exercise5");
}

void exercise6() //cv::compare()��cv::split()��ʹ��
{
	/*
	���������Ĺ��ܣ���ͼ�����ɫͨ���������,Ȼ����thresdֵ,
					������ɫͨ���д���thresdֵ��Ԫ��,��ȥthesd/2,Ȼ��洢��ԭ�����Ǹ���ɫͨ��ͼ����.
	*/
	Mat image = cv::imread("images//1.jpg");
	for (int i = 0; i < 2; i++)  //1.jpg�ֱ��ʺܸ�,����������4���²���
	{
		cv::pyrDown(image, image);
	}
	std::vector<Mat> chanels;
	cv::split(image, chanels);  //����ͨ��
	Mat g_chanel = chanels[1];
	cv::imshow("green", g_chanel);

	Mat clone1 = g_chanel.clone();
	Mat clone2 = g_chanel.clone();

	//��Mat�����е�������Сֵ
	double minPixel, maxPixel;
	int minPosition, maxPosition;
	cv::minMaxIdx(g_chanel, &minPixel, &maxPixel,&minPosition,&maxPosition);  //�����Ҫ��λ��,Position�Ĳ����ǿ��Բ�����ȥ��  
    
	double thresh = (maxPixel - minPixel) / 2;

	clone1.setTo(thresh);
	clone2.setTo(0);

	cv::compare(g_chanel, clone1, clone2,cv::CMP_GE);
	/*
	compare()���ĸ������ĺ��壺
	CMP_EQ=0,    //���
	CMP_GT=1,   //����
	CMP_GE=2,   //���ڵ���
	CMP_LT=3,   //С��
	CMP_LE=4,   //С�ڵ���
	*/
	//clone1��Ԫ��ֵΪthresh,ͨ��compare,clone2��Ϊһ����ʶg_chanel�е�ֵ����thresh������ͼ��

	cv::imshow("clone2", clone2);

	//������,����ڶ���������inputArray,������һ����Ҳ�ǿ�����������,���������������,���ĸ�����������ͼ��
	cv::subtract(g_chanel, thresh / 2, g_chanel, clone2);

	cv::imshow("green_subtracted", g_chanel);

	cv::waitKey(0);
}












