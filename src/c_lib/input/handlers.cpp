/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "handlers.hpp"

#ifdef __clang__
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#include <math.h>
#include <chat/_interface.hpp>
#include <input/skeleton_editor.hpp>
#include <input/equipped_sprite_adjuster.hpp>
#include <hud/_interface.hpp>
#include <hud/container/_interface.hpp>
#include <hud/hud.hpp>

//toggling graphics settings
#include <t_map/glsl/shader.hpp>

InputState input_state;
bool used_freecam_yet = false;

bool mouse_unlocked_for_ui_element()
{   // if mouse was unlocked to allow control of a ui element
    return (input_state.container_block || input_state.agent_inventory || input_state.awesomium);
}

// triggers
void toggle_mouse_bind()
{
    set_mouse_bind(!input_state.mouse_bound);
}

void toggle_help_menu()
{
    input_state.help_menu = (!input_state.help_menu);
    if (input_state.help_menu)
        Hud::clear_prompt(Hud::press_help_text);
}

void enable_agent_inventory()
{
    // we check "did_open_container_block" here, in case agent container is opened with container block in same frame
    if (input_state.agent_inventory || ItemContainer::did_open_container_block) return;

    input_state.agent_inventory = true;

    // release all toolbelt
    Toolbelt::left_trigger_up_event();
    Toolbelt::right_trigger_up_event();

    HudContainer::enable_agent_inventory_hud();
    ItemContainer::open_inventory();
    set_mouse_rebind(input_state.mouse_bound);
    set_mouse_bind(false);
}

void disable_agent_inventory()
{
    if (!input_state.agent_inventory) return;
    input_state.agent_inventory = false;
    // poll mouse button state
    // if left or right is unpressed, trigger an up event
    int x = 0;
    int y = 0;
    Uint8 btns = SDL_GetMouseState(&x, &y);
    if (!(btns & SDL_BUTTON_LEFT)) Toolbelt::left_trigger_up_event();
    if (!(btns & SDL_BUTTON_RIGHT)) Toolbelt::right_trigger_up_event();

    HudContainer::disable_agent_inventory_hud();
    ItemContainer::close_inventory();
    //if (input_state.input_focus)    // dont change mouse state if we're not in focus. it grabs the window
    if (input_state.input_focus)    // dont change mouse state if we're not in focus. it grabs the window
        set_mouse_bind(input_state.rebind_mouse);
    input_state.ignore_mouse_motion = true;
}

void toggle_agent_inventory()
{
    if (input_state.agent_inventory) disable_agent_inventory();
    else enable_agent_inventory();
}

void enable_container_block(ItemContainerID container_id)
{
    if (input_state.container_block || input_state.agent_inventory) return;

    GS_ASSERT(container_id != NULL_CONTAINER);

    // release all toolbelt
    Toolbelt::left_trigger_up_event();
    Toolbelt::right_trigger_up_event();

    input_state.container_block = true;
    input_state.container_block_id = container_id;
    HudContainer::enable_container_block_hud(container_id);
    set_mouse_rebind(input_state.mouse_bound);
    set_mouse_bind(false);
}

void disable_container_block()
{
    if (!input_state.container_block || input_state.agent_inventory) return;
    input_state.container_block = false;
    HudContainer::disable_container_block_hud();
    if (input_state.input_focus)    // dont change mouse state if we're not in focus. it grabs the window
        set_mouse_bind(input_state.rebind_mouse);
    input_state.ignore_mouse_motion = true;

    // if we are requesting the close client-side, then we need to tell the
    // container module and server.
    // else, it was already closed by the container module
    ItemContainerID container_id = ItemContainer::opened_container;
    if (container_id == NULL_CONTAINER) return;
    ItemContainer::close_container(container_id);
    ItemContainer::send_container_close(container_id);
}

void close_all_containers()
{
    disable_container_block();
    disable_agent_inventory();
}

void toggle_scoreboard()
{
    input_state.scoreboard = (!input_state.scoreboard);
}

void toggle_map()
{
    input_state.map = (!input_state.map);
}

void disable_chat()
{
    if (!input_state.chat) return;
    input_state.chat = false;
}

void enable_chat()
{
    if (input_state.chat) return;
    input_state.chat = true;
}

void toggle_chat()
{
    if (input_state.chat)
        disable_chat();
    else
        enable_chat();
}

void toggle_full_chat()
{
    input_state.full_chat = (!input_state.full_chat);
}

void toggle_hud()
{
    input_state.draw_hud = (!input_state.draw_hud);
}

void toggle_admin_controls()
{
    input_state.admin_controls = (!input_state.admin_controls);
}

