#ifndef TRACKSTRUCT_H
#define TRACKSTRUCT_H
#include <vector>

using namespace std;

struct coordinate
{
    double x;
    double y;
};

struct myTime
{
    int nsec;   /** ns，范围从 0 到 1000000000 */
    int sec;    /** 秒，范围从 0 到 59 */
    int min;    /** 分，范围从 0 到 59 */
    int hour;   /** 小时，范围从 0 到 23 */
    int mday;   /** 一月中的第几天，范围从 1 到 31 */
    int mon;    /** 月份，范围从 0 到 11 */
    int year;   /** 自 1900 起的年数 */
    int wday;   /** 一周中的第几天，范围从 0 到 6 */
    int yday;   /** 一年中的第几天，范围从 0 到 365 */
    int isdst;  /** 夏令时 */   
};

struct myTrack
{
    //int data[72][72];   /** 每帧图像数据 */
    //int sumEvent;       /** 总事例数 */
    int numEvent;       /** 事例编号 */
    //int sumFrame;       /** 单事例帧数 */
    int numFrame;       /** 单事例帧编号 */
    myTime topTime;     /** 时间信息 */
    int peakADC;        /** 每帧ADC最大值 */
    coordinate Peak;    /** 每帧ADC最大点XY值 */
    int sumADC;         /** 每帧ADC总和 */
    int HitADC;         /** 着火点ADC总和 */
    int sumHit;         /** 每帧着火点个数 */
    double lenXY;       /** 每帧径迹在XY方向的长度 */
    double ratioXY;     /** 每帧径迹在XY方向的比例 */
    coordinate Bcenter; /** 每帧重心点XY */
    double lenBtoP;     /** 每帧重心点与ADC最大值的距离 */
    //double mom2d;       /** 图像二阶矩 */
    //double mom3d;       /** 图像三阶矩 */
    int length;         /** 径迹长度 */
    double width;       /** 径迹宽度 */
    int outline;        /** 径迹轮廓大小 */
    int edgeHit;        /** 边缘着火点个数 */
    double ellip;       /**径迹椭率*/
    double circ;        /**径迹椭率*/
    string filename;    /** 文件名 */
};
#endif