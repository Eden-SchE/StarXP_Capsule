#ifndef TRACKHANDLE_H
#define TRACKHANDLE_H

#include <cstdio>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>

#include "trackStruct.h"

using namespace std;
using namespace cv;


class TrackHandle {
    private:
        int ADC_Threshold;
        // myTrack track;
        // vector<myTrack> Vtrack;

        int track_w, track_h, contour_size;
        int preADC = 0; 
        int numEvent = 0;
        int preHit = 0;
        int numFrame = 0;
        int preData[72][72] = {0};
        string currentFileName;

        Mat Array2Mat(const short *dataPtr, uint32_t index, uint32_t w, uint32_t h);
        Mat S16C1_to_U8C1(Mat S16C1_Array);
        Mat RoiCenter(Mat frame, bool isSource = false);
        int Max_Value(Mat frame);
        coordinate Max_Coor(Mat frame);
        int Sum_Value(Mat frame);
        int Count_NonZero(Mat frame);
        double Diagonal_Length(int x, int y);
        double Ratio_XY(int x, int y);
        coordinate Bcenter_Coor(Mat frame);
        double Dis_Bcenter_Max(coordinate max_coor, coordinate bcenter_coor);
        Mat Hit_Frame(Mat frame);
        int Contour_Size();
        int length(Mat frame_center);
        myTime time(const short *dataPtr);

        void feature_extraction(const short *dataPtr, uint32_t dataSize, vector<myTrack> &Vtrack);

    public:
        TrackHandle(int Threshold);
        ~TrackHandle();

        void length_vector(const short *dataPtr, uint32_t dataSize, vector<int>& numbers);

        void calculate(string filename, vector<myTrack> &Vtrack);
        vector<string> calculateFromFolder(string folder, vector<myTrack> &Vtrack);
};

#endif