void enable_login_window()
{
    #if GS_AUTH
    //if (input_state.login_window) return;
    //input_state.login_window = true;
    //update_awesomium_window_state();
    //Hud::clear_prompt(Hud::open_login_text);
    #endif
}

void disable_login_window()
{
    #if GS_AUTH
    if (!input_state.login_window) return;
    input_state.login_window = false;
    update_awesomium_window_state();

    if (!mouse_unlocked_for_ui_element())
    {   // dont manipulate the mouse state if we opened on top of containers
        if (input_state.input_focus)    // dont change mouse state if we're not in focus. it grabs the window
            set_mouse_bind(input_state.rebind_mouse);
        input_state.ignore_mouse_motion = true;
    }
    #endif
}

void toggle_login_window()
{
    #if GS_AUTH
    if (input_state.login_window)
        disable_login_window();
    else
        enable_login_window();
    #endif
}

void toggle_settings_window()
{
    #if GS_AWESOMIUM
    if (input_state.settings_window)
        disable_settings_window();
    else
        enable_settings_window();
    #endif
}

void disable_settings_window()
{
    #if GS_AWESOMIUM
    input_state.settings_window = false;
    update_awesomium_window_state();
    #endif
}

void enable_settings_window()
{
    #if GS_AWESOMIUM
    input_state.settings_window = true;
    update_awesomium_window_state();
    #endif
}

void update_awesomium_window_state()
{
    #if GS_AWESOMIUM
    bool on = (input_state.settings_window || input_state.login_window);
    if ((on && input_state.awesomium) || (!on && !input_state.awesomium))
        return;
    input_state.awesomium = on;
    if (on)
    {
        if (!mouse_unlocked_for_ui_element())
        {   // dont manipulate the mouse state if we opened on top of containers
            set_mouse_rebind(input_state.mouse_bound);
            set_mouse_bind(false);
        }
        Awesomium::enable();
    }
    else
    {
        if (!mouse_unlocked_for_ui_element())
        {   // dont manipulate the mouse state if we opened on top of containers
            if (input_state.input_focus)    // dont change mouse state if we're not in focus. it grabs the window
                set_mouse_bind(input_state.rebind_mouse);
            input_state.ignore_mouse_motion = true;
        }
        Awesomium::disable();
    }
    #endif
}

void disable_awesomium_windows()
{
    disable_login_window();
    disable_settings_window();
}

void enable_jump()
{
    input_state.can_jump = true;
}

void enable_quit()
{   // release mouse
    unbind_mouse();
    input_state.quit = true;
}

void toggle_confirm_quit()
{
    #if PRODUCTION
    input_state.confirm_quit = (!input_state.confirm_quit);
    #else
    input_state.confirm_quit = false;
    enable_quit();  // quit automatically in debug
    #endif
}

void toggle_skeleton_editor()
{
    input_state.skeleton_editor = (!input_state.skeleton_editor);
    if (input_state.skeleton_editor)
        printf("Skeleton editor enabled\n");
    else
        printf("Skeleton editor disabled\n");
}

void toggle_equipped_sprite_adjuster()
{
    input_state.equipped_sprite_adjuster = (!input_state.equipped_sprite_adjuster);
    if (input_state.equipped_sprite_adjuster)
        printf("Equipped sprite adjuster enabled\n");
    else
        printf("Equipped sprite adjuster disable\n");
}

void toggle_input_mode()
{
    switch (input_state.input_mode)
    {
        case INPUT_STATE_AGENT:
            Toolbelt::left_trigger_up_event(); // clear any trigger events
            Toolbelt::right_trigger_up_event(); // clear any trigger events
            input_state.input_mode = INPUT_STATE_CAMERA;
            break;
        case INPUT_STATE_CAMERA:
            if (ClientState::player_agent.you() != NULL)
                input_state.input_mode = INPUT_STATE_AGENT;
            break;
        default: break;
    }
}

void toggle_camera_mode()
{
    switch (input_state.camera_mode)
    {
        case INPUT_STATE_AGENT:
            if (!used_freecam_yet)
            { // put cam right over avatars head (usually we'll go back to wherever cam was last)
                used_freecam_yet = true;
                free_camera->copy_state_from(agent_camera);
                free_camera->move(0, 0, 7);
            }
            input_state.camera_mode = INPUT_STATE_CAMERA;
            break;
        case INPUT_STATE_CAMERA:
            if (ClientState::player_agent.you() != NULL)
                input_state.camera_mode = INPUT_STATE_AGENT;
            break;
        default: break;
    }
}

