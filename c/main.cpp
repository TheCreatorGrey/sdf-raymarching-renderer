#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include "window.hpp"
#include <chrono>

int main()
{
    Window win;

    bool running = true;
    int frame = 0;


    int points[8][3] = {
        {-1, -1, -1},
        {-1, 1, -1},
        {1, 1, -1},
        {1, -1, -1},
        {-1, -1, 1},
        {-1, 1, 1},
        {1, 1, 1},
        {1, -1, 1}
    };

    float camera_x = 0;
    float camera_y = 0;
    float camera_z = 0;

    float cam_ro_x = 0;
    float cam_ro_y = 0;
    float cam_ro_z = 0;

    SDL_Event event;

    while (running) 
    {   
        win.clear();

        // +- 340 & +- 240

        //win.cam_pos[2] -= 5;

        //win.cam_ro[2] -= .01;
        //win.cam_pos[0] = std::sin(frame/100)*600;
        //win.cam_pos[2] = std::cos(frame/100)*600;

        // Keyboard stuff
        
        if (win.keys['w']) {
            camera_z += .1;
        }

        if (win.keys['s']) {
            camera_z -= .1;
        }

        //auto begin = std::chrono::high_resolution_clock::now();

        // Coordinate system has 0,0 at center
        for (int x = -win.screen_width/2; x <= win.screen_width/2; x++) {
            for (int y = -win.screen_height/2; y <= win.screen_height/2; y++) {

                // Relative coordinates ranging from -0.5 to 0.5
                // This could cause stretching later. Keep this in mind
                float rel_x = ((float)x)/(win.screen_width/2);
                float rel_y = ((float)y)/(win.screen_height/2);

                //std::cout << rel_x;
                
                win.set_pixel(x, y, camera_z*100, rel_x*255, rel_y*255);
            }
        }

        //auto end = std::chrono::high_resolution_clock::now();
        //auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        //printf("Time measured: %.3f seconds.\n", elapsed.count() * 1e-9);

        frame++;
        win.refresh();

        if (!win.open) {
            running = false;
            std::cout << "Closing";
        }

        //SDL_Delay(100);
    }
}
