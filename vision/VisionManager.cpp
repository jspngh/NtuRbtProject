#include <unistd.h>
#include <iostream>
#include <string>
#include <stack>

#include "VisionManager.h"

// #define SHOW_DEBUG
#define SHOW_INFO
#define SHOW_WARN

#define ROBOT_ARM_I
#define USE_KINECT
//#define BUILD_SEPERATE

using namespace cv;
using namespace std;

void DEBUG(string msg)
{
#ifdef SHOW_DEBUG
    cout << "DEBUG: " << msg << endl;
#endif
}

void INFO(string msg)
{
#ifdef SHOW_INFO
    cout << "INFO: " << msg << endl;
#endif
}

void WARN(string msg)
{
#ifdef SHOW_WARN
    cout << "WARN: " << msg << endl;
#endif
}

VisionManager::VisionManager()
{
    this->kinectManager = NULL;
}

VisionManager::VisionManager(KinectManager *km)
{
    this->kinectManager = km;
}

bool VisionManager::initVision()
{
    kinectManager->startVideo();
    Mat tmpMat(Size(640,480),CV_8UC3,Scalar(0));
    bool init_success = false;
    for (int i=0; i<10 && !init_success; i++)
    {
        init_success = getVideo(tmpMat);
        usleep(1000000);
    }

    return init_success;
}

void VisionManager::stopVision()
{
    kinectManager->stopVideo();
}

bool VisionManager::getVideo(Mat& output)
{
    if (kinectManager == NULL)
        return false;

    return kinectManager->getVideo(output);
}

bool VisionManager::grow_region(Mat& m, int y, int x, int i)
{
    int h = m.rows; // height
    int w = m.cols; // width

    stack<pair<int, int>> region;
    region.push(make_pair(-1, -1));
    pair<int, int> current_pos(y, x);

    int region_size = 0;

    while(!region.empty())
    {
        int k = current_pos.first;
        int j = current_pos.second;

        //top
        if (k > 0 && m.at<uchar>(k-1,j) == 255)
        {
            region.push(make_pair(k-1, j));
            m.at<uchar>(k-1,j) = i;
            region_size++;
        }
        //bottom
        if (k < (h-1) && m.at<uchar>(k+1,j) == 255)
        {
            region.push(make_pair(k+1, j));
            m.at<uchar>(k+1,j) = i;
            region_size++;
        }

        //right
        if (j < (w-1) && m.at<uchar>(k,j+1) == 255)
        {
            region.push(make_pair(k, j+1));
            m.at<uchar>(k,j+1) = i;
            region_size++;
        }

        //left
        if (j > 0 && m.at<uchar>(k,j-1) == 255)
        {
            region.push(make_pair(k, j-1));
            m.at<uchar>(k,j-1) = i;
            region_size++;
        }

        current_pos = region.top();
        region.pop();
        if(current_pos.first == -1 || current_pos.second == -1)
            break;
    }

    // if the region is too small, ignore it
    if (region_size < REGION_THRESHOLD)
    {
        // DEBUG("region only has size: " + to_string(region_size));
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                if (m.at<uchar>(y,x) == i)
                    m.at<uchar>(y,x) = 0;
            }
        }
        return false;
    }
    return true;
}

Stone VisionManager::getRegionCentroid(Mat& m, int reg_nr, bool yellow)
{
    int A = 0, mX = 0, mY = 0;

    for (int k = 0; k < m.rows; k++) // height
    {
        for (int j = 0; j < m.cols; j++) // width
        {
            if (m.at<uchar>(k,j) == reg_nr)
            {
                A++;
                mX += j;
                mY += k;
            }
        }
    }
    Stone result = {mX/A, mY/A, yellow};
    return result;
}

