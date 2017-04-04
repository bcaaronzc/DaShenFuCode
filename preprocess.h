#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "findnumber.h"

//����ͷ����Ƶ�ߴ���480*640����*����
//Ŀǰ2016�������ߴ���1550*710��mm������*�ߣ�
//Ŀǰ��Ԫ�ַ���complex���㷽���ǻ�ȡһ����Ԫ�ַ��ĳ����������������Ļ������,���������-3*��Ԫ�ַ�����/3�����ַ�֮��ļ���ˣ�
//��Ȼ��Ҫ����ʵ�ʴ�����ĳߴ�������
//2 2783 

using namespace cv;
using std::cout;
using std::cin;
using std::endl;

const int codesize = 5;
const int sfsize = 9;

class Preprocess
{
private:
	bool proornot;
	enum type { simple, complex };
	//int getrowvalue(Mat *srcImage);//˽�г�Ա�������ж��е�ֵ
	//int getcolvalue(Mat *srcImage);//�ж��е�ֵ
	int x, y;//������ж�ʹ�õ�����
	int rowlength, collength;//�ж�complexʱʹ�õ�ROI����ĳ���
	int step;//complexʱѰ�ҵĲ���
	int height, width;//findnum���ҵ���һ����Ԫ�ַ��ĳ���
	int rectsize;
public:
	Preprocess();
	~Preprocess();
	Mat process(Mat*srcImage);//����֡
	void openprovideo(Mat* dstImage);//�򿪴��������Ƶ�ĵ�֡
	void findnum(Mat*dstImage, Mat*tempImage, Mat*code, Mat*unit, int condition, Point* codeCenter, Point* sfCenter);	//����һ����Ԫ��Ĵ�С������ģʽ��comlpexΪС��Χ���ң�simple�Ǵ�Χ����
																														//simple�ǲ����������������κ������ģ���Ȼ���ܵ��»������⣬��������ʱ���
																														//complex�������ķ���ѡȡROI��ɵģ�����ʱ�䳤���Ǹ��ܿ˷���Ե����
	//int* getinf();//�����Ѿ������һ����Ԫ��ĳ����
	//
	Point getCenterPoint(Rect* rect);
	//
};

class Shenfu
{
private:
	Mat srcImage, dstImage;
	Mat code[codesize];//����������飬5��
	Mat shenfu[sfsize];//����Ź������飬9��
	// 
	Point codeCenter[codesize];
	Point sfCenter[sfsize];
	//
	std::string codeNum[codesize];//����code�����飬5��
	std::string shenfuNum[sfsize];//����shenfu�����飬9��
	Mat tempImage;//������Ϊ�м���ȵ�ͼ
	Preprocess pro;
	VideoCapture cap;
	bool istest, controlpoint;//���Ʋ��Եİ�ť��һ���������
	double delay;//�ӳ�
	enum type { simple, complex };
	int* p_unitsize;
	std::vector<Rect>* p_rect;
	std::string strFinalString; //�����������
	Ptr<ml::KNearest>  knn;//knnʶ��
	findnumber sfPic[sfsize];
	// ����һ������� findnumber ����
	findnumber codePic[codesize];
	//
public:
	Shenfu() { istest = false; controlpoint = false; delay = 30; p_unitsize = nullptr; p_rect = nullptr; }//�����ӳ�
	~Shenfu() { cap.release(); }
	void opentest(bool condition);//�Ƿ�������ģʽ
	void startpreprocess();//�Ƿ�ʼ����
	void showROI();//����ROI����ĺ���
				   //����ĺ�������ϣ���Ҫ����ע��
	int findDigit(Mat rois);//ʶ����ʾ����
	void readTrainData();//��ȡ����ʶ��ѵ���ļ�
	//void openpicture();
	//
	Point returnHitPoint(findnumber code);
	//
};
#endif