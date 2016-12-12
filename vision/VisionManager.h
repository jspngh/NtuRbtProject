#include <pthread.h>
#include <vector>
#include <cmath>

#include <libfreenect/libfreenect.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

class VisionManager : public Freenect::FreenectDevice {
	public:
		VisionManager(freenect_context *_ctx, int _index);

		bool getVideo(cv::Mat& output);

        cv::Mat processFrame(cv::Mat frame);

    private:
		std::vector<uint8_t> m_buffer;
		std::vector<uint16_t> m_gamma;
        cv::Mat rgbMat;
        cv::Mat ownMat;
		pthread_mutex_t m_rgb_mutex;
		bool m_new_rgb_frame;

		// Do not call directly
		void VideoCallback(void* _rgb, uint32_t timestamp);

        std::vector< std::vector<cv::Point> > findObjects(cv::Mat mat);
        // cv::Mat findObjects(cv::Mat mat);

        cv::Mat filterStones(cv::Mat raw);
        cv::Mat filterBoard(cv::Mat raw);
};
