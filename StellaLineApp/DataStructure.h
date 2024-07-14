#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <windows.h>
#include <vector>
#include <map>


#define CIRCLE_SIZE 10
#define LINE_SIZE 1

//线段结构体
typedef struct
{
    cv::Point _point;         //线段起始点
    int _pointsize = 1;       //线段粗细度
}DrawLineInfo;

//单条车道线容器
std::vector<DrawLineInfo> SingleLine{};

//绘制图片线段总容器
std::map<int, std::vector<DrawLineInfo>> AllLine{};

//鼠标移动点位定位容器
std::vector<cv::Point> MovepointVector;


//绘制图形
void DrawPixMethod(cv::Mat _drawpix,cv::Point _startpoint,cv::Point _endpoint)
{
    cv::line(_drawpix,_startpoint,_endpoint,cv::Scalar(0,255,255),LINE_SIZE, cv::LINE_AA);
    cv::circle(_drawpix, _startpoint, CIRCLE_SIZE, cv::Scalar(0,100,255), -1);
    cv::circle(_drawpix, _endpoint, CIRCLE_SIZE, cv::Scalar(0,100,255), -1);
}

#endif // DATASTRUCTURE_H