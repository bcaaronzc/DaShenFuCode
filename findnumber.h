#pragma once
#ifndef FINDNUMBER_H_
#define FINDNUMBER_H_

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;

class findnumber
{
public:
	int number;
	Mat picture;
	float distance;
	// �洢���ĵ㣨���λ�ã�
	Point centerPoint;
	//
};

#endif