void init_input_state()
{   // set input_state defaults

    set_mouse_bind(Options::mouse_bound);
    #if PRODUCTION
    input_state.input_mode = INPUT_STATE_AGENT;
    input_state.camera_mode = INPUT_STATE_AGENT;
    #else
    input_state.input_mode = INPUT_STATE_CAMERA;
    input_state.camera_mode = INPUT_STATE_CAMERA;
    #endif

    input_state.login_mode = false;

    set_mouse_rebind(input_state.mouse_bound);

    input_state.draw_hud = true;
    input_state.vbo_debug = false;
    input_state.diagnostics = false;
    input_state.admin_controls = false;

    input_state.help_menu = false;
    input_state.scoreboard = false;
    input_state.map = false;
    input_state.chat = false;
    input_state.full_chat = false;
    //input_state.hud = true;
    input_state.skeleton_editor = false;
    input_state.terminal_is_open = false;

    input_state.can_jump = true;
    input_state.quit = false;

    input_state.ignore_mouse_motion = false;
    input_state.last_mouse_button_down_event_frame = -1;
    input_state.last_mouse_button_up_event_frame = -1;

    // containers
    input_state.agent_inventory = false;
    input_state.container_block = false;
    input_state.container_block_id = NULL_CONTAINER;

    // options
    input_state.invert_mouse = false;
    input_state.sensitivity = 100.0f;

    // debug
    input_state.frustum = true;

    // awesomium
    input_state.awesomium = false;
    input_state.login_window = false;
    input_state.settings_window = false;

    input_state.error_message = Hud::has_error();

    // SDL state
    // these starting conditions are variable, so dont rely on them for deterministic logic
    Uint8 app_state = SDL_GetAppState();
    input_state.input_focus = (app_state & SDL_APPMOUSEFOCUS);
    input_state.mouse_focus = (app_state & SDL_APPINPUTFOCUS);
    input_state.app_active   = (app_state & SDL_APPACTIVE);
}

// options
const float ZOOM_SENSITIVITY_SCALE = 0.7f;
void update_input_state()
{
    input_state.invert_mouse = Options::invert_mouse;
    input_state.sensitivity = Options::sensitivity;
    if (Hud::hud_draw_settings.zoom)
        input_state.sensitivity *= ZOOM_SENSITIVITY_SCALE;

    Uint8 app_state = SDL_GetAppState();
    input_state.input_focus = (app_state & SDL_APPMOUSEFOCUS);
    input_state.mouse_focus = (app_state & SDL_APPINPUTFOCUS);
    input_state.app_active   = (app_state & SDL_APPACTIVE);

    bool had_error = input_state.error_message;
    input_state.error_message = Hud::has_error();
    if (!had_error && input_state.error_message)
    {
        disable_agent_inventory();
        disable_container_block();
        disable_chat();
    }
}

// keys that can be held down
const int KEY_HOLD_DELAY = 20;  // in ticks
const int KEY_HOLD_RATE = 2;    // 15 pulses per second

typedef enum {
    kBACKSPACE = 0,
    kLEFT_ARROW,
    kRIGHT_ARROW,
    kUP_ARROW,
    kDOWN_ARROW
} HeldDownKey;

typedef struct {
    bool pressed;
    int t;
} KeyRateLimit;

const int KEYS_HELD_DOWN = 5;
static KeyRateLimit keys_held_down[KEYS_HELD_DOWN];
static const SDLKey keys_held_down_map[KEYS_HELD_DOWN] = {
    SDLK_BACKSPACE,
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_UP,
    SDLK_DOWN
};

static void update_keys_held_down()
{
    for (int i=0; i<KEYS_HELD_DOWN; i++)
    {
        if (keys_held_down[i].pressed)
            keys_held_down[i].t++;
        else
            keys_held_down[i].t = 0;
    }
}


void trigger_keys_held_down()
{
    void chat_key_down_handler(SDL_Event* event);   // forward decl
    if (!input_state.chat) return;

    update_keys_held_down();
    static SDL_Event event;
    for (int i=0; i<KEYS_HELD_DOWN; i++)
    {
        if (keys_held_down[i].t > KEY_HOLD_DELAY
          && (keys_held_down[i].t - KEY_HOLD_DELAY) % KEY_HOLD_RATE == 0)
        {
            event.user.code = SDL_EVENT_USER_TRIGGER;
            event.key.keysym.sym = keys_held_down_map[i];
            chat_key_down_handler(&event);
        }
    }
}

/* Chat buffer */

