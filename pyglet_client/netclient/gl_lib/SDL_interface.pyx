
cimport libc.stdlib

#cdef extern from "SDL.h":

## Camera.c ##
cdef class Camera: #maybe public?
    float fov, x_size, y_size, z_near, z_far
    float x,y,z, x_angle,y_angle

cdef extern int _world_projection(Camera camera)
cdef extern int _hud_projection(Camera camera)

## End Camera.c ##

## Texture Loader ##

cdef extern from "SDL.h":
    struct SDL_Surface:
        int w
        int h

cdef extern from "texture_loader.h":
    int _init_image_loader()
    SDL_Surface* _load_image(char *file)
    int _create_hud_texture(char *file)
    int _create_block_texture(char *file)
    int _create_texture(SDL_Surface* surface)

cdef SDL_Surface* load_image(char* file):
    cdef SDL_Surface* surface
    surface = _load_image(file)
    return surface

cdef create_texture(SDL_Surface* surface, type =None):
    if type == None:
        _create_texture(surface)

## SDL functions ##

cdef extern int _init_video()
cdef extern int _del_video()
cdef extern int _swap_buffers()
cdef extern int _get_ticks()

def get_ticks():
    return _get_ticks()

## Draw functions ##
cdef extern from "draw.h":
    int _draw_point(int r, int g,int b, float x0, float y0, float z0)
    int _draw_line(int r, int g,int b, float x0, float y0, float z0, float x1, float y1, float z1)
    int _blit_sprite(int tex, float x0, float y0, float x1, float y1, float z)

def draw_line(int r, int g, int b, float x0, float y0, float z0, float x1, float y1, float z1):
    return _draw_line(r,g,b,x0,y0,z0,x1,y1,z1)

def draw_point(int r, int g, int b, float x0, float y0, float z0):
    return _draw_point(r,g,b,x0,y0,z0)

## Window Properties ##

cdef class Window:
    cdef int w
    cdef int h

cdef class Texture:
    cdef int id
    cdef int w
    cdef int h
    cdef SDL_Surface* surface

    def __init__(self, file, texture_type =None):
        if texture_type == None:
            self.surface = load_image(file)
        elif texture_type == "mipmapped":
            self.surface = _create_block_texture("./texture/textures_01.png")
        if self.surface == 0:
            print "Error Loading Texture: " + str(file)
        self.w = self.surface.w
        self.h = self.surface.h
        self.id = create_texture(self.surfaces, texture_type)

cdef class Textures:
    cdef Texture hud_tex
    cdef Texture tile_tex

    def __init__(self):
        self.hud_tex = Texture("./texture/target.png", "mipmapped")
        self.tile_tex = Texture("./texture/textures_01.png")

cdef class Global:
    cdef Camera camera
    cdef Window window
    cdef Textures textures
#    cdef Window window

    #make field of view adjustable!
    def init(self):
        print "Creating SDL OpenGL Window"
        self.set_aspect(85.0 ,800.0, 600.0, 0.1, 1000.0)
        self.set_projection(0.,0.,0.,0.,0.)
        _init_video()
        #input init
        _init_input()
        _init_image_loader()

    def close_window(self):
        print "Deconstructing SDL OpenGL Window"
        _del_video()

    def flip(self):
        _swap_buffers()

    #camera
    def set_aspect(self, float fov, float x_size, float y_size, float z_near, float z_far):
        self.camera.fov = fov
        self.camera.x_size = x_size
        self.camera.y_size = y_size
        self.camera.z_near = z_near
        self.camera.z_far = z_far

    def set_projection(self, float x, float y, float z, float x_angle, float y_angle):
        self.camera.x = x
        self.camera.y = y
        self.camera.z = z
        self.camera.x_angle = x_angle
        self.camera.y_angle = y_angle

    def world_projection(Global self):
        _world_projection(self.camera)

    def hud_projection(Global self):
        _hud_projection(self.camera)

### init

ctypedef struct MouseMotion:
    int x
    int y
    int dx
    int dy
    int button

ctypedef struct MouseEvent:
    int x
    int y
    int button
    int state


ctypedef unsigned char Uint8

### call backs
cdef extern from "input.h":
    ctypedef int (*key_state_func)(Uint8* keystate, int numkeys)
    int _key_state_callback(key_state_func user_func, Uint8* keystate, int numkeys)

    ctypedef int (*key_event_func)(char key)
    int _key_event_callback(key_event_func user_func, char key)

    ctypedef int (*mouse_motion_func)(MouseMotion ms)
    int _mouse_motion_callback(mouse_motion_func user_func, MouseMotion)

    ctypedef int (*mouse_event_func)(MouseEvent me)
    int _mouse_event_callback(mouse_event_func user_func, MouseEvent me)

    ctypedef int (*key_text_event_func)(char key, char* key_name)
    int _key_text_event(key_text_event_func user_func, char key, char* key_name)

## input.c
cdef extern int _init_input()
cdef extern int _get_key_state(key_state_func key_state_cb)
cdef extern int _process_events(mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb, key_text_event_func keyboard_text_event_cb)
cdef extern int _set_text_entry_mode(int n)

def get_key_state():
    _get_key_state(&key_state_callback)

def process_events():
    temp = _process_events(&mouse_event_callback, &mouse_motion_callback, &key_event_callback, &key_text_event_callback)
    #mouse_event_func mouse_event_cb, mouse_motion_func mouse_motion_cb, key_event_func keyboard_event_cb)
def set_text_entry_mode(int n):
    temp = _set_text_entry_mode(n)

#cpdef int call_back_test():
#    _key_event_callback(&key_event_callback, 42)

import input

cdef int key_state_callback(Uint8* keystate, int numkeys):
    pressed_keys = []
    for i in range(0, numkeys):
        if keystate[i] != 0:
            pressed_keys.append(i)
    input.inputEventGlobal.keyboard_state(pressed_keys)

cdef int key_event_callback(char key):
    input.inputEventGlobal.keyboard_event(key)
    #print "event"
    pass

cdef int key_text_event_callback(char key, char* key_name):
    cdef bytes py_string
    py_string = key_name
    key_string = key_name.decode('ascii')
    #print "keystring= " + key_string
    input.inputEventGlobal.keyboard_text_event(key, key_string)
    pass

cdef int mouse_motion_callback(MouseMotion ms):
    input.inputEventGlobal.mouse_motion(ms.x,ms.y,ms.dx,ms.dy, ms.button)
    #print "(dx,dy)= %i, %i" % (ms.dx, ms.dy)
    pass

cdef int mouse_event_callback(MouseEvent me):
    input.inputEventGlobal.mouse_event(me.button, me.state, me.x, me.y)
    #print str(me.state)
    pass

SDL_global = Global()
