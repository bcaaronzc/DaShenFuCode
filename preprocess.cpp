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
	proornot = true;//���Ϊ�Ѿ��������
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

//ע�⣬����������Ѱ�����ֵĺ����д����ļ��Ӵ�������ʵ�����е�ʱ�������Ҫ�ر���д��

void Preprocess::findnum(Mat* dstImage, Mat* tempImage, Mat* code, Mat* unit, int condition, Point* codeCenter, Point* sfCenter)
{
	if (condition == complex)
	{
		//�Ƚ�������ȸ���
		*tempImage = dstImage->clone();
		//�������Ƕ����Ѱ������
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
				rect[rect_cont] = boundrect[i];//����ʹ��rect(rectsize)
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

		//�����������ҵ������ֵ�Ԫ�������㷨���򵥵Ĳ���
		//����y���ϵ�����ȷ�����еĴ���
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

		//�����Ƕ���x����������Ƕ��������е��������ǽ��������е�����
		//����֮���˳���Ǵ����ң���������
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
						//bug ������������Ϸ���������������������
						//swap(rect[i], rect[j]);
						p_voidptr = p_index[i];
						p_index[i] = p_index[j];
						p_index[j] = p_voidptr;
					}
				}
			}
		}
		//�����Ƕ���code��unit�ĸ�ֵ
		//unit��shenfu[]������Ĳ�����
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

		int stopor = waitKey(delay);//���ӳ�ʱ�����治����ͼ��Ҳ���ǿ���ͼ��ĸ���
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
		// �ӵ� codePic ����
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

	//һϵ�д���
	cv::threshold(rois, th, 50, 255, THRESH_BINARY);
	resize(th, th, Size(20, 30));//��ͼƬresize20*30��С
	Mat matROIFloat;
	th.convertTo(matROIFloat, CV_32FC1);
	Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
	Mat matCurrentChar(0, 0, CV_32F);

	knn->findNearest(matROIFlattenedFloat, 1, matCurrentChar);//����knn�㷨ʶ������

	float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
	strFinalString = strFinalString + char(int(fltCurrentChar));//�������
	int number = atoi(strFinalString.c_str());
	return number;
}

void Shenfu::readTrainData() {
	knn = ml::StatModel::load<ml::KNearest>("KNN_DATA.xml");//��ȡ����ʶ��xml�ļ�
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

// Preprocess ��ʵ��
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
	proornot = true;//���Ϊ�Ѿ��������
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

//ע�⣬����������Ѱ�����ֵĺ����д����ļ��Ӵ�������ʵ�����е�ʱ�������Ҫ�ر���д��
void Preprocess::findnum(Mat* dstImage, Mat* tempImage, Mat* code, Mat* unit, int condition)
{
	if (condition == complex)
	{
		//�Ƚ�������ȸ���
		*tempImage = dstImage->clone();
		//�������Ƕ����Ѱ������
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
		//�����������ҵ������ֵ�Ԫ�������㷨���򵥵Ĳ���
		//����y���ϵ�����ȷ�����еĴ���
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

		//�����Ƕ���x����������Ƕ��������е��������ǽ��������е�����
		//����֮���˳���Ǵ����ң���������
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
		//�����Ƕ���code��unit�ĸ�ֵ
		//unit��shenfu[]������Ĳ�����
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

// Shenfu ��ʵ��
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
		
		int stopor = waitKey(delay);//���ӳ�ʱ�����治����ͼ��Ҳ���ǿ���ͼ��ĸ���
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
		Mat roi(*dstImage, rois[r]);//��RoiͼƬ��dstImage���ҳ�
		std::string strFinalString;
		Mat th;

		//һϵ�д���
		cv::threshold(roi, th, 50, 255, THRESH_BINARY);
		resize(th, th, Size(20, 30));//��ͼƬresize20*30��С
		Mat matROIFloat;
		th.convertTo(matROIFloat, CV_32FC1);
		Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
		Mat matCurrentChar(0, 0, CV_32F);

		knn->findNearest(matROIFlattenedFloat, 1, matCurrentChar);//����knn�㷨ʶ������

		float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
		strFinalString = strFinalString + char(int(fltCurrentChar));//�������
		putText(*tempImage, strFinalString, Point(rois[r].x, rois[r].y), FONT_HERSHEY_SIMPLEX, 1, Scalar(127, 0, 255), 2);//��������ʾ�ڷ�����
	}
}

void Shenfu::readTrainData(){
	knn = ml::StatModel::load<ml::KNearest>("KNN_DATA.xml");//��ȡ����ʶ��xml�ļ�
}

*/
