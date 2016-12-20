OBJS = VisionManager.cpp KinectManager.cpp

OBJ_NAME = vision_mngr

CFLAGS = --std=c++11 -Wall
INCLUDES =
LFLAGS = `pkg-config --cflags --libs opencv` -lpthread -lfreenect

all : $(OBJS)
	g++ $(CFLAGS) $(INCLUDES) $(LFLAGS) $(OBJS) -o $(OBJ_NAME)
