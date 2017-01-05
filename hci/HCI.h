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
    void animationLoop();

    std::thread gAnimationThread;
    bool SDL_OK;
    bool loopFlag;
    int isSpeaking;
    int scaleFac;

    //Screen dimension constants
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    const static int NR_CATEGORIES = 18;
    const static int MEM_DEPTH = 3;
    const static int ANIMATION_FRAMES = 80;
    const static int ANIMATION_PATTERN = 11;
    const static int SPEECH_FRAMES = 8;
    const static int SPEECH_PATTERN = 65;

    std::pair<std::string,int> gSoundboardCategories[NR_CATEGORIES] = {
        {"hello",2},
        {"hello_response",2},
        {"how_are_you_response",3},
        {"compliment",2},
        {"compliment_response",2},
        {"insult",2},
        {"insult_response",2},
        {"winning",4},
        {"won",3},
        {"losing",3},
        {"lost",3},
        {"too_easy",3},
        {"too_hard",4},
        {"busy",0},
        {"waiting",3},
        {"your_move",3},
        {"my_move",3},
        {"proc_err",0},
    };

    int animationPattern[ANIMATION_PATTERN] = { 3, 5, 4, 4, 4, 3, 3, 4, 4, 4, 4 };
    int speechPattern[SPEECH_PATTERN];
    int latestResponses[NR_CATEGORIES];

    //Animation textures
    LTexture gAnimation[ANIMATION_FRAMES];
    LTexture gSpeech[SPEECH_FRAMES];
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
