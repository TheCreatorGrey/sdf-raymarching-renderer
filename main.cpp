#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include "window.hpp"
#include <chrono>


struct coordinate {
    float x;
    float y;
    float z;
};

//struct coordinate repeat(struct coordinate point) {
//    struct coordinate result;
//    result.x = fmod(point.x, 5.0) * sign(point.x);
//    result.y = fmod(point.y, 5.0) * sign(point.y);
//    result.z = fmod(point.z, 5.0) * sign(point.z);
//    return result;
//}

struct coordinate rotate(struct coordinate point, struct coordinate origin, float angle_rad) {
    // High school trig
    float x_leg = (point.x-origin.x);
    float y_leg = (point.y-origin.y);
    float hypot = sqrt(pow(x_leg, 2) + pow(y_leg, 2)); // Pythagoren theorem

    float pre_rotation = atan(y_leg/x_leg);

    // Find new opposite / adjacent with hypot using SOHCAHTOA

    struct coordinate rotated;

    rotated.x = cos(angle_rad-pre_rotation)*hypot;
    rotated.y = sin(angle_rad-pre_rotation)*hypot;
    rotated.z = point.z;

    return rotated;
}


struct sdfInfo {
    float dist;
    int r;
    int g;
    int b;
};

struct sdfInfo unify(struct sdfInfo sdf1, struct sdfInfo sdf2) {
    // Sdf2 cuts out of sdf1
    struct sdfInfo un;

    un.dist = fmin(sdf1.dist, sdf2.dist);

    if (un.dist == sdf1.dist) {
        un.r = sdf1.r;
        un.g = sdf1.g;
        un.b = sdf1.b;
    } else {
        un.r = sdf2.r;
        un.g = sdf2.g;
        un.b = sdf2.b;
    }

    return un;
}

struct sdfInfo negate(struct sdfInfo sdf1, struct sdfInfo sdf2) {
    // Sdf2 cuts out of sdf1
    struct sdfInfo negation;

    negation.dist = fmax(sdf1.dist, -sdf2.dist);
    negation.r = sdf1.r;
    negation.g = sdf1.g;
    negation.b = sdf1.b;

    return negation;
}

struct sdfInfo sdfCube(struct coordinate point, struct coordinate position, float size, int r=100, int g=100, int b=100) {
    float dx = abs(point.x - position.x)-size;
    float dy = abs(point.y - position.y)-size;
    float dz = abs(point.z - position.z)-size;

    struct sdfInfo cube;

    cube.dist = fmax(dx, fmax(dy, dz));
    cube.r = r;
    cube.g = g;
    cube.b = b;

    return cube;
}

struct sdfInfo sdfSphere(struct coordinate point, struct coordinate position, float rad) {
    struct sdfInfo sphere;

    sphere.dist = sqrt(pow(point.x-position.x, 2) + pow(point.y-position.y, 2) + pow(point.z-position.z, 2)) - rad;
    sphere.r = 0;
    sphere.g = 0;
    sphere.b = 255;

    return sphere;
}

struct sdfInfo signedDistance(float x, float y, float z, int time) {
    struct coordinate pos;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;

    struct coordinate point;
    point.x = x;
    point.y = y;
    point.z = z;
    
    return unify(
        negate(
            sdfCube(rotate(point, pos, ((float)time/10)), pos, 1, 180, 0, 255),
            sdfSphere(point, pos, 1.2)
        ),

        negate(
            sdfCube(point, pos, 10),
            sdfCube(point, pos, 9)
        )
    );
}

float signedDistanceOnly(float x, float y, float z, int time) {
    struct sdfInfo result = signedDistance(x, y, z, time);
    return result.dist;
}

//float sdfInfinite(float x, float y, float z) {
//    return signedDistance(fmod(x, 5.0), fmod(y, 5.0), fmod(z, 5.0));
//}


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
    auto cast = [](int color_out[], float scan_pitch, float scan_yaw, float cam_pitch, float cam_yaw, float cam_x, float cam_y, float cam_z, int slices, int frame) {
        float* vector = ray_vector(scan_pitch, scan_yaw, cam_pitch, cam_yaw);

        float point[3] = {cam_x, cam_y, cam_z};

        // Default/empty color
        color_out[0] = 255;
        color_out[1] = 255;
        color_out[2] = 255;

        float thresh = 0.01; // When the program decides that the distance is close enough to change the color of the pixel

        for (int s=0; s<64; s++) {
            struct sdfInfo sdf = signedDistance(point[0], point[1], point[2], frame);

            // Ray has hit object
            if (sdf.dist < thresh) {
                float eps = 0.02;
                float norm_X = (signedDistanceOnly(point[0]+eps, point[1], point[2], frame) - signedDistanceOnly(point[0]-eps, point[1], point[2], frame)) / (eps*2);
                float norm_Y = (signedDistanceOnly(point[0], point[1]+eps, point[2], frame) - signedDistanceOnly(point[0], point[1]-eps, point[2], frame)) / (eps*2);
                float norm_Z = (signedDistanceOnly(point[0], point[1], point[2]+eps, frame) - signedDistanceOnly(point[0], point[1], point[2]-eps, frame)) / (eps*2);

                float light[3] = {1.0, 1.0, 1.0};

                // See how close it is to the light normal (average difference of normals)
                float lit_factor = ((abs(norm_X-light[0]) + abs(norm_Y-light[1]) + abs(norm_Z-light[2]))/3)/2;
                // Flip because normals facing opposite to the light should be the most well lit
                lit_factor = 1-lit_factor;


                color_out[0] = sdf.r*lit_factor; //100-s*6;
                color_out[1] = sdf.g*lit_factor;
                color_out[2] = sdf.b*lit_factor;
                return;
            }

            point[0] += vector[0]*sdf.dist;
            point[1] += vector[1]*sdf.dist;
            point[2] += vector[2]*sdf.dist;
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

                cast(ray_color, rel_x, rel_y, camera_rx_rad, camera_ry_rad, camera_x, camera_y, camera_z, 8, frame);

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
