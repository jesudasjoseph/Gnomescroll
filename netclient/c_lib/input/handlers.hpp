#pragma once

typedef enum InputStateMode
{
    INPUT_STATE_AGENT,
    INPUT_STATE_CAMERA
} InputStateMode;

struct InputState
{
    bool mouse_bound;
    bool help_menu;
    bool inventory;
    bool scoreboard;
    bool map;
    bool chat;
    bool full_chat;
    bool hud;
    bool debug;
    bool skeleton_editor;
    bool graphs;
    
    bool has_focus;
    bool rebind_mouse;

    bool can_jump;
    bool confirm_quit;
    bool quit;

    InputStateMode input_mode;
    InputStateMode camera_mode;

    // options
    bool invert_mouse;
    float sensitivity;
};

extern InputState input_state;

void init_handlers();

// handlers
void key_down_handler(SDL_Event* event);
void key_up_handler(SDL_Event* event);
void mouse_button_down_handler(SDL_Event* event);
void mouse_button_up_handler(SDL_Event* event);
void mouse_motion_handler(SDL_Event* event);
void quit_event_handler(SDL_Event* event);
void active_event_handler(SDL_Event* event);
void key_state_handler(Uint8 *keystate, int numkeys);

// triggers
void toggle_mouse_bind();
void toggle_help_menu();
void toggle_inventory();
void toggle_scoreboard();
void toggle_map();
void toggle_chat();
void toggle_skeleton_editor();

void enable_jump();
void enable_quit();

void toggle_input_mode();
void toggle_camera_mode();

// options
void set_input_options(
    bool invert_mouse,
    float sensitivity
);

