#pragma once

#include <SDL2/SDL.h>
#include <iostream>

class Window
{
    public:
        Window();
        ~Window();

        bool keys[400]; // !! Will make segfault if not enough range for key codes
        bool open;

        void set_pixel(int, int, int, int, int);
        void refresh();
        void clear();

        int screen_width;
        int screen_height;

    private:
        SDL_Window *window;
        SDL_Renderer *renderer;
};