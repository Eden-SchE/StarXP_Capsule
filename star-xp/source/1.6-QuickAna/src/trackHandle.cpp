#include <iostream>
#include "trackHandle.h"

using namespace std;
using namespace cv;

TrackHandle::TrackHandle(int Threshold) {
    ADC_Threshold = Threshold;
}

TrackHandle::~TrackHandle() {
    // vector<myTrack>().swap(Vtrack);
}

Mat TrackHandle::S16C1_to_U8C1(Mat S16C1_Array)
{
    double min, max;
    Mat U8C1_Array;
    minMaxIdx(S16C1_Array, &min, &max);
    S16C1_Array.convertTo(U8C1_Array, CV_8UC1, 255/(max-min), -min);

    return U8C1_Array;
}

myTime TrackHandle::time(const short *dataPtr) {
    myTime Ttime;
    struct timespec tn;
    tn.tv_sec = 0;
    tn.tv_nsec = 0;

    for(int i = 0; i < 4; i++) {
        uint64 time_ns = (32768 + dataPtr[i])*pow(65536, 3-i);
        tn.tv_sec += time_ns / 1000000000;
        tn.tv_nsec += time_ns % 1000000000;
    }

    tn.tv_sec += 8*60*60;
    tm *myTm = gmtime(&tn.tv_sec);

    Ttime.nsec = tn.tv_nsec;
    Ttime.sec = myTm->tm_sec;
    Ttime.min = myTm->tm_min;
    Ttime.hour = myTm->tm_hour;
    Ttime.mday = myTm->tm_mday;
    Ttime.mon = myTm->tm_mon;
    Ttime.year = myTm->tm_year;
    Ttime.wday = myTm->tm_wday;
    Ttime.yday = myTm->tm_yday;
    Ttime.isdst = myTm->tm_isdst;

    return Ttime;
}

Mat TrackHandle::Array2Mat(const short *dataPtr, uint32_t index, uint32_t w, uint32_t h) {
    Mat track_img(h, w, CV_16SC1, cv::Scalar(0));
    
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            track_img.at<short>(i, j) =  dataPtr[i * w + j + index];
        }
    }

    track_img.at<short>(0, 0) = 0;
    track_img.at<short>(0, 1) = 0;
    track_img.at<short>(0, 2) = 0;
    track_img.at<short>(0, 3) = 0;

    return track_img;
}

void TrackHandle::length_vector(const short *dataPtr, uint32_t dataSize, vector<int>& lens) {
    vector<int>().swap(lens);
    for(int i = 0; i < dataSize; i+=72*72) {
        Mat frame = Array2Mat(dataPtr, i, 72, 72);
        Mat frame_u8 = S16C1_to_U8C1(frame);
        Mat frame_center = RoiCenter(frame_u8);
        Mat frame_center_blur;
        medianBlur(frame_center, frame_center_blur, 3);
        Mat frame_thinning;
        ximgproc::thinning(frame_center, frame_thinning);
        int frame_len = sum(frame_thinning)[0]/255;
        lens.push_back(frame_len);
    }
}

int TrackHandle::length(Mat frame_center) {
    Mat frame_center_blur;
    medianBlur(frame_center, frame_center_blur, 3);
    Mat frame_thinning;
    ximgproc::thinning(frame_center, frame_thinning);
    int frame_len = sum(frame_thinning)[0]/255;
    
    return frame_len;
}

