### Camera.c ##
#cdef extern from "./SDL/camera.h":
#    cdef struct Camera: #maybe public?
#        float fov
#        float x_size
#        float y_size
#        float z_near
#        float z_far
#        float x
#        float y
#        float z
#        float x_angle
#        float y_angle
#        float xl, yl, zl
#        float xu, yu, zu
#        float ratio

#cdef extern from "./SDL/camera.h":
#    int _world_projection(Camera* camera)
#    int _hud_projection(Camera* camera)
#    int _set_camera(Camera* c)
#    void _set_camera_state(float x, float y, float z, float theta, float phi)

#def set_projection(float x, float y, float z, float x_angle, float y_angle):
#    _set_camera_state(x,y,z, x_angle, y_angle);

## SDL functions ##
cdef extern from "./SDL/SDL_functions.h":
    int _set_resolution(int xres, int yres, int fullscreen)
    int _init_video()
    int _del_video()
    int _swap_buffers()
    int _get_ticks()

def get_ticks():
    return _get_ticks()

## Settings ##
def set_resolution(xres, yres, fullscreen = 0):
    _set_resolution(xres, yres, fullscreen)

## Draw functions ##
cdef extern from "./SDL/draw_functions.h":
    int _draw_point(int r, int g,int b, float x0, float y0, float z0)
    int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1)
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)
    int _draw_rect(int r, int g, int b, float x, float y, float w, float h)
    int _draw_border_rect(int r, int g, int b, float x, float y, float w, float h)

def draw_line(int r, int g, int b, float x0, float y0, float z0, float x1, float y1, float z1):
    return _draw_line(r,g,b,x0,y0,z0,x1,y1,z1)

def draw_point(int r, int g, int b, float x0, float y0, float z0):
    return _draw_point(r,g,b,x0,y0,z0)

def draw_rect(int r, int g, int b, float x, float y, float w, float h):
    return _draw_rect(r,g,b, x,y, w,h)

def draw_border_rect(int r, int g, int b, float x, float y, float w, float h):
    return _draw_border_rect(r,g,b, x,y, w,h)

## Window Properties ##
cdef class Window:
    cdef int w
    cdef int h

def close(self):
    print "Deconstructing SDL OpenGL Window"
    _del_video()

def flip(self):
    _swap_buffers()


'''
Particles
'''
cdef extern from "./SDL/particle_functions.h":
    int _draw_particle(int id, float size, float x, float y, float z)
    int _draw_particle2(int id, float size, float x, float y, float z)
    int _planar_laser(float x0, float y0, float z0, float x1, float y1, float z1)
    int _planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1)

def draw_particle(id,size, x,y,z):
    _draw_particle2(id, size, x, y, z)

def planar_laser(float x0, float y0, float z0, float x1, float y1, float z1):
    _planar_laser(x0, y0, z0, x1, y1, z1)

def planar_laser2(int density, float width, float x0, float y0, float z0, float x1, float y1, float z1):
    _planar_laser2(density, width, x0, y0, z0, x1, y1, z1)
