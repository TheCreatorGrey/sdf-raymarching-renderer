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

    int downres = 4;

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


    // 6x6x6 grid of blocks with RGB colors
    int grid[6][6][6][3];
    for (int ix=0; ix<6; ix++) {
        for (int iy=0; iy<6; iy++) {
            for (int iz=0; iz<6; iz++) {
                if (ix == 0 || iy == 0 || iz == 0) {
                    grid[ix][iy][iz][0] = 255;
                    grid[ix][iy][iz][1] = 10;
                    grid[ix][iy][iz][2] = 10;
                } else {
                    grid[ix][iy][iz][0] = rand() % 200;
                    grid[ix][iy][iz][1] = 0;
                    grid[ix][iy][iz][2] = 0;
                }  
            }
        }
    }

    
    auto ray_vector = [](float vec[], float scan_pitch, float scan_yaw, float cam_pitch, float cam_yaw) {
        float x = cos(scan_pitch) * sin(scan_yaw);
        float y = -sin(scan_pitch);
        float z = cos(scan_pitch) * cos(scan_yaw);


        float distance = sqrt(pow(z, 2) + pow(y, 2));

        // Now calculate the new offsets based on the camera x/pitch angle
        // Opposite is y distance, Adjacent is z distance
        // SOH = sin(cam_pitch) = opposite / hypot
        y = sin(scan_pitch+cam_pitch)*distance; // Reassign offset

        // Repeat for CAH (cos adjacent hypot)
        z = cos(scan_pitch+cam_pitch)*distance;


        // IT'S VERY IMPORTANT THAT THE TOP DOWN TRANSFORM HAPPENS AFTER THE FROM THE SIDE TRANSFORM
        // That was my first attempt, and it confused me

        // Repeat from top down

        distance = sqrt(pow(x, 2) + pow(z, 2));

        // The angle may be changed/inverted by the previous operation. It needs to be recaptured
        // TOA can be used to get the angle with the opposide and adjacent sides of the imaginary triangle
        // tan(theta) = opp/adj
        // solve for theta: 
        // theta = atan(opp/adj)
        // math is so cool for real

        float angle = atan2(x, z);

        z = sin(angle+cam_yaw)*distance;
        x = cos(angle+cam_yaw)*distance;

        vec[0] = x;
        vec[1] = y;
        vec[2] = z;

        //std::cout << x;
        //std::cout << y;
        //std::cout << z;
        //std::cout << "\n";
    };

    int ray_color[3];
    auto cast = [grid, ray_vector](int color_out[], float scan_pitch, float scan_yaw, float cam_pitch, float cam_yaw, float cam_x, float cam_y, float cam_z, float slice_length, int slices) {
        float vector[3];
        ray_vector(vector, scan_pitch, scan_yaw, cam_pitch, cam_yaw);

        float point[3] = {cam_x, cam_y, cam_z};

        // Default/empty color
        color_out[0] = 255;
        color_out[1] = 255;
        color_out[2] = 255;

        for (int s=0; s<slices; s++) {
            int voxel_x = round(point[0]);
            int voxel_y = round(point[1]);
            int voxel_z = round(point[2]);

            if (0 < voxel_x && voxel_x < 6) {
                if (0 < voxel_y && voxel_y < 6) {
                    if (0 < voxel_z && voxel_z < 6) {
                        color_out[0] = grid[voxel_x][voxel_y][voxel_z][0]; //100-s*6;
                        color_out[1] = grid[voxel_x][voxel_y][voxel_z][1];
                        color_out[2] = grid[voxel_x][voxel_y][voxel_z][2];
                        return;
                    }
                }
            }

            point[0] += vector[0]*slice_length;
            point[1] += vector[1]*slice_length;
            point[2] += vector[2]*slice_length;
        }
    };


    float camera_x = 0;
    float camera_y = 0;
    float camera_z = 0;

    float camera_rx = 0;
    float camera_ry = 0;
    float camera_rz = 0;

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

        if (win.keys['a']) {
            camera_x -= .1;
        }

        if (win.keys['d']) {
            camera_x += .1;
        }

        if (win.keys['q']) {
            camera_y += .1;
        }

        if (win.keys['e']) {
            camera_y -= .1;
        }

        //auto begin = std::chrono::high_resolution_clock::now();

        // Coordinate system has 0,0 at center
        for (int x=0; x <= win.screen_width; x+=downres) {
            for (int y=0; y <= win.screen_height; y+=downres) {
                // Relative coordinates ranging from -0.5 to 0.5
                // This could cause stretching later. Keep this in mind
                float rel_x = ((float)x/win.screen_width)-0.5;
                float rel_y = ((float)y/win.screen_height)-0.5;

                cast(ray_color, rel_x, rel_y, camera_rx, camera_ry, camera_x, camera_y, camera_z, .1, 64);

                //std::cout << rel_x;
                
                win.set_pixel(x, y, ray_color[0], ray_color[1], ray_color[2]);
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
