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
	// 存储中心点（打击位置）
	Point centerPoint;
	//
};

#endif