void terminal_key_down_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_ESCAPE:
            input_state.terminal_is_open = false;
            return;
        case SDLK_BACKSPACE:
            Hud::terminal_renderer.left();
            Hud::terminal_renderer.add(' ');
            Hud::terminal_renderer.left();
            return;
        case SDLK_RETURN:
            Hud::terminal_renderer.down();
            Hud::terminal_renderer.set_cursor_x(0);
            return;
        case SDLK_LEFT:
            Hud::terminal_renderer.left();
            return;
        case SDLK_RIGHT:
            Hud::terminal_renderer.right();
            return;
        case SDLK_UP:
            Hud::terminal_renderer.up();
            return;
        case SDLK_DOWN:
            Hud::terminal_renderer.down();
            return;
        default: break;
    }

    int t = getUnicodeValue(event->key.keysym);
    t = (t) ? t : event->key.keysym.sym;

    if (t < 0 || t > 127)
        return;

    Hud::terminal_renderer.add((char)t);
}

void chat_key_down_handler(SDL_Event* event)
{
    using Chat::chat_client;
    if (chat_client == NULL) return;

    switch (event->key.keysym.sym)
    {
        case SDLK_ESCAPE:
            chat_client->input->clear_buffer();
            toggle_chat();
            return;
        case SDLK_BACKSPACE:
            chat_client->input->remove();
            return;
        case SDLK_RETURN:
            chat_client->submit();
            toggle_chat();
            return;
        case SDLK_LEFT:
            chat_client->input->cursor_left();
            return;
        case SDLK_RIGHT:
            chat_client->input->cursor_right();
            return;
        case SDLK_UP:
            chat_client->input->history_older();
            return;
        case SDLK_DOWN:
            chat_client->input->history_newer();
            return;
        //case SDLK_PAGEUP:
            //if (hud->inited && hud->chat != NULL && hud->chat->inited)
                //hud->chat->page_up();
            //return;
        //case SDLK_PAGEDOWN:
            //if (hud->inited && hud->chat != NULL && hud->chat->inited)
                //hud->chat->page_down();
            //return;
        default: break;
    }

    int t = getUnicodeValue(event->key.keysym);
    t = (t) ? t : event->key.keysym.sym;

    if (t < 0 || t > 127)
        return;

    chat_client->input->add((char)t);
}

void chat_key_up_handler(SDL_Event* event){}
void chat_mouse_down_handler(SDL_Event* event){}
void chat_mouse_up_handler(SDL_Event* event){}
void chat_mouse_motion_handler(SDL_Event* event){}

void auth_form_key_down_handler(SDL_Event* event)
{
    using Hud::auth_screen;

    switch (event->key.keysym.sym)
    {
        case SDLK_TAB:
            auth_screen.tab();
            return;
        case SDLK_RETURN:
            auth_screen.enter();
            return;
        case SDLK_BACKSPACE:
            auth_screen.backspace();
            return;
        case SDLK_LEFT:
            auth_screen.cursor_left();
            return;
        case SDLK_RIGHT:
            auth_screen.cursor_right();
            return;
        default:
            break;
    }

    int t = getUnicodeValue(event->key.keysym);
    t = (t) ? t : event->key.keysym.sym;

    if (t < 0 || t > 127)
        return;

    auth_screen.insert(char(t));
}

void auth_form_mouse_up_handler(SDL_Event* event)
{
    Vec2i v;
    SDL_GetMouseState(&v.x, &v.y);
    v.y = _yres - v.y;

    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            Hud::auth_screen.click(v);
            break;
        default:
            return;
    }
}

void auth_form_mouse_motion_handler(SDL_Event* event)
{
    Vec2i v;
    SDL_GetMouseState(&v.x, &v.y);
    v.y = _yres - v.y;
    Hud::auth_screen.hover(v);
}


/* Container / HUD */

void container_key_down_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_e:
        case SDLK_ESCAPE:
            close_all_containers();
            break;

        case SDLK_t:
        case SDLK_RETURN:
            toggle_chat();
            Chat::use_global_channel();
            break;

        default: break;
    }
}

void container_key_up_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;
    if (you->status.dead) return;
}

void container_mouse_down_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;
    if (you->status.dead) return;

    // gets correct mouse pixels
    int x,y;
    SDL_GetMouseState(&x, &y);

    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            HudContainer::left_mouse_down(x,y);
            break;

        case SDL_BUTTON_RIGHT:
            HudContainer::right_mouse_down(x,y);
            break;

        default:
            return;
    }
}

