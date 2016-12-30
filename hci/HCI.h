#ifndef M_HCI
#define M_HCI

#include <utility>
#include <string>
#include <thread>

#include "LTexture.h"
#include "../ai/Message.h"

class HCI
{
public:
    HCI();
    ~HCI();

    void msg(Message m);
private:
    std::thread gAnimationThread;
    void animationLoop();
    bool SDL_OK;
    bool loopFlag;

    //Screen dimension constants
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    //Animation dimension constants
    const static int ANIMATION_WIDTH = 500;
    const static int ANIMATION_HEIGHT = 500;

    const static int NR_CATEGORIES = 15;
    const static int MEM_DEPTH = 3;
    const static int ANIMATION_FRAMES = 106;

    std::pair<std::string,int> gSoundboardCategories[NR_CATEGORIES] = {
        {"hello",4},
        {"hello_response",0},
        {"how_are_you_response",3},
        {"compliment",2},
        {"compliment_response",0},
        {"insult",0},
        {"insult_response",0},
        {"winning",4},
        {"won",3},
        {"losing",3},
        {"lost",3},
        {"too_easy",3},
        {"too_hard",4},
        {"busy",0},
        {"proc_err",0},
    };

    int latestResponses[NR_CATEGORIES];

    //Animation textures
    LTexture gAnimation[ANIMATION_FRAMES];
    LTexture gMask;

    Mix_Chunk** gSoundboard[NR_CATEGORIES] = { NULL };

    //The window we'll be rendering to
    SDL_Window* gWindow = NULL;

    //The window renderer
    SDL_Renderer* gRenderer = NULL;

    //Starts up SDL and creates window
    bool init();

    //Loads media
    bool loadMedia();

    //Frees media and shuts down SDL
    void close();
};

#endif
