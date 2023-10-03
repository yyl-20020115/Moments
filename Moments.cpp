//图像矩：（Image Moments）
//步骤：提取图像边缘
//发现轮廓
//计算每个轮廓对象的矩
//计算每个对象的中心、弧长、面积

#include<opencv2/opencv.hpp>
#include<iostream>
#include<math.h>

cv::Mat src, dst, drawImage;
const char* result = "moments_demo";
int threshold_value = 120;
int threshold_max = 255;
cv::RNG rng(12345);

void Moments_demo(int, void*);

int main(int argc, char* argv)
{
	src = cv::imread("items.png");
	if (!src.data)
	{
		printf("could not load image...\n");
		return -1;
	}
	char input[] = "gray image";
	cv::namedWindow(input, cv::WINDOW_AUTOSIZE);
	cv::namedWindow(result, cv::WINDOW_AUTOSIZE);
	//输入图像转为灰度图像
	cvtColor(src, dst, cv::COLOR_BGR2GRAY);
	GaussianBlur(dst, dst, cv::Size(3, 3), 0, 0);
	cv::imshow(input, dst);

	const char* thresh = "threshold value";
	cv::createTrackbar(thresh, result, &threshold_value, threshold_max, Moments_demo);
	Moments_demo(0, 0);

	cv::waitKey(0);
	return 0;
}

void Moments_demo(int value, void* params)
{
	//提取图像边缘
	cv::Mat canny_out;
	cv::Canny(dst, canny_out, threshold_value, threshold_value * 2, 3, false);
	//imshow("canny image", canny_out);

	//发现轮廓，找到图像轮廓
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierachy;
	findContours(canny_out, contours, hierachy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//计算每个轮廓对象的矩
	std::vector<cv::Moments> contours_moments(contours.size());
	std::vector<cv::Point2f> centers(contours.size());
	for (size_t i = 0; i < contours.size(); i++)
	{
		//计算矩
		contours_moments[i] = cv::moments(contours[i]);
		//moments(InputArray  array,//输入数据
		//bool   binaryImage = false // 是否为二值图像
		centers[i] = cv::Point(static_cast<float>(
			contours_moments[i].m10 / contours_moments[i].m00), 
			static_cast<float>(contours_moments[i].m01 / contours_moments[i].m00));
		//图像中心Center(x0, y0)=(m10/m00,m01/m00)
	}

	src.copyTo(drawImage);
	for (size_t i = 0; i < contours.size(); i++)
	{
		printf("centers point x：%.2f,y：%.2f\n", centers[i].x, centers[i].y);
		printf("contours %d Area：%.2f Arc length：%.2f \n", i, cv::contourArea(contours[i]), cv::arcLength(contours[i], true));
		//contourArea(InputArray  contour,//输入轮廓数据
		//bool   oriented// 默认false、返回绝对值)
		//arcLength(InputArray  curve,//输入曲线数据
		//bool   closed// 是否是封闭曲线)

		//考虑如何把数据显示在原图像上
		//double A;
		//A=contourArea(contours[i]);
		//std::ostringstream os;
		//os << A;
		//putText(drawImage,os.str(), centers[i], cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 2, 8);
		//依次含义：原图，输入字的内容，起始位置，字体，字的大小，颜色，线条大小粗 细，连接域

		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::drawContours(drawImage, contours, i, color, 2, cv::LINE_AA, hierachy, 0, cv::Point(0, 0));//绘制轮廓
		
		circle(drawImage, centers[i], 2, color, 2, cv::LINE_AA);//绘制图形中心
	}
	imshow(result, drawImage);
	return;
}