void container_mouse_up_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;
    if (you->status.dead) return;

    Toolbelt::left_trigger_up_event(); // clear any trigger events
    Toolbelt::right_trigger_up_event(); // clear any trigger events

    if (!input_state.mouse_bound && // for whatever reason it only needs to be done when mouse is unbound
        input_state.ignore_next_right_click_event &&
        event->button.button == SDL_BUTTON_RIGHT)
    {
        input_state.ignore_next_right_click_event = false;
        return;
    }

    // check intersection with any slots

    //SDL_MouseButtonEvent e = event->button;
    //printf("Button type: %d\n", e.type);
    //printf("Button which: %d\n", e.which);
    //printf("Button button: %d\n", e.button);
    //printf("Button state: %d\n", e.state);
    //printf("Button x,y: %d,%d\n", e.x, e.y);  // reports 0,0 no matter what the mouse grab state is

    // gets correct mouse pixels
    int x,y;
    SDL_GetMouseState(&x, &y);
    //printf("GetMouseState x,y: %d,%d\n", x,y);

    HudContainer::ContainerInputEvent container_event;

    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            container_event = HudContainer::left_mouse_up(x,y);
            //printf("container event: ");
            //printf("id %d ", container_event.container_id);
            //printf("slot %d ", container_event.slot);
            //printf("synthesizer %d ", container_event.synthesizer);
            //printf("craft output %d ", container_event.craft_output);
            //printf("\n");
            ItemContainer::mouse_left_click_handler(container_event.container_id, container_event.slot, container_event.alt_action);
            break;

        case SDL_BUTTON_RIGHT:
            container_event = HudContainer::right_mouse_up(x,y);
            ItemContainer::mouse_right_click_handler(container_event.container_id, container_event.slot, container_event.alt_action);
            break;

        default:
            return;
    }
}

void container_mouse_motion_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;

    //SDL_MouseMotionEvent e = event->motion;

    //printf("Motion type: %d\n", e.type);
    //printf("Motion state: %d\n", e.state);
    //printf("Motion x,y: %d,%d\n", e.x, e.y);
    //printf("Motion xrel,yrel: %d,%d\n", e.xrel, e.yrel);

    int x,y;
    SDL_GetMouseState(&x, &y);
    //printf("X,Y %d,%d\n", x, y);

    /* Coordinates start at TOP LEFT */
    HudContainer::mouse_motion(x,y);
}


/* Agents::Agent */

void agent_key_down_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;

    HudContainer::ContainerInputEvent container_event;
    switch (event->key.keysym.sym)
    {
        //case SDLK_9:
            //t_mech::add_mech();
            //break;

        //case SDLK_0:
            //t_mech::rotate_mech();
            //break;

        case SDLK_LEFT:
            HudCubeSelector::cube_selector.left();
            break;
        case SDLK_RIGHT:
            HudCubeSelector::cube_selector.right();
            break;
        case SDLK_UP:
            HudCubeSelector::cube_selector.up();
            break;
        case SDLK_DOWN:
            HudCubeSelector::cube_selector.down();
            break;

        case SDLK_1:
            container_event = HudContainer::select_slot(1);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_2:
            container_event = HudContainer::select_slot(2);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_3:
            container_event = HudContainer::select_slot(3);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_4:
            container_event = HudContainer::select_slot(4);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_5:
            container_event = HudContainer::select_slot(5);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_6:
            container_event = HudContainer::select_slot(6);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_7:
            container_event = HudContainer::select_slot(7);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_8:
            container_event = HudContainer::select_slot(8);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;
        case SDLK_9:
            container_event = HudContainer::select_slot(9);
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;

        default: break;
    }

}

void agent_key_up_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;

    switch (event->key.keysym.sym)
    {
        case SDLK_SPACE:
            enable_jump();
            break;

        #if !PRODUCTION
        case SDLK_p:
            if (agent_camera != NULL)
                agent_camera->toggle_third_person();
            break;
        #endif

        default: break;
    }
}

void agent_mouse_down_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;

    HudContainer::ContainerInputEvent container_event;
    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            Toolbelt::left_trigger_down_event();
            break;

        case SDL_BUTTON_RIGHT:
            Toolbelt::right_trigger_down_event();
            break;

        case SDL_BUTTON_MIDDLE:
            // nothing
            break;

        case 4: // scroll up
            container_event = HudContainer::scroll_up();
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;

        case 5: // scroll down
            container_event = HudContainer::scroll_down();
            Toolbelt::toolbelt_item_selected_event(container_event.container_id, container_event.slot);
            break;

        default: break;
    }
}

void agent_mouse_up_handler(SDL_Event* event)
{
    class Agents::Agent* you = ClientState::player_agent.you();
    if (you == NULL) return;

    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            Toolbelt::left_trigger_up_event();
            break;

        case SDL_BUTTON_RIGHT:
            Toolbelt::right_trigger_up_event();
            break;

        case SDL_BUTTON_MIDDLE:
            // nothing
            break;

        default: break;
    }
}

