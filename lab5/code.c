#include "opencv2/opencv.hpp"
#include <time.h>
using namespace cv;
using namespace std;

Mat GetNormalImage(Mat & src)
{
    return src;
}

Mat GetSmoothImage(Mat & src)
{
    Mat bluredImg;
    blur(src, bluredImg, Size(15, 15), Point(-1, -1));
    return bluredImg;
}

Mat ChangeColors(Mat& src)
{
    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);
    Mat mask;
    inRange(hsv, Scalar(82, 25, 0), Scalar(161, 255, 255), mask);
    src.setTo(Scalar(0, 0, 200), mask);
    return src;
}

Mat RotateImage(Mat& src)
{
    flip(src, src, 0);
    return src;
}

enum Status
{
    NORMAL,
    SMOOTH,
    CHANGEDCOLORS,
    CHANGEROTATE
};

int main()
{
    VideoCapture cam(0);
    Status status = NORMAL;

    clock_t dTime1, dTime2;
    dTime1 = clock();
    unsigned numFramesCaptured = 0;
    while (cam.isOpened())
    {
        Mat img;

        clock_t startInterval = clock();

        clock_t rTime1, rTime2;
        rTime1 = clock();
        cam >> img;
        rTime2 = clock();

        resize(img, img, Size(720,480));
        flip(img, img, 1);

        clock_t tTime1, tTime2;
        tTime1 = clock();
        if (status == NORMAL) img = GetNormalImage(img);
        else if (status == SMOOTH) img = GetSmoothImage(img);
        else if (status == CHANGEDCOLORS) img = ChangeColors(img);
        else if (status == CHANGEROTATE) img = RotateImage(img);
        tTime2 = clock();
        
        clock_t sTime1, sTime2;
        sTime1 = clock();
        imshow("camera", img);
        sTime2 = clock();

        auto pressedKey = waitKey(30);

        if (pressedKey == 49) status = NORMAL;
        else if (pressedKey == 50) status = CHANGEROTATE;
        else if (pressedKey == 51) status = SMOOTH;
        else if (pressedKey == 52) status = CHANGEDCOLORS;
        else if (pressedKey == 27) break;

        clock_t endInterval = clock();

        numFramesCaptured++;
        dTime2 = clock();
        auto diffTime = dTime2 - dTime1;
        auto intervalTime = endInterval - startInterval;
        if (diffTime >= CLOCKS_PER_SEC)
        {
            double curFPS = numFramesCaptured * CLOCKS_PER_SEC / diffTime;
            double readingTime = (double)(rTime2 - rTime1) * 100 / intervalTime;
            double transformationTime = (double)(tTime2 - tTime1) * 100 / intervalTime;
            double showingTime = (double)(sTime2 - sTime1) * 100 / intervalTime;
            cout << "Reading time: " << readingTime << "%, Transformation time: " << transformationTime << "%, Showing time: " << showingTime << "%, FPS = " << curFPS << endl;
            cout << intervalTime << " " << rTime2 - rTime1 << " " << tTime2 - tTime1 << " " << sTime2 - sTime1 << endl;
            numFramesCaptured = 0;
            dTime1 = clock();
        }
    }
}