Mat TrackHandle::RoiCenter(Mat frame, bool isSource) {
    Mat frame_blur;
    GaussianBlur(frame, frame_blur, Size(3, 3), 0);
    Mat frame_binary;
    threshold(frame_blur, frame_binary, 0, 255, cv::THRESH_BINARY);
    Mat frame_close;
    Mat kernel = getStructuringElement(MORPH_RECT,Size(2,2));
    morphologyEx(frame_binary, frame_close, MORPH_CLOSE, kernel);
    vector<vector<cv::Point>> contours;
    vector<cv::Point> max_contours;
    double area = 0.0;
    findContours(frame_close, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
    for(auto &iter:contours) {
        double area_temp = contourArea(iter);
        if(area_temp >= area) {
            area = area_temp;
            max_contours = iter;
        }
    }
    if(area == 0.0) {
        contour_size = 0;
        if(isSource == false) {
            return frame_binary;
        } else {
            return frame;
        }
    }
    
    contour_size = max_contours.size();
    Rect border = boundingRect(max_contours);
    Mat ROI;
    if(isSource == false) {
        Mat track_fill(frame.size(), CV_8UC1, Scalar::all(0));
        std::vector<std::vector<cv::Point>> temp_contours;
        temp_contours.push_back(max_contours);
        drawContours(track_fill, temp_contours, -1, 255, -1);
        ROI = track_fill(border);
    } else {
        ROI = frame_blur(border);
    }
    
    int roi_x, roi_y, roi_w, roi_h;
    roi_x = border.x;
    roi_y = border.y;
    roi_w = border.width;
    roi_h = border.height;

    track_w = roi_w;
    track_h = roi_h;

    int center_y = (72-roi_h)/2;
    int center_x = (72-roi_w)/2;

    Mat track_center(frame.size(), CV_8UC1, Scalar::all(0));
    for(int y = center_y; y < center_y+roi_h; y++) {
        for(int x = center_x; x < center_x+roi_w; x++) {
            track_center.at<uchar>(y, x) = ROI.at<uchar>(y-center_y, x-center_x);
        }
    }

    return track_center;
}

Mat TrackHandle::Hit_Frame(Mat frame) {
    Mat hit_frame;
    threshold(frame, hit_frame, ADC_Threshold, 32767, cv::THRESH_TOZERO);
    return hit_frame;
}

int TrackHandle::Max_Value(Mat frame) {
    double min, max;
    minMaxIdx(frame, &min, &max);

    return max;
}

coordinate TrackHandle::Max_Coor(Mat frame) {
    coordinate Peak;
    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;

    minMaxLoc(frame, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
    Peak.x = maxLoc.x;
    Peak.y = maxLoc.y;

    return Peak;
}

int TrackHandle::Sum_Value(Mat frame) {
    return sum(frame)[0];
}

int TrackHandle::Count_NonZero(Mat frame) {
    return countNonZero(frame);
}

double TrackHandle::Diagonal_Length(int x, int y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}

double TrackHandle::Ratio_XY(int x, int y) {
    return x/y;
}

coordinate TrackHandle::Bcenter_Coor(Mat frame) {
    coordinate Bcenter;
    int cols = frame.cols;
    int rows = frame.rows;
    double sumXV = 0.0;
    double sumYV = 0.0;
    double sumVal = 0.0;
    for(int y = 0; y < rows; y++) {
        for(int x = 0; x < cols; x++) {
            short val = frame.at<short>(y,x);
            if(val != 0) {
                sumXV += val*x;
                sumYV += val*y;
            }
        }
    }
    sumVal = double(sum(frame)[0]);
    Bcenter.x = sumXV / sumVal;
    Bcenter.y = sumYV / sumVal;
    
    return Bcenter;
}

double TrackHandle::Dis_Bcenter_Max(coordinate max_coor, coordinate bcenter_coor) {
    return sqrt(pow(max_coor.x - bcenter_coor.x, 2) + pow(max_coor.y - bcenter_coor.y, 2));
}

int TrackHandle::Contour_Size() {
    return contour_size;
}

void TrackHandle::feature_extraction(const short *dataPtr, uint32_t dataSize, vector<myTrack> &Vtrack) {
    for(int i = 0; i < dataSize; i+=72*72) {
        Mat frame = Array2Mat(dataPtr, i, 72, 72);
        if(countNonZero(frame) < 1) continue;
        Mat frame_hit = Hit_Frame(frame);
        // Mat frame_u8 = S16C1_to_U8C1(frame);
        Mat frame_hit_u8 = S16C1_to_U8C1(frame_hit);
        Mat frame_center_u8 = RoiCenter(frame_hit_u8, false);

        myTrack track;
        int area = track.sumHit;
        int len = track.length;
        double wid = (double) area / len;
        int edgeHitSum = 0;
        int numAdd = 0, numLoss = 0;
        for(int lin = 0; lin < 72; lin ++){
            for(int row = 0; row < 72; row ++){
                //track.data[lin][row] = frame_hit.at<short>(lin,row);
                if(frame_hit.at<short>(lin,row) != 0)
                {
                    if(lin == 0 || lin == 71 || row == 0 || row == 71)
                    {
                        edgeHitSum ++;
                    }
                }
                double subPixel = frame_hit.at<short>(lin,row) - preData[lin][row];
                if(subPixel > 20) numAdd++;
                if(subPixel < -20) numLoss++;
                preData[lin][row] = frame_hit.at<short>(lin,row);
            }
        }
        track.topTime = time(dataPtr + i);
        track.peakADC = Max_Value(frame_hit);
        track.Peak = Max_Coor(frame_hit);
        track.sumADC = Sum_Value(frame);
        track.HitADC = Sum_Value(frame_hit);
        track.sumHit = Count_NonZero(frame_hit);
        track.lenXY = Diagonal_Length(track_w, track_h);
        track.ratioXY = Ratio_XY(track_w, track_h);
        track.Bcenter = Bcenter_Coor(frame_hit);
        track.lenBtoP = Dis_Bcenter_Max(track.Peak, track.Bcenter);
        track.length = length(frame_center_u8);
        track.outline = Contour_Size();
        track.width = wid;
        track.edgeHit = edgeHitSum;

        //------------for data----------------------
        // if((track.sumHit-preHit) > 10 && (track.HitADC - preADC) > 1000)  // condition 1
        // if(numAdd > numLoss)                                                 // condition 2
        // {
        //     numEvent ++;
        //     numFrame = 1;
        // }
        // else
        // {
        //     numFrame ++;
        // }

        //------------for simulation----------------------
        numEvent ++;
        numFrame = 1;

        preADC = track.HitADC;
        preHit = track.sumHit;
        track.numEvent = numEvent;
        track.numFrame = numFrame;

        // cout << "Number " << numEvent << " Event ===" << numAdd << endl;

        track.filename = currentFileName;

        Vtrack.push_back(track);
    }
}


void TrackHandle::calculate(string filename, vector<myTrack> &Vtrack) {
    FILE* fp = fopen(filename.c_str(), "rb");
    if (fp == nullptr) {
        cout << "Error opening file" << endl;
    }
    fseek(fp, 0, 2);
    long long int fsize = ftell(fp);
    fseek(fp, 0, 0);
    cout << "fsize:" << fsize << endl;
    auto dataPtr = new short[fsize/2];
    fread(dataPtr, sizeof(short), fsize/2, fp);
    fclose(fp);

    currentFileName = filename;
    feature_extraction(dataPtr, fsize/2, Vtrack);

    delete[] dataPtr;
}


vector<string> TrackHandle::calculateFromFolder(string folder, vector<myTrack> &Vtrack) {
    vector<string> filePaths;
    glob(folder + "/UDP*", filePaths, false);
    sort(filePaths.begin(), filePaths.end());
    return filePaths;
}
