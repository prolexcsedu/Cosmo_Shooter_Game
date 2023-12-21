#include "Functions/Movement.hpp"
#include "Functions/Initialize.hpp"
#include "Functions/All_Function.hpp"

int main(int argv, char **args)
{
    // Initializations
    load_init();
    Mix_Music *backgroundMusic = Mix_LoadMUS("music1.mp3");
    SDL_Renderer *rend = SDL_CreateRenderer(o, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    Mix_PlayMusic(music, -1);

    // render Game Over texture
    CreateRect(ScoreBoardRect, 50, 50, 0, 0);

    load_render(rend);

    while (game_running == true and i)
    {
        Game_Menu.render_menu(rend);

        Limit_Declare(i);

        while (i)
        {
            Load_Background();
            SDL_RenderCopy(rend, backgroundTextures[currentBackgroundIndex], NULL, NULL);

            Mix_PlayMusic(backgroundMusic, -1);
            SDL_Delay(15);
            SDL_RenderCopy(rend, bg_texture, NULL, NULL);
            spaceship_game.border_limits();
            Key_Press();
            

            Handle_Collision(rend);
            Final_Check(rend);
            SDL_RenderClear(rend);

            if (player_dead == true)
            {
                break;
            }
        }

        if (i)
        {
            GameOver.print_final(rend);
        }
    }

    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    SDL_DestroyWindow(o);
    SDL_DestroyRenderer(rend);
   
    clean_background();

    TTF_Quit();
    SDL_Quit();

    return 0;
}