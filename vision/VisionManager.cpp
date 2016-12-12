#include <iostream>

#include "VisionManager.h"

// #define SHOW_DEBUG

using namespace cv;
using namespace std;

void DEBUG(string msg)
{
#ifdef SHOW_DEBUG
    cout << msg << endl;
#endif
}

VisionManager::VisionManager(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index),
    m_buffer(FREENECT_VIDEO_RGB),
    m_gamma(2048), m_new_rgb_frame(false),
    rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
    ownMat(Size(640,480),CV_8UC3,Scalar(0)) {

        pthread_mutex_init( &m_rgb_mutex, NULL );
        for( unsigned int i = 0 ; i < 2048 ; i++) {
            float v = i/2048.0;
            v = std::pow(v, 3)* 6;
            m_gamma[i] = v*6*256;
        }
    }

void VisionManager::VideoCallback(void* _rgb, uint32_t timestamp) {
    pthread_mutex_lock(&m_rgb_mutex);
    DEBUG("video callback");
    uint8_t* rgb = static_cast<uint8_t*>(_rgb);
    rgbMat.data = rgb;
    m_new_rgb_frame = true;
    pthread_mutex_unlock(&m_rgb_mutex);
};

bool VisionManager::getVideo(Mat& output) {
    pthread_mutex_lock(&m_rgb_mutex);
    DEBUG("getting frame");
    if(m_new_rgb_frame) {
        cvtColor(rgbMat, output, CV_RGB2BGR);
        m_new_rgb_frame = false;
        pthread_mutex_unlock(&m_rgb_mutex);
        return true;
    } else {
        pthread_mutex_unlock(&m_rgb_mutex);
        return false;
    }
}

vector< vector<Point> > VisionManager::findObjects(Mat gray)
{
    Mat canny_output;
    vector< vector<Point> > objects;
    vector<Vec4i> hierarchy;

    Canny(gray, canny_output, 50, 150, 3);
    dilate(canny_output, canny_output, Mat());
    erode(canny_output, canny_output, Mat());
    findContours(gray, objects, hierarchy, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    return objects;
}

Mat VisionManager::filterStones(Mat raw)
{
    Mat result = Mat::zeros(raw.size(), CV_8UC1);
    for (int i=0; i < raw.size().width; i++)
    {
        for (int j=0; j < raw.size().height; j++)
        {
            Vec3b bgr_val = raw.at<Vec3b>(j,i);
            if (bgr_val[0] > 100)
            {
                result.at<uchar>(j,i) = 255;
            }
        }
    }
    return result;
}

Mat VisionManager::filterBoard(Mat raw)
{
    Mat result = Mat::zeros(raw.size(), CV_8UC1);
    for (int i=0; i < raw.size().width; i++)
    {
        for (int j=0; j < raw.size().height; j++)
        {
            Vec3b bgr_val = raw.at<Vec3b>(j,i);
            if (bgr_val[0] > 100 && bgr_val[1] < 110 && bgr_val[2] < 110)
            {
                result.at<uchar>(j,i) = 255;
            }
        }
    }

    // Mat canny_output;
    // vector< vector<Point> > objects;
    // vector<Vec4i> hierarchy;

    // Canny(result, canny_output, 50, 150, 3);
    // dilate(canny_output, canny_output, Mat());
    // erode(canny_output, canny_output, Mat());
    return result;
}

Mat VisionManager::processFrame(Mat frame)
{
    DEBUG("processing frame");
    // Mat frameHSV(Size(640,480), CV_8UC3, Scalar(0));
    // Mat redMaskL = Mat::zeros(frame.size(), CV_8UC1);
    // Mat redMaskH = Mat::zeros(frame.size(), CV_8UC1);

    // Mat frameYellow = Mat::zeros(frame.size(), CV_8UC1);
    // Mat frameRed = Mat::zeros(frame.size(), CV_8UC1);
    // Mat frameBlue = Mat::zeros(frame.size(), CV_8UC1);

    // cvtColor(frame, frameHSV, CV_BGR2HSV);

    // // find the yellow stones
    // inRange(frameHSV, Scalar(20, 150, 150), Scalar(30, 255, 255), frameYellow);
    // // find the red stones
    // inRange(frameHSV, Scalar(0, 150, 100), Scalar(10, 255, 255), redMaskL);
    // inRange(frameHSV, Scalar(170, 150, 100), Scalar(180, 255, 255), redMaskH);
    // frameRed = redMaskL | redMaskH;
    // // find the blue board
    // inRange(frameHSV, Scalar(60, 80, 80), Scalar(160, 255, 255), frameBlue);


    // vector< vector<Point> > contours = findObjects(frameBlue);
    // Scalar color_black = Scalar(255,255,0);
    // Mat drawing = Mat::zeros(frame.size(), CV_8UC3);
    // for( int i = 0; i< contours.size(); i++ )
    // {
    //     drawContours(drawing, contours, i, color_black, CV_FILLED);
    // }


    // return frameRed | frameYellow | frameBlue;
    return filterBoard(frame);

//     vector<Mat> splited_frame;
//     split(frame, splited_frame);
//     Mat drawing = Mat::zeros(frame.size(), CV_8UC3);
//     for (size_t i = 0; i < splited_frame.size(); i++)
//     {
//         threshold(splited_frame[i], splited_frame[i], 150, 255, cv::THRESH_BINARY);
//         vector<vector<Point>> contours = findObjects(splited_frame[i]);
//         Scalar color_black = Scalar(255,255,255);
//         for (int i = 0; i<contours.size(); i++)
//             drawContours(drawing, contours,i, color_black, CV_FILLED);
//     }

//     return drawing;
}


int main(int argc, char **argv) {
    bool die(false);

    Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
    Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

    Freenect::Freenect freenect;
    DEBUG("creating device");
    VisionManager& device = freenect.createDevice<VisionManager>(0);

    namedWindow("rgb",CV_WINDOW_AUTOSIZE);
    DEBUG("starting video");
    device.startVideo();
    while (!die) {
        device.getVideo(rgbMat);
        Mat test = device.processFrame(rgbMat);
        cv::imshow("rgb", test);
        char k = cvWaitKey(1000);
        if( k == 27 ){
            cvDestroyWindow("rgb");
            break;
        }
    }

    device.stopVideo();
    return 0;
}
