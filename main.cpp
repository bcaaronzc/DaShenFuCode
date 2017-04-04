#include "preprocess.h"

int main()
{
	int choice = 0;
	Shenfu ex_shenfu;
	//ex_shenfu.openpicture();
	ex_shenfu.readTrainData();//读取数字识别文件
	ex_shenfu.opentest(true);
	do
	{
		ex_shenfu.startpreprocess();
		choice = waitKey(2000);//两秒内按下esc结束，否则重新开始循环，读帧
	} while (choice != 27);
	do
	{
		ex_shenfu.showROI();
		choice = waitKey(1000);
	} while (choice != 27);
	waitKey(0);
	return 0;
}