void agent_mouse_motion_handler(SDL_Event* event)
{
}

void agent_key_state_handler(Uint8 *keystate, int numkeys,
    char *f, char *b, char *l, char *r,
    char *jet, char *jump, char *crouch, char *boost,
    char *charge, char *misc2, char *misc3)
{
    if (keystate[SDLK_w])
        *f = 1;
    if (keystate[SDLK_s])
        *b = 1;
    if (keystate[SDLK_a])
        *l = 1;
    if (keystate[SDLK_d])
        *r = 1;
    if (keystate[SDLK_LSHIFT])  // LSHIFT
        *jet = 1;
    if (keystate[SDLK_SPACE] || keystate[SDLK_PAGEDOWN])  // 2nd "bind" for CorpusC's funky keyboard
        *jump = 1;
    if (keystate[SDLK_LCTRL])  // LCTRL
        *crouch = 1;
}


/* Camera */

void camera_key_down_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_p:    // jump to agent camera
            if (free_camera != NULL && agent_camera != NULL)
                free_camera->copy_state_from(agent_camera);
            break;

        #if !PRODUCTION
        case SDLK_j:    // teleport our agent to the free camera
            if (free_camera != NULL)
                ClientState::player_agent.teleport_to(free_camera->get_position());
            break;
        #endif

        default: break;
    }
}

void print_mob_id()
{   // hitscan against mobs
    IF_ASSERT(current_camera == NULL) return;
    struct Vec3 p = current_camera->get_position();
    struct Vec3 v = current_camera->forward_vector();
    int ignore_id = -1;
    EntityType ignore_type = NULL_ENTITY_TYPE;
    class Voxels::VoxelHitscanTarget target;
    float vox_distance = 0.0f;
    Vec3 collision_point = vec3_init(0);
    bool voxel_hit = STATE::voxel_hitscan_list->hitscan(
        p, v, ignore_id, ignore_type, collision_point, vox_distance, target);
    if (!voxel_hit) return;
    printf("mob id: %d\n", target.entity_id);
}

void camera_key_up_handler(SDL_Event* event)
{
}

void camera_mouse_down_handler(SDL_Event* event)
{
    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            print_mob_id();
            break;

        case SDL_BUTTON_RIGHT:
            free_camera->toggle_zoom();
            break;

        default: break;
    }
}

void camera_mouse_up_handler(SDL_Event* event)
{
}

void camera_mouse_motion_handler(SDL_Event* event)
{
}

void camera_key_state_handler(Uint8 *keystate, int numkeys)
{
    const float speed = 0.8f;

    if (keystate[SDLK_w])
        free_camera->move(speed, 0,0);
    if (keystate[SDLK_s])
        free_camera->move(-speed,0,0);
    if (keystate[SDLK_a])
        free_camera->move(0, speed, 0);
    if (keystate[SDLK_d])
        free_camera->move(0,-speed,0);
    if (keystate[SDLK_r])
        free_camera->move(0,0,speed);
    if (keystate[SDLK_f])
        free_camera->move(0,0,-speed);
}

/*
 * Reference:
 * http://wiki.libsdl.org/moin.cgi/SDLKeycodeLookup
 */

// Handlers

