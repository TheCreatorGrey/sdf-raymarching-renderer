#define PI 3.1415926538

// Thx stack overflow for this function :3
float atan2(in float y, in float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI/2.0 - atan(x,y), atan(y,x), s);
}

vec3 ray_vector(in vec2 cam_ro, in vec2 scan)
{
    float x = sin(scan.x);
    float y = cos(scan.x) * sin(scan.y);
    float z = cos(scan.x) * cos(scan.y);
    
    float dist = sqrt(pow(z, 2.0) + pow(y, 2.0));
    float angle = atan2(z, y);
    
    y = sin(angle+cam_ro.x)*dist;
    z = cos(angle+cam_ro.x)*dist;
    
    
    dist = sqrt(pow(x, 2.0) + pow(z, 2.0));
    angle = atan2(x, z);
    
    z = sin(angle+cam_ro.y)*dist;
    x = cos(angle+cam_ro.y)*dist;
    
    return vec3(x, y, z);
}

vec3 ray(in vec2 cam_ro, in vec2 scan, in vec3 cam_pos)
{
    vec3 color = vec3(255, 255, 255);
    
    vec3 ray_vec = ray_vector(cam_ro, scan);
    int steps = 64;
    float step_len = 0.1;
    
    vec3 point = cam_pos;
    int step_n = 0;
    while (step_n < steps) 
    {
        vec3 voxel = round(point);
        
        
        
        color.b -= 1.0;
        
        if ((0.0 < voxel.x) && (voxel.x < 6.0)) {
            if ((0.0 < voxel.y) && (voxel.y < 6.0)) {
                if ((0.0 < voxel.z) && (voxel.z < 6.0)) {
                    color = vec3(255, 0, 0);
                }
            }
        }
        
        point.x += ray_vec.x*step_len;
        point.y += ray_vec.y*step_len;
        point.z += ray_vec.z*step_len;
        
        step_n += 1;
    }
    
    return color;
}


vec3 camera_pos = vec3(0, 0, 0);
vec2 camera_ro = vec2(0, 0);

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{    
    vec2 camera_ro_rad = (PI / 180.0) * camera_ro;
    
    camera_ro.x = iTime;
    
    vec2 rel_coord = (fragCoord/iResolution.xy)-0.5;
    
    vec3 col = ray(camera_ro, rel_coord, camera_pos);
    


    // Output to screen
    fragColor = vec4(col,1.0);
}