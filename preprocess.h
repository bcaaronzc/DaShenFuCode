#ifndef PREPROCESS_H_
#define PREPROCESS_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "findnumber.h"

//摄像头的视频尺寸是480*640（高*长）
//目前2016年大神符尺寸是1550*710（mm）（长*高）
//目前单元字符的complex计算方法是获取一个单元字符的长宽，用整个大神符屏幕来计算,（大神符长-3*单元字符长）/3就是字符之间的间距了，
//当然，要根据实际大神符的尺寸来调试
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
	//int getrowvalue(Mat *srcImage);//私有成员函数，判断行的值
	//int getcolvalue(Mat *srcImage);//判断列的值
	int x, y;//定义的判断使用的中心
	int rowlength, collength;//判断complex时使用的ROI区域的长宽
	int step;//complex时寻找的步长
	int height, width;//findnum所找到的一个单元字符的长宽
	int rectsize;
public:
	Preprocess();
	~Preprocess();
	Mat process(Mat*srcImage);//处理单帧
	void openprovideo(Mat* dstImage);//打开处理过的视频的单帧
	void findnum(Mat*dstImage, Mat*tempImage, Mat*code, Mat*unit, int condition, Point* codeCenter, Point* sfCenter);	//运算一个单元格的大小，两种模式，comlpex为小范围查找，simple是大范围查找
																														//simple是不允许在中心行有任何噪音的，不然可能导致划分问题，但是运行时间短
																														//complex是在中心反复选取ROI完成的，运行时间长但是更能克服边缘噪音
	//int* getinf();//返回已经运算的一个单元格的长与宽
	//
	Point getCenterPoint(Rect* rect);
	//
};

class Shenfu
{
private:
	Mat srcImage, dstImage;
	Mat code[codesize];//神符密码数组，5个
	Mat shenfu[sfsize];//神符九宫格数组，9个
	// 
	Point codeCenter[codesize];
	Point sfCenter[sfsize];
	//
	std::string codeNum[codesize];//储存code的数组，5个
	std::string shenfuNum[sfsize];//储存shenfu的数组，9个
	Mat tempImage;//用来作为中间过度的图
	Preprocess pro;
	VideoCapture cap;
	bool istest, controlpoint;//控制测试的按钮，一件开启监控
	double delay;//延迟
	enum type { simple, complex };
	int* p_unitsize;
	std::vector<Rect>* p_rect;
	std::string strFinalString; //最终输出数字
	Ptr<ml::KNearest>  knn;//knn识别
	findnumber sfPic[sfsize];
	// 加了一个密码的 findnumber 数组
	findnumber codePic[codesize];
	//
public:
	Shenfu() { istest = false; controlpoint = false; delay = 30; p_unitsize = nullptr; p_rect = nullptr; }//计算延迟
	~Shenfu() { cap.release(); }
	void opentest(bool condition);//是否开启测试模式
	void startpreprocess();//是否开始处理
	void showROI();//监视ROI区域的函数
				   //后面的函数请加上，不要忘了注释
	int findDigit(Mat rois);//识别并显示数字
	void readTrainData();//读取数字识别训练文件
	//void openpicture();
	//
	Point returnHitPoint(findnumber code);
	//
};
#endif