list<Stone> VisionManager::findStones(Mat raw)
{
    INFO("finding all stones");
    list<Stone> result;

    Mat redMaskL = Mat::zeros(raw.size(), CV_8UC1);
    Mat redMaskH = Mat::zeros(raw.size(), CV_8UC1);

    Mat frameYellow = Mat::zeros(raw.size(), CV_8UC1);
    Mat frameRed = Mat::zeros(raw.size(), CV_8UC1);

    Mat processedFrame = Mat::zeros(raw.size(), CV_8UC3);

#ifdef ROBOT_ARM_I
    // find yellow
    inRange(raw, Scalar(20, 76, 102), Scalar(35, 204, 255), frameYellow);
    // find red
    inRange(raw, Scalar(0, 102, 66), Scalar(10, 240, 204), redMaskL);
    inRange(raw, Scalar(165, 102, 66), Scalar(180, 240, 204), redMaskH);
    frameRed = redMaskL | redMaskH;
#else
    INFO("working with robot arm II");
    // find yellow
    inRange(raw, Scalar(20, 76, 102), Scalar(35, 255, 255), frameYellow);
    // find red
    inRange(raw, Scalar(0, 102, 66), Scalar(10, 255, 204), redMaskL);
    inRange(raw, Scalar(165, 102, 66), Scalar(180, 255, 204), redMaskH);
    frameRed = redMaskL | redMaskH;
#endif

#ifdef BUILD_SEPERATE
    imshow("stones", frameRed | frameYellow);
    // cv::imwrite("/home/nemphis/documents/robotics/project/vision/images/stones_debug.jpg", frameRed | frameYellow);
#endif

    int region_y = 100;
    int region_r = 100;
    for (int k = 0; k < frameYellow.rows; k++) // height
    {
        for (int j = 0; j < frameYellow.cols; j++) // width
        {
            if (frameYellow.at<uchar>(k,j) == 255)
            {
                bool valid_region = grow_region(frameYellow, k, j, region_y);
                if (valid_region)
                {
                    Stone s = getRegionCentroid(frameYellow, region_y, true);
                    result.push_back(s);
                    // INFO("region found, with centroid at " + to_string(s.x) + " " + to_string(s.y));
                    region_y = (region_y + 10) % 256;
                }
            }

            if (frameRed.at<uchar>(k,j) == 255)
            {
                bool valid_region = grow_region(frameRed, k, j, region_r);
                if (valid_region)
                {
                    Stone s = getRegionCentroid(frameRed, region_r, false);
                    result.push_back(s);
                    // INFO("region found, with centroid at " + to_string(s.x) + " " + to_string(s.y));
                    region_r = (region_r + 10) % 256;
                }
            }
        }
    }

    return result;
}

pair<BoardEdge,BoardEdge> VisionManager::findBoardEdges(Mat raw)
{
    pair<BoardEdge,BoardEdge> result;
    result.first.x = -1;
    result.second.x = -1;
    Mat frameBlue = Mat::zeros(raw.size(), CV_8UC1);

    // find blue
#ifdef ROBOT_ARM_I
    inRange(raw, Scalar(95, 63, 76), Scalar(120, 230, 230), frameBlue);
#else
    inRange(raw, Scalar(95, 63, 50), Scalar(120, 255, 230), frameBlue);
#endif

    // find the vertical edges of the board
    list<BoardEdge> edgeList;
    int rectWidth = 5;
    int rectHeight = 100;
    for (int i = rectWidth; i < raw.cols - rectWidth; i+=10)
    {
        for (int j = 0; j < raw.rows - rectHeight; j+=10)
        {
            int currScore = 0;
            for (int di = -rectWidth; di <= rectWidth; di++)
            {
                for (int dj = 0; dj <= rectHeight; dj++)
                {
                    int blue = (frameBlue.at<uchar>(j+dj,i+di) == 255) ? 1 : -1;
                    int tmp = di < 0 ? -1 : 1;
                    currScore += tmp * blue;
                }
            }
            if (abs(currScore) > 350)
            {
                BoardEdge newEdge = {i, j, abs(currScore)};
                edgeList.push_back(newEdge);
            }
        }
    }

    // sort list on scores
    edgeList.sort();
    edgeList.reverse();

    // find the horizontal top edge of the board
    list<BoardEdge> topBorderList;
    rectWidth = 100;
    rectHeight = 5;
    for (int i = rectWidth; i < raw.cols - rectWidth; i+=10)
    {
        for (int j = 0; j < raw.rows - rectHeight; j+=10)
        {
            int currScore = 0;
            for (int di = 0; di <= rectWidth; di++)
            {
                for (int dj = -rectHeight; dj <= rectHeight; dj++)
                {
                    int blue = (frameBlue.at<uchar>(j+dj,i+di) == 255) ? 1 : -1;
                    int tmp = dj < 0 ? -1 : 1;
                    currScore += tmp * blue;
                }
            }
            if (abs(currScore) > 350)
            {
                BoardEdge newBorder = {i, j, abs(currScore)};
                topBorderList.push_back(newBorder);
            }
        }
    }

    list<BoardEdge>::iterator it = topBorderList.begin();
    topBorder = *it;
    it++;
    for (; it!=topBorderList.end(); it++)
    {
        if (it->y < topBorder.y)
            topBorder = *it;
    }


    INFO("number of edges: " +  to_string(edgeList.size()));

    it = edgeList.begin();
    if(it==edgeList.end())
        return result;

    result.first = {it->x,it->y,it->score};
    it = edgeList.end();
    it--;
    result.second = {it->x,it->y,it->score};

    if (result.second.x < 0)
    {
        WARN("only one edge found");
        return result;
    }

    DEBUG("first: " + to_string(result.first.score) + " at x " + to_string(result.first.x) +" at y " + to_string(result.first.y));
    DEBUG("second: " + to_string(result.second.score) + " at x " + to_string(result.second.x) +" at y " + to_string(result.second.y));

#ifdef BUILD_SEPERATE
    for (int j=result.first.y; j < frameBlue.rows; j++)
    {
        frameBlue.at<uchar>(j,result.first.x) = 255;
        frameBlue.at<uchar>(j,result.second.x) = 255;
    }
    for (int i=0; i < frameBlue.cols; i++)
    {
        frameBlue.at<uchar>(topBorder.y,i) = 255;
    }
    cv::imshow("board",frameBlue);
    // cv::imwrite("/home/nemphis/documents/robotics/project/vision/images/board_debug.jpg", frameBlue);
#endif

    return result;
}

