#include <unistd.h>
#include <stdio.h>
#include <utility>

#include "LTexture.h"

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 768;
const int SCREEN_HEIGHT = 1280;

const int NR_CATEGORIES = 14;
const int ANIMATION_FRAMES = 45;


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Animation textures
LTexture gAnimation[ANIMATION_FRAMES];

pair<string,int> gSoundboardCategories[NR_CATEGORIES] = {
    {"hello",4},
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
};

Mix_Chunk** gSoundboard[NR_CATEGORIES] = { NULL };

//The music that will be played
Mix_Music *gMusic = NULL;

bool init()
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

        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
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

                if( Mix_Volume( -1, 20 ) < 0 )
                {
                    printf( "SDL_mixer volume could not be set! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;

    //Load animation textures
    for (int i = 0; i < ANIMATION_FRAMES; i++)
    {
        std::string filepath =  "./animations/HAL/frame_" + std::to_string(i) + ".gif";
        if( !gAnimation[i].loadFromFile( filepath.c_str() ) )
        {
            printf( "Failed to load prompt texture!\n" );
            success = false;
        }
    }

    //Load sound effects
    for (int i = 0; i < NR_CATEGORIES; i++)
    {
        pair<string,int> category = gSoundboardCategories[i];
        gSoundboard[i] = new Mix_Chunk*[category.second];
        for (int j = 0; j < category.second; j++)
        {
            std::string filepath =  "./soundboard/HAL/" + category.first + "/" + to_string(j+1) + ".mp3";
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

void close()
{
    //Free loaded images
    for(int i = 0; i < ANIMATION_FRAMES; i++)
    {
        gAnimation[i].free();
    }

    //Free the sound effects
    for (int i = 0; i < NR_CATEGORIES; i++)
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
    Mix_FreeMusic( gMusic );
    gMusic = NULL;

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

int main( int argc, char* args[] )
{
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Main loop flag
            bool quit = false;

            //frame_cntr
            int frame = 0;

            //Event handler
            SDL_Event e;

            //While application is running
            while( !quit )
            {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    //Handle key press
                    else if( e.type == SDL_KEYDOWN )
                    {
                        switch( e.key.keysym.sym )
                        {
                            //Play high sound effect
                            case SDLK_1:
                                Mix_PlayChannel( -1, gSoundboard[0][1], 0 );
                                break;

                                //Play medium sound effect
                            case SDLK_2:
                                Mix_PlayChannel( -1, gSoundboard[1][0], 0 );
                                break;

                                //Play low sound effect
                            case SDLK_3:
                                Mix_PlayChannel( -1, gSoundboard[5][0], 0 );
                                break;

                                //Play scratch sound effect
                            case SDLK_4:
                                Mix_PlayChannel( -1, gSoundboard[8][1], 0 );
                                break;

                            case SDLK_9:
                                //If there is no music playing
                                if( Mix_PlayingMusic() == 0 )
                                {
                                    //Play the music
                                    Mix_PlayMusic( gMusic, -1 );
                                }
                                //If music is being played
                                else
                                {
                                    //If the music is paused
                                    if( Mix_PausedMusic() == 1 )
                                    {
                                        //Resume the music
                                        Mix_ResumeMusic();
                                    }
                                    //If the music is playing
                                    else
                                    {
                                        //Pause the music
                                        Mix_PauseMusic();
                                    }
                                }
                                break;

                            case SDLK_0:
                                //Stop the music
                                Mix_HaltMusic();
                                break;
                        }
                    }
                }

                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );

                //Render prompt
                gAnimation[frame].render( 0, 0 );
                frame = ++frame % ANIMATION_FRAMES;
                // if (frame > 12 && frame < 35)
                //     frame = 35;

                //Update screen
                SDL_RenderPresent( gRenderer );

                usleep(100001);
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}
