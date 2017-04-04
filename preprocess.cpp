#include<opencv2/opencv.hpp>
#include<cstdlib>
#include<string>
#include "Preprocess.h"
#include "findnumber.h"
using namespace cv;

Preprocess::Preprocess()
{
	proornot = false;
	x = y = 0;
	rowlength = 107;
	collength = 80;
	step = 5;
	height = width = 0;
}

Preprocess::~Preprocess()
{
	proornot = false;
}

Mat Preprocess::process(Mat*srcImage)
{
	Mat dstImage;
	cvtColor(*srcImage, dstImage, CV_BGR2GRAY);
	GaussianBlur(dstImage, dstImage, Size(3, 3), 0, 0);
	threshold(dstImage, dstImage, 200, 255, 0);
	proornot = true;//标记为已经处理过了
	return dstImage;
}

void Preprocess::openprovideo(Mat* dstImage)
{
	if (proornot)
	{
		namedWindow("preprocess window");
		imshow("preprocess window", *dstImage);
	}
	else
	{
		cout << "the preprocess hasnot been done" << endl;
	}
}

//注意，接下里的这个寻找数字的函数有大量的监视窗口是在实际运行的时候可能需要关闭重写的

void Preprocess::findnum(Mat* dstImage, Mat* tempImage, Mat* code, Mat* unit, int condition, Point* codeCenter, Point* sfCenter)
{
	if (condition == complex)
	{
		//先进行了深度复制
		*tempImage = dstImage->clone();
		//接下来是定义和寻找轮廓
		std::vector<std::vector<cv::Point>> contours;
		findContours(*tempImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
		imshow("contours", *tempImage);
		rectsize = 14;
		std::vector<cv::Rect> rect(rectsize);
		int rect_cont = 0;
		for (int i = 0; i < contours.size(); ++i)
		{
			std::vector<cv::Rect> boundrect(contours.size());

			boundrect[i] = boundingRect(contours[i]);
			//cout << boundrect[i].area()<<endl;
			if ((boundrect[i].area() <= 900) && (boundrect[i].area()>320))
			{
				if (rect_cont >= 14)
				{
					rect_cont = 0;
				}
				rect[rect_cont] = boundrect[i];//不能使用rect(rectsize)
				cout << rect[rect_cont].area() << "x = " << rect[rect_cont].x << ", y = " << rect[rect_cont].y << endl;
				rectangle(*tempImage, rect[rect_cont++], Scalar(255, 168, 0));
			}
			imshow("rectangluar", *tempImage);
		}
		Rect* p_index[14];
		Rect* p_voidptr = nullptr;
		for (int i = 0; i < 14; ++i)
		{
			p_index[i] = &rect[i];
		}

		//下面是排列找到的数字单元的排列算法，简单的插排
		//先是y轴上的排序确定四行的存在
		for (int i = 0; i < rectsize; ++i)
		{

			for (int j = i + 1; j < rectsize; ++j)
			{
				if (p_index[i]->y>p_index[j]->y)
				{
					//swap(rect[i],rect[j]);
					//p_voidptr = p_index[i];
					//p_index[i] = p_index[j];
					//p_index[j] = p_voidptr;
					swap(p_index[i], p_index[j]);
				}
			}
		}

		//这里是对于x轴的排序，先是对于密码行的排序，再是接下来几行的排序
		//排完之后的顺序是从左到右，从上往下
		for (int i = 0; i < 5; ++i)
		{
			for (int j = i + 1; j < 5; ++j)
			{
				if (p_index[i]->x>p_index[j]->x)
				{
					//swap(rect[i],rect[j]);
					p_voidptr = p_index[i];
					p_index[i] = p_index[j];
					p_index[j] = p_voidptr;
				}
			}
		}
		for (int k = 0; k < 7; k += 3)
		{
			for (int i = 5 + k; i < 8 + k; ++i)
			{
				for (int j = i + 1; j <8 + k; ++j)
				{
					if (p_index[i]->x > p_index[j]->x)
					{
						//bug 数字如果朝右上方，最下面两排排序会出错
						//swap(rect[i], rect[j]);
						p_voidptr = p_index[i];
						p_index[i] = p_index[j];
						p_index[j] = p_voidptr;
					}
				}
			}
		}
		//这里是对于code和unit的赋值
		//unit是shenfu[]在这里的参数名
		cout << endl;
		for (int i = 0; i < 5; ++i)
		{
			cout << p_index[i]->x << " " << p_index[i]->y << endl;
			code[i] = (*dstImage)(Rect(p_index[i]->tl(), p_index[i]->br()));
			//
			codeCenter[i] = getCenterPoint(p_index[i]);
			//
			//code[i] = (*dstImage)(Rect(rect[i].tl(), rect[i].br()));
		}

		for (int i = 0; i < 9; ++i)
		{
			cout << p_index[i + 5]->x << " " << p_index[i + 5]->y << endl;
			unit[i] = (*dstImage)(Rect(p_index[i + 5]->tl(), p_index[i + 5]->br()));
			//
			sfCenter[i] = getCenterPoint(p_index[i + 5]);
			//
			//unit[i] = (*dstImage)(Rect(rect[i + 5].tl(), rect[i + 5].br()));
		}
	}
}

//
Point Preprocess::getCenterPoint(Rect* rect)
{
	Point cpt;
	cpt.x = rect->x + cvRound(rect->width / 2.0);
	cpt.y = rect->y + cvRound(rect->height / 2.0);
	return cpt;
}
//

void Shenfu::opentest(bool condition)
{
	istest = condition;
}

void Shenfu::startpreprocess()
{
	cap.open(1);
	if (!cap.isOpened())
	{
		cout << "open video failed." << endl;
		exit(0);
	}
	if (istest)
	{
		namedWindow("orign");
	}
	while (!controlpoint)
	{
		if (!cap.read(srcImage))
		{
			cout << "read per frame error." << endl;
			exit(0);
		}
		if (istest)
			imshow("orign", srcImage);
		dstImage = pro.process(&srcImage);
		if (istest)
		{
			pro.openprovideo(&dstImage);
		}
		//
		pro.findnum(&dstImage, &tempImage, code, shenfu, complex, codeCenter, sfCenter);
		//
		//pro.findDigit(&dstImage, &tempImage, knn, code[0]);
		//pro.findDigit(&dstImage, &tempImage, knn, shenfu[0]);

		imshow("rectangluar", tempImage);

		int stopor = waitKey(delay);//在延迟时间里面不更新图像，也就是控制图像的更新
		if (stopor >= 0 && istest)
		{
			waitKey(0);
			controlpoint = true;
		}
	}
}

void Shenfu::showROI()
{
	for (int i = 0; i < 5; ++i)
	{
		int num = findDigit(code[i]);
		cout << num << endl;
		// 加的 codePic 数组
		codePic[i].number = num;
		codePic[i].picture = code[i];
		codePic[i].centerPoint = codeCenter[i];
		//
		imshow("code", code[i]);
		waitKey(1000);
	}
	for (int i = 0; i < 9; ++i)
	{
		int num = findDigit(shenfu[i]);
		cout << num << endl;
		sfPic[i].number = num;
		sfPic[i].picture = shenfu[i];
		//
		sfPic[i].centerPoint = sfCenter[i];
		//
		imshow("shenfu", shenfu[i]);
		waitKey(1000);
	}
}

int Shenfu::findDigit(Mat rois) {
	std::string strFinalString;
	Mat th;

	//一系列处理
	cv::threshold(rois, th, 50, 255, THRESH_BINARY);
	resize(th, th, Size(20, 30));//将图片resize20*30大小
	Mat matROIFloat;
	th.convertTo(matROIFloat, CV_32FC1);
	Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
	Mat matCurrentChar(0, 0, CV_32F);

	knn->findNearest(matROIFlattenedFloat, 1, matCurrentChar);//进行knn算法识别数字

	float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
	strFinalString = strFinalString + char(int(fltCurrentChar));//算出数字
	int number = atoi(strFinalString.c_str());
	return number;
}

void Shenfu::readTrainData() {
	knn = ml::StatModel::load<ml::KNearest>("KNN_DATA.xml");//读取数字识别xml文件
}

//
Point Shenfu::returnHitPoint(findnumber code) 
{
	for (int i = 0; i < 9; i++)
	{
		if (code.number == sfPic[i].number)
		{
			return sfPic[i].centerPoint;
		}
	}
	cout << "Cannot find code number." << endl;
	return 0;
}
//


/*
int Preprocess::getrowvalue(Mat *srcImage)
{
	for (int i = 0; i <= srcImage->cols; ++i)
	{
		if (srcImage->ptr<uchar>(0)[i] != 0)
			return i;
	}
	return 0;
}*/
/*
int Preprocess::getcolvalue(Mat *srcImage)
{
	for (int i = 0; i <= srcImage->rows; ++i)
	{
		if (srcImage->ptr<uchar>(i)[0] != 0)
			return i;
	}
	return 0;

}*/
/*
int* Preprocess::getinf()
{
	int list[5] = { width, height, x, y, rectsize };
	return list;
}*/
/*
#include<opencv2/opencv.hpp>
#include "Preprocess.h"
using namespace cv;

// Preprocess 的实现
Preprocess::Preprocess()
{
	proornot = false;
	x = y = 0;
	rowlength = 107;
	collength = 80;
	step = 5;
	height = width = 0;
}

Preprocess::~Preprocess()
{
	proornot = false;
}

Mat Preprocess::process(Mat*srcImage)
{
	Mat dstImage;
	cvtColor(*srcImage, dstImage, CV_BGR2GRAY);
	GaussianBlur(dstImage, dstImage, Size(3, 3), 0, 0);
	threshold(dstImage, dstImage, 200, 255, 0);
	proornot = true;//标记为已经处理过了
	return dstImage;
}

void Preprocess::openprovideo(Mat* dstImage)
{
	if (proornot)
	{
		namedWindow("preprocess window");
		imshow("preprocess window", *dstImage);
	}
	else
	{
		cout << "the preprocess hasnot been done" << endl;
	}
}

int Preprocess::getrowvalue(Mat *srcImage)
{
	for (int i = 0; i <= srcImage->cols; ++i)
	{
		if (srcImage->ptr<uchar>(0)[i] != 0)
			return i;
	}
	return 0;
}

int Preprocess::getcolvalue(Mat *srcImage)
{
	for (int i = 0; i <= srcImage->rows; ++i)
	{
		if (srcImage->ptr<uchar>(i)[0] != 0)
			return i;
	}
	return 0;
}

//注意，接下里的这个寻找数字的函数有大量的监视窗口是在实际运行的时候可能需要关闭重写的
void Preprocess::findnum(Mat* dstImage, Mat* tempImage, Mat* code, Mat* unit, int condition)
{
	if (condition == complex)
	{
		//先进行了深度复制
		*tempImage = dstImage->clone();
		//接下来是定义和寻找轮廓
		std::vector<std::vector<cv::Point>> contours;
		findContours(*tempImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
		drawContours(*tempImage, contours, 2, Scalar(255, 255, 0));
		imshow("contours", *tempImage);
		rectsize = 14;
		std::vector<cv::Rect> rect(rectsize);
		//std::vector<Rect>* p_rect = &rect;
		int rect_cont = 0;
		for (int i = 0; i < contours.size(); ++i)
		{
			std::vector<cv::Rect> boundrect(contours.size());

			boundrect[i] = boundingRect(contours[i]);
			//cout << boundrect[i].area()<<endl;
			if ((boundrect[i].area() <= 900) && (boundrect[i].area()>320))
			{
				if (rect_cont >= 14)
				{
					rect_cont = 0;
				}
				rect[rect_cont] = boundrect[i];
				cout << rect[rect_cont].area() << " " << rect[rect_cont].x << " " << rect[rect_cont].y << endl;
				rectangle(*tempImage, rect[rect_cont++], Scalar(255, 168, 0));
			}

			imshow("rectangluar", *tempImage);
		}

		Rect* p_index[14];
		Rect* p_voidptr = nullptr;
		for (int i = 0; i < 14; ++i)
		{
			p_index[i] = &rect[i];
		}
		//下面是排列找到的数字单元的排列算法，简单的插排
		//先是y轴上的排序确定四行的存在
		for (int i = 0; i < rectsize; ++i)
		{
			for (int j = i + 1; j < rectsize; ++j)
			{
				if (p_index[i]->y>p_index[j]->y)
				{
					//swap(rect[i],rect[j]);
					//p_voidptr = p_index[i];
					//p_index[i] = p_index[j];
					//p_index[j] = p_voidptr;
					swap(p_index[i], p_index[j]);
				}
			}
		}

		//这里是对于x轴的排序，先是对于密码行的排序，再是接下来几行的排序
		//排完之后的顺序是从左到右，从上往下
		for (int i = 0; i < 5; ++i)
		{
			for (int j = i + 1; j < 5; ++j)
			{
				if (p_index[i]->x>p_index[j]->x)
				{
					//swap(rect[i],rect[j]);
					p_voidptr = p_index[i];
					p_index[i] = p_index[j];
					p_index[j] = p_voidptr;
				}
			}
		}
		for (int k = 0; k < 7; )
		{
			for (int i = 5 + k; i < 8 + k; ++i)
			{
				for (int j = i + 1 + k; j <8 + k; ++j)
				{
					if (p_index[i]->x>p_index[j]->x)
					{
						//swap(rect[i], rect[j]);
						p_voidptr = p_index[i];
						p_index[i] = p_index[j];
						p_index[j] = p_voidptr;
					}
				}
			}
			k = k + 3;
		}
		//这里是对于code和unit的赋值
		//unit是shenfu[]在这里的参数名
		cout << endl;
		for (int i = 0; i < 5; ++i)
		{
			cout << p_index[i]->x << " " << p_index[i]->y << endl;
			code[i] = (*dstImage)(Rect(p_index[i]->tl(), p_index[i]->br()));
			//code[i] = (*dstImage)(Rect(rect[i].tl(), rect[i].br()));
		}

		for (int i = 0; i < 9; ++i)
		{
			cout << p_index[i + 5]->x << " " << p_index[i + 5]->y << endl;
			unit[i] = (*dstImage)(Rect(p_index[i + 5]->tl(), p_index[i + 5]->br()));
			//unit[i] = (*dstImage)(Rect(rect[i + 5].tl(), rect[i + 5].br()));
		}
	}
}

int* Preprocess::getinf()
{
	int list[5] = { width, height, x, y, rectsize };
	return list;
}

// Shenfu 的实现
void Shenfu::opentest(bool condition)
{
	istest = condition;
}

void Shenfu::startpreprocess()
{
	cap.open(0);
	if (!cap.isOpened())
	{
		cout << "open video failed." << endl;
		exit(0);
	}
	if (istest)
	{
		namedWindow("orign");
	}
	while (!controlpoint)
	{
		if (!cap.read(srcImage))
		{
			cout << "read per frame error." << endl;
			exit(0);
		}
		if (istest)
		{
			imshow("orign", srcImage);
		}
		dstImage = pro.process(&srcImage);
		if (istest)
		{
			pro.openprovideo(&dstImage);
		}

		pro.findnum(&dstImage, &tempImage, code, shenfu, complex);
		//pro.findDigit(&dstImage, &tempImage, knn, rois);
			
		imshow("rectangluar", tempImage);
		
		int stopor = waitKey(delay);//在延迟时间里面不更新图像，也就是控制图像的更新
		if (stopor >= 0 && istest)
		{
			waitKey(0);
			controlpoint = true;
		}
	}
}

void Shenfu::showROI()
{	
	for (int i = 0; i < 5; ++i)
	{
		imshow("code", code[i]);
		waitKey(1000);
	}
	for (int i = 0; i < 9; ++i)
	{
		imshow("shenfu", shenfu[i]);
		waitKey(1000);
	}
}*/
/*
void Preprocess::findDigit(Mat* dstImage, Mat* tempImage, Ptr<ml::KNearest> knn, std::vector<Rect> rois){
	for (int r = 0; r < rois.size(); r++)
	{
		Mat roi(*dstImage, rois[r]);//将Roi图片在dstImage中找出
		std::string strFinalString;
		Mat th;

		//一系列处理
		cv::threshold(roi, th, 50, 255, THRESH_BINARY);
		resize(th, th, Size(20, 30));//将图片resize20*30大小
		Mat matROIFloat;
		th.convertTo(matROIFloat, CV_32FC1);
		Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
		Mat matCurrentChar(0, 0, CV_32F);

		knn->findNearest(matROIFlattenedFloat, 1, matCurrentChar);//进行knn算法识别数字

		float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
		strFinalString = strFinalString + char(int(fltCurrentChar));//算出数字
		putText(*tempImage, strFinalString, Point(rois[r].x, rois[r].y), FONT_HERSHEY_SIMPLEX, 1, Scalar(127, 0, 255), 2);//将数字显示在方框上
	}
}

void Shenfu::readTrainData(){
	knn = ml::StatModel::load<ml::KNearest>("KNN_DATA.xml");//读取数字识别xml文件
}

*/
