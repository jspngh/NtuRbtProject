OBJS = SoundManager.cpp LTexture.cpp

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = snd_mngr

#This is the target that compiles our executable
all : $(OBJS)
	g++ --std=c++11 $(OBJS) -w -lSDL2 -lSDL2_image -lSDL2_mixer -o $(OBJ_NAME)