// key down
void key_down_handler(SDL_Event* event)
{
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_down_handler(event);
        return;
    }

    if (input_state.equipped_sprite_adjuster)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_ESCAPE:
            case SDLK_l:
                toggle_equipped_sprite_adjuster();
                return;
            default:
                break;
        }

        EquippedSpriteAdjuster::key_down_handler(event);
        return;
    }

    if (input_state.login_mode)
    {
        if (event->key.keysym.sym == SDLK_ESCAPE)
            enable_quit();
        else
            auth_form_key_down_handler(event);
    }

    else if (input_state.awesomium)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_ESCAPE:
                if (input_state.settings_window)
                    disable_settings_window();
                else
                if (input_state.login_mode)
                    enable_quit();  // quit automatically if we are in login mode
                else
                    disable_awesomium_windows();
                break;

            default:
                Awesomium::SDL_keyboard_event(event);
                break;
        }
    }
    else if (input_state.terminal_is_open)
        terminal_key_down_handler(event);
    else if (input_state.chat)
        chat_key_down_handler(event);
    else if (input_state.agent_inventory || input_state.container_block)
        container_key_down_handler(event);
    else
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_down_handler(event);
        else
            camera_key_down_handler(event);

        // these should occur for both Agents::Agent and Camera
        switch (event->key.keysym.sym)
        {
            case SDLK_b:
                if (input_state.admin_controls)
                    Animations::agent_bleed(ClientState::player_agent.camera_position());
                break;

            case SDLK_g:
                if (input_state.admin_controls)
                    toggle_camera_mode();
                break;

            case SDLK_h:
                toggle_help_menu();
                break;

            case SDLK_k:
                if (input_state.admin_controls)
                    input_state.frustum = (!input_state.frustum);
                break;

            case SDLK_l:
                #if !PRODUCTION
                if (input_state.admin_controls)
                    toggle_equipped_sprite_adjuster();
                #endif
                break;

            case SDLK_SEMICOLON:
                if (input_state.admin_controls)
                {
                    Options::animation_level %= 3;
                    Options::animation_level += 1;
                    printf("Animation level: %d\n", Options::animation_level);
                }
                break;

            case SDLK_m:
                if (input_state.admin_controls)
                    toggle_map();
                break;

            case SDLK_n:
                if (input_state.confirm_quit)
                    toggle_confirm_quit();
                break;

            case SDLK_o:
                #if !PRODUCTION
                if (input_state.admin_controls)
                    toggle_skeleton_editor();
                #endif
                break;


            case SDLK_t:
                if (input_state.admin_controls)
                    toggle_input_mode();
                else
                {
                    toggle_chat();
                    Chat::use_global_channel();
                }
                break;

            case SDLK_u:
                #if PRODUCTION
                if (!input_state.mouse_bound || input_state.admin_controls)
                #endif
                    toggle_mouse_bind();
                break;

            case SDLK_y:
                if (input_state.confirm_quit)
                    enable_quit();
                break;

            case SDLK_TAB:
                toggle_scoreboard();
                break;

            case SDLK_RETURN:
                toggle_chat();
                Chat::use_global_channel();
                break;

            case SDLK_LEFTBRACKET:
                if (input_state.admin_controls)
                    ClientState::player_agent.toggle_camera_mode();
                break;

            case SDLK_e:
                toggle_agent_inventory();
                break;

            case SDLK_ESCAPE:
                if (Hud::has_error())
                    enable_quit();
                else
                    toggle_confirm_quit();
                break;

            default: break;
        }
    }

    // these should occur for all of Chat, Agents::Agent and Camera
    switch (event->key.keysym.sym)
    {
        //#if !PRODUCTION
        //case SDLK_n:
            //Awesomium::send_json_settings(Options::settings);
            //break;
        //#endif

        case SDLK_HOME:
            save_screenshot();
            break;

        case SDLK_F1:
            toggle_settings_window();
            break;

        case SDLK_F2:
            input_state.diagnostics = (!input_state.diagnostics);
            break;

        case SDLK_F3:
            input_state.vbo_debug = (!input_state.vbo_debug);
            break;

        case SDLK_F4:
            toggle_hud();
            break;

        case SDLK_F9:
            #if PRODUCTION
            if (input_state.admin_controls)
            #endif
                TextureSheetLoader::reload_texture_sheets();
            break;

        //case SDLK_F10:
            //if (input_state.admin_controls) t_map::toggle_3d_texture_settings();
            //break;

        case SDLK_F12:
            toggle_admin_controls();
            break;

        case SDLK_INSERT:
            input_state.terminal_is_open = !input_state.terminal_is_open;
            break;

        case SDLK_BACKQUOTE:
            toggle_full_chat();
            break;

        case SDLK_BACKSPACE:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kBACKSPACE].pressed = true;
            break;
        case SDLK_LEFT:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kLEFT_ARROW].pressed = true;
            break;
        case SDLK_RIGHT:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kRIGHT_ARROW].pressed = true;
            break;
        case SDLK_UP:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kUP_ARROW].pressed = true;
            break;
        case SDLK_DOWN:
            if (event->user.code != SDL_EVENT_USER_TRIGGER)
                keys_held_down[kDOWN_ARROW].pressed = true;
            break;

        default: break;
    }
}

// key up
void key_up_handler(SDL_Event* event)
{
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_up_handler(event);
        return;
    }

    if (input_state.equipped_sprite_adjuster)
    {
        EquippedSpriteAdjuster::key_up_handler(event);
        return;
    }

    if (input_state.awesomium)
        Awesomium::SDL_keyboard_event(event);
    else if (input_state.chat)
        chat_key_up_handler(event);
    else if (input_state.agent_inventory || input_state.container_block)
        container_key_up_handler(event);
    else
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_up_handler(event);
        else
            camera_key_up_handler(event);

        switch (event->key.keysym.sym)
        {
            default: break;
        }
    }

    switch (event->key.keysym.sym)
    {
        case SDLK_BACKSPACE:
            keys_held_down[kBACKSPACE].pressed = false;
            break;
        case SDLK_LEFT:
            keys_held_down[kLEFT_ARROW].pressed = false;
            break;
        case SDLK_RIGHT:
            keys_held_down[kRIGHT_ARROW].pressed = false;
            break;
        case SDLK_UP:
            keys_held_down[kUP_ARROW].pressed = false;
            break;
        case SDLK_DOWN:
            keys_held_down[kDOWN_ARROW].pressed = false;
            break;

        default: break;
    }

}

