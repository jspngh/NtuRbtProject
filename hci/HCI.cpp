#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <utility>
#include <cstdlib>
#include <random>

#include "HCI.h"

using namespace std;

HCI::HCI() : gAnimationThread(&HCI::animationLoop, this)
{
    // seed random number generator
    srand (time(NULL));

    loopFlag = true;
    isSpeaking = 0;
    scaleFac = 1;
    for (int i=0; i < NR_CATEGORIES; i++)
    {
        latestResponses[i] = -1;
    }
    for (int i=0; i < SPEECH_PATTERN; i++)
    {
        speechPattern[i] = -1;
    }
    for (int i=0; i < SPEECH_FRAMES; i++)
    {
        speechPattern[i] = i;
        speechPattern[20 + i] = i;
        speechPattern[45 + i] = i;
    }
    for (int i=1; i < SPEECH_FRAMES; i++)
    {
        speechPattern[7 + i] = 7-i;
        speechPattern[27 + i] = 7-i;
        speechPattern[52 + i] = 7-i;
    }
}

HCI::~HCI()
{
    close();
}

void HCI::animationLoop()
{
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
        exit(-1);
    }
    if( !loadMedia() )
    {
        printf( "Failed to load media!\n" );
        exit(-1);
    }

    //frame counter
    int animationFrame = 0;
    int speechFrame = 0;

    int animationSpeed = 0;
    uint8_t speedCounter = 0;

    //Event handler
    SDL_Event e;

    //While application is running
    while( loopFlag )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                loopFlag = false;
            }
        }

        //Clear screen
        SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
        SDL_RenderClear( gRenderer );

        //Render prompt
        SDL_Rect rAnimation = {0,
                               0,
                               scaleFac * gAnimation[animationFrame].getWidth(),
                               scaleFac * gAnimation[animationFrame].getHeight()};
        SDL_Rect rMask = {0,
                          0,
                          scaleFac * gMask.getWidth(),
                          scaleFac * gMask.getHeight()};
        SDL_Rect rSpeech = {0,
                            0,
                            (int) (1./2 * scaleFac * gSpeech[0].getWidth()),
                            (int) (1./2 * scaleFac * gSpeech[0].getHeight())};

        gAnimation[animationFrame].render( (SCREEN_WIDTH - rAnimation.w)/ 2,
                                  (SCREEN_HEIGHT - rAnimation.h)/ 2,
                                  &rAnimation );

        if (isSpeaking > 0)
        {
            isSpeaking -= 20000;
            int tmp = speechPattern[speechFrame];
            if (tmp > -1)
            {
                gSpeech[tmp].setAlpha( 130 - tmp * 6 );
                gSpeech[tmp].render( (SCREEN_WIDTH - rSpeech.w)/ 2,
                                     (SCREEN_HEIGHT - rSpeech.h)/ 2,
                                     &rSpeech );
            }
            speechFrame = ++speechFrame % SPEECH_PATTERN;
        }

        gMask.render( (SCREEN_WIDTH - rMask.w)/ 2,
                      (SCREEN_HEIGHT - rMask.h)/ 2,
                      &rMask );

        speedCounter = ++speedCounter % animationPattern[animationSpeed];
        if (speedCounter == 0)
        {
            animationFrame = ++animationFrame % HCI::ANIMATION_FRAMES;
            if (animationFrame == 0)
                animationSpeed = ++animationSpeed % HCI::ANIMATION_PATTERN;
        }

        //Update screen
        SDL_RenderPresent( gRenderer );

        usleep(20000);
    }
}

