#include <pthread.h>
#include <utility>
#include <list>
#include <cmath>

#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "../board.hpp"
#include "KinectManager.h"

#define REGION_THRESHOLD 300

struct Stone
{
    int x;
    int y;
    bool yellow;

    bool operator<(const Stone& rhs) const
    {
        return y < rhs.y;
    }

    bool operator>(const Stone& rhs) const
    {
        return y > rhs.y;
    }

    bool operator==(const Stone& rhs) const
    {
        return y == rhs.y;
    }
};

struct BoardEdge
{
    int x;
    int y;
    int score;

    bool operator<(const BoardEdge& rhs) const
    {
        return x < rhs.x;
    }

    bool operator>(const BoardEdge& rhs) const
    {
        return x > rhs.x;
    }

    bool operator==(const BoardEdge& rhs) const
    {
        return x == rhs.x;
    }
};

class VisionManager{
    public:
        VisionManager();
        VisionManager(KinectManager *km);

        bool initVision();
        void stopVision();

        bool updateBoard(Board& board);

        bool getVideo(cv::Mat& output);
        void processFrame(cv::Mat frame, State** output);

    private:
        KinectManager *kinectManager;
        std::pair<BoardEdge,BoardEdge> boardLocation;

        std::list<Stone> findStones(cv::Mat raw);
        std::pair<BoardEdge,BoardEdge> findBoardEdges(cv::Mat raw);

        bool grow_region(cv::Mat& m, int x, int y, int reg_nr);
        Stone getRegionCentroid(cv::Mat& m, int reg_nr, bool yellow);
};
