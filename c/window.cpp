#include "window.hpp"

Window::Window() // Setup (Called when Window is initialized)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "Failed to initialize the SDL2 library\n";
    }

    open = true;

    screen_width = 680;
    screen_height = 480;

    window = SDL_CreateWindow(
        "Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screen_width, screen_height,
        0
    );

    renderer = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED
    );
}

Window::~Window() // Called when Window is deinitialized
{
    std::cout << "Pepperoni ravioli\n\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::set_pixel(int x, int y, int r, int g, int b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 1);
    SDL_RenderDrawPoint(renderer, x, y);
}

void Window::pixel_downres(int x, int y, int r, int g, int b, int downres)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, 1);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = downres;
    rect.h = downres;

    SDL_RenderFillRect(renderer, &rect);
}

void Window::refresh()
{
    SDL_Event e;
    while(SDL_PollEvent(&e) > 0) // Iterate all events occuring at one time (if any)
    {
        switch(e.type)
        {
            case SDL_QUIT:
                open = false;
                break;

            case SDL_KEYDOWN:
                std::cout << e.key.keysym.sym;
                std::cout << "\n";
                keys[e.key.keysym.sym] = true;
                
                break;

            case SDL_KEYUP:
                keys[e.key.keysym.sym] = false;

                break;
        }
    }

    SDL_RenderPresent(renderer);
}

void Window::clear()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
    SDL_RenderClear(renderer);
}