void HCI::msg(Message m)
{
    int cat;
    if (m.type)
    {
        // this is a BehaviourState
        printf("HCI: received a behaviour state message\n");
        switch(m.s)
        {
            case INVITE:
                cat = 0;
                break;
            case WINNING:
                cat = 7;
                break;
            case WON:
                cat = 8;
                break;
            case LOSING:
                cat = 9;
                break;
            case LOST:
                cat = 10;
                break;
            case BUSY_MOVE:
                cat = 13;
                break;
            case WAITING_VISION:
                cat = 13;
                break;
            case WAITING_USER:
                cat = 14;
                break;
            case MOVE_DONE:
                cat = 15;
                break;
            case BOARD_PROC_ERR:
                cat = 16;
                break;
            case COMPLIMENT:
                cat = 3;
                break;
            case INSULT:
                cat = 5;
                break;
        }
    }
    else
    {
        printf("HCI: received a voicecommand message\n");
        switch(m.c)
        {
            case HELLO:
                cat = 1;
                break;
            case HOW_ARE_YOU:
                cat = 2;
                break;
            case COMPLIMENTING:
                cat = 4;
                break;
            case INSULTING:
                cat = 6;
                break;
            case TOO_EASY:
                cat = 11;
                break;
            case TOO_HARD:
                cat = 12;
                break;
        }

    }
    int randResp, latestResp;
    randResp = latestResp = latestResponses[cat];
    printf("before the if...\n");
    if(gSoundboardCategories[cat].second < 1) return;

    printf("before the while...\n");
    while ((randResp = rand() % gSoundboardCategories[cat].second) == latestResp);
    printf("%d, %d\n", randResp, cat);
    Mix_PlayChannel( -1, gSoundboard[cat][randResp], 0 );
    isSpeaking = gSoundboard[cat][randResp]->alen / 170000.0 * 1000000;
    latestResponses[cat] = randResp;
}

bool HCI::init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Get screen resolution
        SDL_Rect r;
        SDL_GetDisplayBounds(0, &r);
        if(r.w * r.h > 3000000)
            scaleFac = 2;

        SCREEN_WIDTH = r.w;
        SCREEN_HEIGHT = r.h;
        printf("Running on a display with resulution %dx%d\n", r.w, r.h);

        //Create window
        gWindow = SDL_CreateWindow( "HAL 9000",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     HCI::SCREEN_WIDTH,
                                     HCI::SCREEN_HEIGHT,
                                     SDL_WINDOW_MAXIMIZED | SDL_WINDOW_ALLOW_HIGHDPI );

        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            if( SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN) < 0)
            {
                printf( "Window could not be set fullscreen! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }

            //Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                LTexture::setRenderer( gRenderer );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }

                //Initialize SDL_mixer
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }

                if( Mix_Volume( -1, 50 ) < 0 )
                {
                    printf( "SDL_mixer volume could not be set! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
            }
        }
    }

    return success;
}

bool HCI::loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load animation textures
    for (int i = 0; i < HCI::ANIMATION_FRAMES; i++)
    {
        std::string filepath =  "./hci/animations/HAL 9000/frame_" + std::to_string(i) + ".gif";
        if( !gAnimation[i].loadFromFile( filepath.c_str() ) )
        {
            printf( "Failed to load prompt texture!\n" );
            success = false;
        }
    }

    for (int i = 0; i < HCI::SPEECH_FRAMES; i++)
    {
        std::string filepath =  "./hci/animations/Pulse/frame_" + std::to_string(i) + ".png";
        if( !gSpeech[i].loadFromFile( filepath.c_str() ) )
        {
            printf( "Failed to load prompt texture!\n" );
            success = false;
        }
        else
        {
            gSpeech[i].setBlendMode( SDL_BLENDMODE_BLEND );
        }
    }

    std::string filepath =  "./hci/animations/HAL 9000/mask.gif";
    if( !gMask.loadFromFile( filepath.c_str() ) )
    {
        printf( "Failed to load prompt texture!\n" );
        success = false;
    }

    //Load sound effects
    for (int i = 0; i < HCI::NR_CATEGORIES; i++)
    {
        pair<string,int> category = gSoundboardCategories[i];
        gSoundboard[i] = new Mix_Chunk*[category.second];
        for (int j = 0; j < category.second; j++)
        {
            std::string filepath =  "./hci/soundboard/HAL/" + category.first + "/" + to_string(j+1) + ".mp3";
            gSoundboard[i][j] = Mix_LoadWAV(filepath.c_str());
            if( gSoundboard[i][j] == NULL )
            {
                printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
        }
    }

    return success;
}

void HCI::close()
{
    //Free loaded images
    for(int i = 0; i < HCI::ANIMATION_FRAMES; i++)
    {
        gAnimation[i].free();
    }

    //Free the sound effects
    for (int i = 0; i < HCI::NR_CATEGORIES; i++)
    {
        pair<string,int> category = gSoundboardCategories[i];
        for (int j = 0; j < category.second; j++)
        {
            Mix_FreeChunk(gSoundboard[i][j]);
            gSoundboard[i][j] = NULL;
        }
        delete[] gSoundboard[i];
    }

    //Free the music
    // Mix_FreeMusic( gMusic );
    // gMusic = NULL;

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
