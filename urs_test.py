from ursina import *
from ursina.shaders import basic_lighting_shader
import math
import numpy as np

app = Ursina()

EditorCamera()

Entity(model="cube", shader=basic_lighting_shader)


# Generate the fan pattern
# offset those points using camera rotation


def ray_vector(scan_pitch, scan_yaw, cam_pitch, cam_yaw):
    # Scan pitch is the horizontal angle that the ray is cast from the camera. You can guess what scan yaw is

    x = math.cos(scan_pitch) * math.sin(scan_yaw)
    y = -math.sin(scan_pitch)
    z = math.cos(scan_pitch) * math.cos(scan_yaw)

    # This is the position relative to the camera. It would be the points position if the camera was located at 0 and with no rotation
    # The point needs to be offset according to the camera's rotation

    # I can imagine a triangle with one point at the camera, another at the point that was just 
    # calculated, and another with a right angle at the same level a the first point.
    # The hypotenuse (the distance from the point to the camera) is one, one unit away from the camera
    # And the other sides are the offset on different axes from the camera

    # First, the points will be rotated around the camera thinking in 2D, from the side, ignoring the x axis for now.

    # Again, I will imagine a triangle
    # The hypotenuse is the distance from the camera to the point
    # In 3D space, the distance is 1, but thinking in 2D, the distances are different
    # I must change the angle while preserving the length of the hypotenuse

    # Using pythagorean, I can find the hypot/distance in 2d space:
    distance = math.sqrt(math.pow(z, 2) + math.pow(y, 2))

    # Now calculate the new offsets based on the camera x/pitch angle
    # Opposite is y distance, Adjacent is z distance
    # SOH = sin(cam_pitch) = opposite / hypot
    y = math.sin(scan_pitch+cam_pitch)*distance # Reassign offset

    # Repeat for CAH (cos adjacent hypot)
    z = math.cos(scan_pitch+cam_pitch)*distance


    # IT'S VERY IMPORTANT THAT THE TOP DOWN TRANSFORM HAPPENS AFTER THE FROM THE SIDE TRANSFORM
    # That was my first attempt, and it confused me

    # Repeat from top down

    distance = math.sqrt(math.pow(x, 2) + math.pow(z, 2))

    # The angle may be changed/inverted by the previous operation. It needs to be recaptured
    # TOA can be used to get the angle with the opposide and adjacent sides of the imaginary triangle
    # tan(theta) = opp/adj
    # solve for theta: 
    # theta = atan(opp/adj)
    # math is so cool for real

    angle = math.atan2(x, z)

    print(angle)

    z = math.sin(angle+cam_yaw)*distance
    x = math.cos(angle+cam_yaw)*distance


    # Now the points can be rotated around the camera on the y/yaw axis.
    # Up and down is more difficult because the points are already rotated
    # They still have the same positions on the camera's coordinate system, so
    # the offsets will be calculated like before, but they will need to be
    # converted from the camera's coordinates to the world's coordinates

    return x, y, z



voxels = np.ones((4, 4, 4))

ray_cubes = []
def ray(scan_pitch, scan_yaw, cam_pitch, cam_yaw, origin, sliceLength, slices):
    vector = ray_vector(scan_pitch, scan_yaw, cam_pitch, cam_yaw)
    current = origin

    for s in range(slices):
        for v in voxels:
            voxpos = [
                round(current[0]),
                round(current[1]),
                round(current[2])
            ]

            if voxpos[0] in voxels:
                if voxpos[1] in voxels[voxpos[0]]:
                    if voxpos[2] in voxels[voxpos[0]][voxpos[1]]:
                        return s #voxels[voxpos[0]][voxpos[1]][voxpos[2]]

        current[0] += vector[0]*sliceLength
        current[1] += vector[1]*sliceLength
        current[2] += vector[2]*sliceLength

        ray_cubes.append(Entity(model="cube", shader=basic_lighting_shader, position=(current[0], current[1], current[2]), scale=0.04))


rotation_x = 0
rotation_y = 0

def refresh_rays():
    for e in ray_cubes:
        destroy(e)

    width, height = 10, 4
    for y in range(height):
        for x in range(width):
            cast = ray(((x+0.5)/width)-0.5, ((y+0.5)/height)-0.5, rotation_x, rotation_y, [0, 0, -2], .1, 10)
            if cast:
                #pygame.draw.rect(screen, (200-(cast*10), 50, 50), (x, y, 1, 1))
                pass
                #Entity(model="cube", shader=basic_lighting_shader)

def input(key):
    global rotation_x, rotation_y



    if key == "r":
        rotation_x+=1
        refresh_rays()

    if key == "o":
        rotation_y+=1
        refresh_rays()
        




app.run()