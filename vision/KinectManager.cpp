#include "KinectManager.h"
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

KinectManager::KinectManager(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index),
    m_buffer(FREENECT_VIDEO_RGB),
    m_gamma(2048), m_new_rgb_frame(false),
    rgbMat(Size(640,480), CV_8UC3, Scalar(0)),
    ownMat(Size(640,480),CV_8UC3,Scalar(0))
{
        pthread_mutex_init( &m_rgb_mutex, NULL );
        for( unsigned int i = 0 ; i < 2048 ; i++) {
            float v = i/2048.0;
            v = std::pow(v, 3)* 6;
            m_gamma[i] = v*6*256;
        }
}
bool KinectManager::getVideo(cv::Mat& output)
{
    pthread_mutex_lock(&m_rgb_mutex);
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

void KinectManager::VideoCallback(void* _rgb, uint32_t timestamp)
{
    pthread_mutex_lock(&m_rgb_mutex);
    uint8_t* rgb = static_cast<uint8_t*>(_rgb);
    rgbMat.data = rgb;
    m_new_rgb_frame = true;
    pthread_mutex_unlock(&m_rgb_mutex);
}
