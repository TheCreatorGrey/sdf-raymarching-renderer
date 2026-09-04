#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include "window.hpp"
#include <chrono>


float sdfCube(float x, float y, float z, float sx, float sy, float sz, float rad) {
    float dx = abs(x - sx)-rad;
    float dy = abs(y - sy)-rad;
    float dz = abs(z - sz)-rad;

    return fmax(dx, fmax(dy, dz));
}

float sdfSphere(float x, float y, float z, float sx, float sy, float sz, float rad) {
    return sqrt(pow(x-sx, 2) + pow(y-sy, 2) + pow(z-sz, 2)) - rad; // radius
}

float signedDistance(float x, float y, float z) {
    return fmax(sdfCube(x, y, z, 0, 0, 0, 1), -sdfSphere(x, y, z, 0, 0, 0, 1.2));
}


float* ray_vector(float scan_pitch, float scan_yaw, float cam_pitch, float cam_yaw) {
    float x = sin(scan_pitch); // -
    float y = cos(scan_pitch) * sin(scan_yaw);
    float z = cos(scan_pitch) * cos(scan_yaw);


    float distance = sqrt(pow(z, 2) + pow(y, 2));
    float angle = atan2(z, y);

    // Now calculate the new offsets based on the camera x/pitch angle
    // Opposite is y distance, Adjacent is z distance
    // SOH = sin(cam_pitch) = opposite / hypot
    y = sin(angle+cam_pitch)*distance; // Reassign offset

    // Repeat for CAH (cos adjacent hypot)
    z = cos(angle+cam_pitch)*distance;


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

    angle = atan2(x, z);

    z = sin(angle+cam_yaw)*distance;
    x = cos(angle+cam_yaw)*distance;

    static float vec[3];
    vec[0] = x;
    vec[1] = y;
    vec[2] = z;

    //std::cout << x;
    //std::cout << y;
    //std::cout << z;
    //std::cout << "\n";

    return vec;
}


int main() {
    Window win;

    bool running = true;
    int frame = 0;

    int downres = 4;

    float objects[2][10] = {
        // type, x, y, z, sx, sy, sz, r, g, b
        {1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0},
        {1.0, 5.0, 0.0, 0.0, 0.28, 0.28, 0.28, 0.5, 1.0, 0.0}
    };

    int ray_color[3];
    auto cast = [](int color_out[], float scan_pitch, float scan_yaw, float cam_pitch, float cam_yaw, float cam_x, float cam_y, float cam_z, int slices) {
        float* vector = ray_vector(scan_pitch, scan_yaw, cam_pitch, cam_yaw);

        float point[3] = {cam_x, cam_y, cam_z};

        // Default/empty color
        color_out[0] = 255;
        color_out[1] = 255;
        color_out[2] = 255;

        float dist;
        float thresh = 0.01; // When the program decides that the distance is close enough to change the color of the pixel

        for (int s=0; s<64; s++) {
            dist = signedDistance(point[0], point[1], point[2]);

            // Ray has hit object
            if (dist < thresh) {
                float eps = 0.02;
                float norm_X = (signedDistance(point[0]+eps, point[1], point[2]) - signedDistance(point[0]-eps, point[1], point[2])) / (eps*2);


                color_out[0] = 255; //100-s*6;
                color_out[1] = 100+(100*norm_X);
                color_out[2] = 0;
                return;
            }

            point[0] += vector[0]*dist;
            point[1] += vector[1]*dist;
            point[2] += vector[2]*dist;
        }
    };


    float camera_x = 0;
    float camera_y = 0;
    float camera_z = 0;

    float camera_rx = 0;
    float camera_ry = 0;
    //float camera_rz = 0;

    float camera_rx_rad; // Converted from degrees
    float camera_ry_rad;

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
            camera_x += .1;
        }

        if (win.keys['d']) {
            camera_x -= .1;
        }

        if (win.keys['q']) {
            camera_y -= .1;
        }

        if (win.keys['e']) {
            camera_y += .1;
        }

        if (win.keys[1073741906]) { // Up key
            camera_rx += 1;

            std::cout << camera_ry;
            std::cout << "\n";
        }

        if (win.keys[1073741905]) { // Down key
            camera_rx -= 1;
            std::cout << camera_ry;
            std::cout << "\n";
            std::cout << camera_rx;
            std::cout << "\n\n";

            if (camera_ry < 1) {
                //camera_ry = 0; //?
            }
        }

        if (win.keys[1073741904]) { // Left key
            camera_ry -= 1;
        }

        if (win.keys[1073741903]) { // Right key
            camera_ry += 1;
        }

        //auto begin = std::chrono::high_resolution_clock::now();
                                                // offset
        camera_rx_rad = ((M_PI/180) * (camera_rx-90));
        camera_ry_rad = ((M_PI/180) * (camera_ry+90));

        // Coordinate system has 0,0 at center
        for (int x=0; x <= win.screen_width; x+=downres) {
            for (int y=0; y <= win.screen_height; y+=downres) {
                // Relative coordinates ranging from -0.5 to 0.5
                // This could cause stretching later. Keep this in mind
                float rel_x = ((float)x/win.screen_width)-0.5;
                float rel_y = ((float)y/win.screen_width)-0.5;

                cast(ray_color, rel_x, rel_y, camera_rx_rad, camera_ry_rad, camera_x, camera_y, camera_z, 8);

                //std::cout << rel_x;
                
                win.pixel_downres(x, y, ray_color[0], ray_color[1], ray_color[2], downres);
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
