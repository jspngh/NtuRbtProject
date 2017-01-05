#include <unistd.h>
#include <stdio.h>
#include <utility>
#include <cstdlib>
#include <random>

#include "HCI.h"

using namespace std;

random_device rd;     // only used once to initialise (seed) engine
mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
uniform_int_distribution<int> uni(0,9); // guaranteed unbiased

HCI::HCI() : gAnimationThread(&HCI::animationLoop, this)
{
    loopFlag = true;
    for (int i=0; i < NR_CATEGORIES; i++)
    {
        latestResponses[i] = -1;
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
    int frame = 0;

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
        gAnimation[frame].render( (SCREEN_WIDTH - ANIMATION_WIDTH)/ 2,
                                  (SCREEN_HEIGHT - ANIMATION_HEIGHT)/ 2 );
        gMask.render( (SCREEN_WIDTH - ANIMATION_WIDTH)/ 2,
                      (SCREEN_HEIGHT - ANIMATION_HEIGHT)/ 2 );
        frame = ++frame % HCI::ANIMATION_FRAMES;

        //Update screen
        SDL_RenderPresent( gRenderer );

        usleep(80000);
    }
}

void HCI::msg(Message m)
{
    int cat;
    if (m.type)
    {
        // this is a BehaviourState
        printf("HCI: received a behaviourstate message\n");
        switch(m.s)
        {
            case LOSING:
                cat = 9;
                break;
            case WINNING:
                cat = 7;
                break;
            case BUSY_MOVE:
                cat = 13;
                break;
            case WAITING_VISION:
                cat = 13;
                break;
            case WAITING_USER:
                cat = 13;
                break;
            case BOARD_PROC_ERR:
                cat = 14;
                break;
            case INVITE:
                cat = 0;
                break;
            case COMPLIMENT:
                cat = 3;
                break;
        }
    }
    else
    {
        printf("HCI: received a voicecommand message\n");
        switch(m.c)
        {
            case HELLO:
                cat = 0;
                break;
            case HOW_ARE_YOU:
                cat = 2;
                break;
            case COMPLIMENTING:
                cat = 4;
                break;
            case INSULT:
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
                LTexture::setRenderer(gRenderer);

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