// mouse down
void mouse_button_down_handler(SDL_Event* event)
{
    if (input_state.last_mouse_button_down_event_frame == ClientState::frame_id) return;
    input_state.last_mouse_button_down_event_frame = ClientState::frame_id;
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::mouse_button_down_handler(event);
        return;
    }

    // chat doesnt affect mouse
    if (input_state.awesomium)
        Awesomium::SDL_mouse_event(event);
    else if (input_state.agent_inventory || input_state.container_block)
        container_mouse_down_handler(event);
    else if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_down_handler(event);
    else
        camera_mouse_down_handler(event);

    switch (event->button.button)
    {
        default: break;
    }

}

void mouse_button_up_handler(SDL_Event* event)
{
    if (input_state.last_mouse_button_up_event_frame == ClientState::frame_id) return;
    input_state.last_mouse_button_up_event_frame = ClientState::frame_id;
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::mouse_button_up_handler(event);
        return;
    }

    // chat doesnt affect mouse

    if (input_state.login_mode)
        auth_form_mouse_up_handler(event);
    if (input_state.awesomium)
        Awesomium::SDL_mouse_event(event);
    else if (input_state.agent_inventory || input_state.container_block)
        container_mouse_up_handler(event);
    else if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_up_handler(event);
    else
        camera_mouse_up_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
}

void mouse_motion_handler(SDL_Event* event)
{
    // chat doesnt affect mouse
    if (input_state.ignore_mouse_motion)
    {
        input_state.ignore_mouse_motion = false;
        return;
    }

    if (input_state.login_mode)
        auth_form_mouse_motion_handler(event);
    else if (input_state.awesomium)
        Awesomium::SDL_mouse_event(event);
    else if (input_state.agent_inventory || input_state.container_block)
        container_mouse_motion_handler(event);
    else if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_motion_handler(event);
    else
        camera_mouse_motion_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
}

void quit_event_handler(SDL_Event* event)
{
    input_state.quit = true;
}

void key_state_handler(Uint8 *keystate, int numkeys)
{
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_state_handler(keystate, numkeys);
        return;
    }

    char f = 0;
    char b = 0;
    char l = 0;
    char r = 0;
    char jet = 0;
    char jump = 0;
    char crouch = 0;
    char boost = 0;
    char charge = 0;
    char misc2 = 0;
    char misc3 = 0;

    if (!input_state.chat && !input_state.awesomium && !input_state.login_mode)
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_state_handler(keystate, numkeys, &f, &b, &l, &r, &jet,
                                    &jump, &crouch, &boost, &charge, &misc2,
                                    &misc3);
        else
            camera_key_state_handler(keystate, numkeys);
    }

    ClientState::player_agent.set_control_state(f, b, l, r, jet, jump, crouch,
                                                boost, charge, misc2, misc3,
                                                agent_camera->theta,
                                                agent_camera->phi);
}

// active event (window / input focus)
void active_event_handler(SDL_Event* event)
{
    //SDL_APPMOUSEFOCUS -- mouse moves past visible portion of window
    //SDL_APPINPUTFOCUS -- window is selected. use this for detecting alt-tab
    //SDL_APPACTIVE     -- worthless, cannot trigger it

    bool gained = event->active.gain;

    // set input_state struct
    if (event->active.state & SDL_APPMOUSEFOCUS)
        input_state.mouse_focus = gained;
    else
    if (event->active.state & SDL_APPINPUTFOCUS)
        input_state.input_focus = gained;
    else
    if (event->active.state & SDL_APPACTIVE)
        input_state.app_active = gained;

    // force mouse position out of window
    if (!input_state.mouse_focus)
        HudContainer::set_mouse_position(-1, -1);

    if (!mouse_unlocked_for_ui_element())
    {   // only do this if container/inventory not open
        // handle alt tab
        if (event->active.state & SDL_APPINPUTFOCUS)
        {
            if (event->active.gain)
            {
                set_mouse_bind(input_state.rebind_mouse);
                set_mouse_rebind(false);
            }
            else
            {
                set_mouse_rebind(input_state.mouse_bound);
                set_mouse_bind(false);
            }
        }
    }
}

#ifdef __clang__
# pragma clang diagnostic pop
#endif
