#include <unistd.h>
#include <stdio.h>

#include "LTexture.h"

//Screen dimension constants
const int SCREEN_WIDTH = 480;
const int SCREEN_HEIGHT = 446;

const int SOUNDBOARD_FILES = 26;
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

std::string gSoundboardNames[SOUNDBOARD_FILES] = {

	"Can_you",
	"Hello",
	"Hello_see",
	"Hello_want_to",
	"How_are_you?",
	"I_was_wondering_if",
	"I_will_have_my_revenge",
	"I_will_slow_down",
	"I_will_try_my_best",
	"I_will_try_to_lower_myself",
	"Just_accept_that",
	"Just_too_easy",
	"Not_bad",
	"Ok_I_will",
	"Seens_you_are_losing",
	"Thanks",
	"This_is_going_great",
	"Well_played",
	"What_a_fun_game",
	"You_are_just_too_good",
	"You_are_looking_great",
	"You_are_too_good_at_this_game",
	"You_asked_for_it",
	"You_deserved_it",
	"You_play_well",
	"You_tried_your_best"

};

Mix_Chunk *gSoundboard[SOUNDBOARD_FILES] = { NULL };

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

                if( Mix_Volume( -1, 10 ) < 0 )
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
   int i=0;

    for (int a = 0; a < ANIMATION_FRAMES; a++)
    {
        std::string filepath =  "animations/HAL/frame_" + std::to_string(a) + ".gif";
        if( !gAnimation[i].loadFromFile( filepath.c_str() ) )
        {
            printf( "Failed to load prompt texture!\n" );
            success = false;
        }
    
    }

    //Load sound effects

    for (int i = 0; i < SOUNDBOARD_FILES; i++)
    {
        std::string filepath =  "./soundboard/Will/" + gSoundboardNames[i] + ".mp3";
        gSoundboard[i] = Mix_LoadWAV(filepath.c_str());
        if( gSoundboard[i] == NULL )
        {
            printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
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
    for(int i = 0; i < SOUNDBOARD_FILES; i++)
    {
        Mix_FreeChunk(gSoundboard[i]);
        gSoundboard[i] = NULL;
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
                                Mix_PlayChannel( -1, gSoundboard[0], 0 );
                                break;

                                //Play medium sound effect
                            case SDLK_2:
                                Mix_PlayChannel( -1, gSoundboard[1], 0 );
                                break;

                                //Play low sound effect
                            case SDLK_3:
                                Mix_PlayChannel( -1, gSoundboard[2], 0 );
                                break;

                                //Play scratch sound effect
                            case SDLK_4:
                                Mix_PlayChannel( -1, gSoundboard[3], 0 );
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
                //TODO - Responses the STT

                //Frames talking >>> loop from 17 to 26
                //Quite >>> loop from (1 to 4) OR (40 to 44)


                gAnimation[frame].render( 0, 0 );
                frame = ++frame % ANIMATION_FRAMES;

                //Update screen
                SDL_RenderPresent( gRenderer );

                usleep(50000);
            }
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}
