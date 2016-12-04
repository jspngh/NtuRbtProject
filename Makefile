OBJS = sound_manager.cpp

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = snd_mngr

#This is the target that compiles our executable
all : $(OBJS)
	g++ $(OBJS) -w -lSDL2 lSDL2_mixer -o $(OBJ_NAME)
