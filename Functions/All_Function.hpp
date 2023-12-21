#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

void load_init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    
    TTF_Init();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    o = SDL_CreateWindow("Cosmo_Shooter_Game",
                         SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED,
                         Width,
                         Height,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    Mix_Music *music = Mix_LoadMUS("music1.mp3");

    if (music == NULL)
    {
        cout << "SORRY!!NO MUSIC\n";
    }
}

void Load_Background(){
    Uint32 currentTime = SDL_GetTicks();

    if (currentTime - lastBackgroundSwitchTime >= 11000)
    {
        currentBackgroundIndex = (currentBackgroundIndex + 1) % 3; // Switch to the next background
        lastBackgroundSwitchTime = currentTime;
    }
}

void load_render(SDL_Renderer *rend)
{
    // Final Screen
    surface = IMG_Load("images/score.png");
    final_img = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    // Menu render
    surface = IMG_Load("images/cosmo.png");
    menu_img = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    

    
    surface = IMG_Load("images/background1.png");
    backgroundTextures[0] = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("images/background2.png");
    backgroundTextures[1] = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("images/background3.png");
    backgroundTextures[2] = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);


    // Game Over
    surface = IMG_Load("images/Game_Over.png");
    GameOver_texture = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("images/bullet.png");
    // render bullet texture
    bullet_texture = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    // render Explosion
    surface = IMG_Load("images/explosion.png");
    explosion_texture = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    // render ufo texture
    surface = IMG_Load("images/ufo.png");
    ufo_texture = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    // render spaceship texture
    surface = IMG_Load("images/spaceship.png");
    spaceship = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);
}

void Limit_Declare(int x)
{
    if (x == 0)
        return;
    font = TTF_OpenFont("Text_Font.ttf", 24);
    white = {255, 255, 255, 255};
    spaceship_game.declare();

    for (int i = 0; i < shot_limit; i++)
    {
        shot[i].declare();
    }

    for (int i = 0; i < ufo_limit; i++)
    {
        obstacle[i].declare();
    }

    SpaceshipRect.h = spaceship_game.dimension;
    SpaceshipRect.w = spaceship_game.dimension;
}

void Key_Press()
{
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
            i = 0;

        else if (e.type == SDL_MOUSEMOTION)
        {
            spaceship_game.x = e.motion.x;
            spaceship_game.y = e.motion.y;

            SpaceshipRect.x = spaceship_game.x;
            SpaceshipRect.y = spaceship_game.y;
        }

        else if (e.type == SDL_KEYDOWN)
        {
            switch (e.key.keysym.sym)
            {

            case SDLK_SPACE:
            {
                for (int i = 0; i < shot_limit; i++)
                {
                    if (shot[i].on == false and shot[0].reload == false)
                    {
                        shot[i].turn_on();
                        shot[i].x = spaceship_game.x;
                        shot[i].y = spaceship_game.y;
                        break;
                    }

                    else if (i == shot_limit - 1 and shot[0].reload == false)
                    {
                        shot[0].reload = true;
                        shot[0].relaod_start = SDL_GetTicks();
                    }
                }

                break;
            };

            case SDLK_ESCAPE:
            {
                i = 0;
                break;
            };
            }
        }
    }
}





void Handle_Collision(SDL_Renderer *rend)
{
    // Bullet
    for (int i = 0; i < shot_limit; i++)
    {
        if (shot[0].reload == true)
        {
            if (SDL_GetTicks() - shot[0].relaod_start >= shot[0].reload_time)
            {
                shot[0].reload = false;

                for (int i = 0; i < shot_limit; i++)
                {
                    shot[i].on = false;
                }
            }
        }

        if (shot[i].on == true)
        {
            SDL_Rect a;

            a.x = shot[i].x;
            a.y = shot[i].y + 15;

            a.h = shot[i].height;
            a.w = shot[i].width;

            SDL_RenderCopy(rend, bullet_texture, NULL, &a);
            shot[i].update();
        }
    }

    // UFO
    for (int i = 0; i < ufo_limit; i++)
    {
        SDL_Rect a;

        if (obstacle[i].on == false && obstacle[i].probability_ufo())
        {
            obstacle[i].on = true;
            obstacle[i].create_ufo();
        }

        if (obstacle[i].on == true || obstacle[i].explode == true)
        {
            if (obstacle[i].x <= 0)
            {
                obstacle[i].on = false;
                continue;
            }

            a.x = obstacle[i].x;
            a.y = obstacle[i].y;

            a.h = obstacle[i].length;
            a.w = obstacle[i].width;

            if (obstacle[i].on)
                SDL_RenderCopy(rend, ufo_texture, NULL, &a);

            else if (obstacle[i].explode)
                SDL_RenderCopy(rend, explosion_texture, NULL, &a);
        }

        // check if bullet hits ufo
        for (int j = 0; j < shot_limit; j++)
        {
            if (shot[j].on == false)
                continue;
            if (obstacle[i].collide(shot[j]))
            {
                spaceship_game.IncreasePoint();
            };
        }

        // check if ufo hit spaceship
        obstacle[i].collide_spaceship(spaceship_game);
        obstacle[i].update();
    }
}



void Final_Check(SDL_Renderer *rend)
{
    player_dead = false;

    if (spaceship_game.on == true)
    {
        // Printing Score
        SDL_Rect ScoreBoardRect;
        SDL_Rect LivesDisplayRect;

        PlayerScoreGame = spaceship_game.points;

        CreateRect(ScoreBoardRect, 50, 120, 0, 0);
        surface = TTF_RenderText_Solid(font, spaceship_game.PointString(points), white);
        PointTexture = SDL_CreateTextureFromSurface(rend, surface);
        SDL_RenderCopy(rend, PointTexture, NULL, &ScoreBoardRect);
        SDL_FreeSurface(surface);

        // Printing Lives
        CreateRect(LivesDisplayRect, 50, 100, 400, 0);
        surface = TTF_RenderText_Solid(font, spaceship_game.LivesDispayString(lives), white);
        LivesTexture = SDL_CreateTextureFromSurface(rend, surface);
        SDL_RenderCopy(rend, LivesTexture, NULL, &LivesDisplayRect);
        SDL_FreeSurface(surface);

        // Reloading
        if (shot[0].reload == true)
        {
            SDL_Rect ReloadingReqt;
            CreateRect(ReloadingReqt, 50, 100, 800, 0);
            surface = TTF_RenderText_Solid(font, "RELOADING...", white);
            ReloadingTexture = SDL_CreateTextureFromSurface(rend, surface);
            SDL_RenderCopy(rend, ReloadingTexture, NULL, &ReloadingReqt);
            SDL_FreeSurface(surface);
        }
    }

    else
        spaceship_game.respawn();

    if (spaceship_game.CheckLives())
    {
        player_dead = true;
        SDL_Delay(500);
        SDL_RenderClear(rend);
        SDL_RenderCopy(rend, GameOver_texture, NULL, NULL);
        SDL_RenderPresent(rend);
        SDL_Delay(500);

        spaceship_game.on = false;
        spaceship_game.declare();

        for (int i = 0; i < shot_limit; i++)
        {
            shot[i].declare();
        }

        for (int i = 0; i < ufo_limit; i++)
        {
            obstacle[i].declare();
        }
    }

    else
    {
        SDL_RenderCopy(rend, spaceship, NULL, &SpaceshipRect);
        SDL_RenderPresent(rend);
    }
}
  

void clean_background(){

     for (int i = 0; i < 3; ++i)
    {
        SDL_DestroyTexture(backgroundTextures[i]);
    }
}