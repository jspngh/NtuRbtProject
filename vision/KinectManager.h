#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <libfreenect/libfreenect.hpp>

class KinectManager : public Freenect::FreenectDevice {
	public:
		KinectManager(freenect_context *_ctx, int _index);
		bool getVideo(cv::Mat& output);

    private:
		std::vector<uint8_t> m_buffer;
		std::vector<uint16_t> m_gamma;
        cv::Mat rgbMat;
        cv::Mat ownMat;
		pthread_mutex_t m_rgb_mutex;
		bool m_new_rgb_frame;

		// Do not call directly
		void VideoCallback(void* _rgb, uint32_t timestamp);
};