void VisionManager::processFrame(Mat frame, State** output)
{
    DEBUG("processing frame");
    Mat frameHSV(Size(640,480), CV_8UC3, Scalar(0));
    cvtColor(frame, frameHSV, CV_BGR2HSV);
    list<Stone> stones = findStones(frameHSV);
    pair<BoardEdge,BoardEdge> boardEdges = findBoardEdges(frameHSV);

#ifdef SHOW_DEBUG
    Mat processedFrame = Mat::zeros(frame.size(), CV_8UC1);

    if (boardEdges.first.x > 0 & boardEdges.second.x > 0)
    {
        for (int j=0; j < frame.rows; j++)
        {
            processedFrame.at<uchar>(j,boardEdges.first.x) = 255;
            processedFrame.at<uchar>(j,boardEdges.second.x) = 255;
        }
    }

    for (list<Stone>::iterator it = stones.begin(); it != stones.end(); it++)
    {
        processedFrame.at<uchar>(it->y, it->x) = 255;
        processedFrame.at<uchar>(it->y+1, it->x) = 255;
        processedFrame.at<uchar>(it->y-1, it->x) = 255;
        processedFrame.at<uchar>(it->y, it->x+1) = 255;
        processedFrame.at<uchar>(it->y, it->x-1) = 255;
    }
    imshow("processed", processedFrame);
#endif

    BoardEdge left, right;
    left = (boardEdges.first.x < boardEdges.second.x) ? boardEdges.first : boardEdges.second;
    right = (boardEdges.first.x > boardEdges.second.x) ? boardEdges.first : boardEdges.second;
    int columnWidth = (right.x - left.x) / 7;

    list<Stone> columns[7];
    for (list<Stone>::iterator it = stones.begin(); it != stones.end(); it++)
    {
        if (it->x < left.x)
        {
            WARN("detected object that was not a stone");
            continue;
        }

        for (int i=1; i<=7; i++)
        {
            if (it->x < left.x + i * columnWidth &&
                it->y > topBorder.y)
            {
                columns[i-1].push_back(*it);
                break;
            }
        }
    }

    for (int i=0; i < 7; i++)
    {
        columns[i].sort();
        columns[i].reverse();

        int count = BOARD_HEIGHT-1;
        for (list<Stone>::iterator it = columns[i].begin(); it != columns[i].end(); it++)
        {
            output[count][i] = it->yellow ? Yellow : Red;
            count--;
        }
        for (; count >= 0; count--)
        {
            output[count][i] = Empty;
        }
    }
}

VisionResult VisionManager::updateBoard(Board& board)
{
#ifndef BUILD_SEPERATE
    Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
    getVideo(rgbMat);
    State** tmp = board.getBoardCopy();

    processFrame(rgbMat, tmp);

    int diff = 0;
    int xdiff = -1, ydiff = -1;

    cout << "previous state" << endl;
    for(int row=0; row < BOARD_HEIGHT; row++)
    {
        cout << " |";
        for(int col = 0; col < BOARD_WIDTH; col++)
        {
            char c;
            switch(board.getState(row,col))
            {
                case Yellow:
                    c = 'Y';
                    break;
                case Red:
                    c = 'R';
                    break;
                case Empty:
                    c= '-';
                    break;
            }
            cout << " " << c << " |";
        }

        cout << endl;
    }

    cout << "current state" << endl;
    for(int row=0; row < BOARD_HEIGHT; row++)
    {
        cout << " |";
        for(int col = 0; col < BOARD_WIDTH; col++)
        {
            char c;
            switch(tmp[row][col])
            {
                case Yellow:
                    c = 'Y';
                    break;
                case Red:
                    c = 'R';
                    break;
                case Empty:
                    c= '-';
                    break;
            }
            cout << " " << c << " |";
        }

        cout << endl;
    }

    for (int i=0; i<BOARD_HEIGHT; i++)
    {
        for (int j=0; j<BOARD_WIDTH; j++)
        {
            if (tmp[i][j] != Empty)
                DEBUG("stone detected at x " + to_string(i) + " y " + to_string(j));

            if (tmp[i][j] != board.getState(i,j))
            {
                if (board.getState(i,j) != Empty)
                {
                    WARN("error during processing of board");
                    diff = 2;
                    i = BOARD_HEIGHT;
                    j = BOARD_WIDTH;
                }
                diff++;
                xdiff = i;
                ydiff = j;
            }
        }
    }

    if (diff == 0)
    {
        DEBUG("no move detected");
        return NO_MOVE;
    }
    if (diff > 1)
    {
        WARN("too many changes detected");
        return PROC_ERR;
    }

    board.setState(xdiff, ydiff, tmp[xdiff][ydiff]);

    for (int i=0; i<BOARD_HEIGHT; i++)
    {
        delete [] tmp[i];
    }
    delete [] tmp;

    return SUCCESS;
#else
    return NO_MOVE;
#endif
}

#ifdef BUILD_SEPERATE
int main(int argc, char **argv) {
    bool die(false);

    Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
    Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

    Freenect::Freenect freenect;
    DEBUG("creating device");
    KinectManager& device = freenect.createDevice<KinectManager>(0);
    VisionManager vm(&device);
    // VisionManager vm;

    namedWindow("rgb",CV_WINDOW_AUTOSIZE);
#ifdef USE_KINECT
    DEBUG("starting video");
    device.startVideo();
    while (!die) {
        device.getVideo(rgbMat);
        cv::imshow("rgb", rgbMat);

        State** board = new State*[BOARD_HEIGHT];
        for (int row = 0; row < BOARD_HEIGHT; row++)
        {
            board[row] = new State[BOARD_WIDTH];
            for (int col = 0; col < BOARD_WIDTH; col++)
            {
                board[row][col] = State::Empty;
            }
        }
        vm.processFrame(rgbMat, board);

        for(int row=0; row < BOARD_HEIGHT; row++)
        {
            cout << " |";
            for(int col = 0; col < BOARD_WIDTH; col++)
            {
                char c;
                switch(board[row][col])
                {
                    case Yellow:
                        c = 'Y';
                        break;
                    case Red:
                        c = 'R';
                        break;
                    case Empty:
                        c= '-';
                        break;
                }
                cout << " " << c << " |";
            }

            cout << endl;
        }
        char k = cvWaitKey(0);
        if( k == 27 ){
            cvDestroyWindow("rgb");
            break;
        }
    }
    device.stopVideo();
#else
    int cnt = 1;
    while (cnt < 8) {
        Mat img = imread("images/im" + to_string(cnt) + ".jpg");
        imshow("rgb", img);

        State** board = new State*[BOARD_HEIGHT];
        for (int row = 0; row < BOARD_HEIGHT; row++)
        {
            board[row] = new State[BOARD_WIDTH];
            for (int col = 0; col < BOARD_WIDTH; col++)
            {
                board[row][col] = State::Empty;
            }
        }
        vm.processFrame(img, board);

        for(int row = BOARD_HEIGHT - 1; row >= 0 ; row--)
        {
            cout << " |";
            for(int col = 0; col < BOARD_WIDTH; col++)
            {
                char c;
                switch(board[row][col])
                {
                    case Yellow:
                        c = 'Y';
                        break;
                    case Red:
                        c = 'R';
                        break;
                    case Empty:
                        c= '-';
                        break;
                }
                cout << " " << c << " |";
            }

            cout << endl;
        }

        waitKey(0);
        cnt++;
    }
#endif

    return 0;
}
#endif
