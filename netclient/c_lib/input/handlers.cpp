#include "handlers.hpp"

#include <chat/client.hpp>
#include <hud/hud.hpp>
#include <input/skeleton_editor.hpp>

//toggling graphics settings
#include <c_lib/t_map/glsl/shader.hpp>

InputState input_state;

// triggers
void toggle_mouse_bind()
{
    input_state.mouse_bound = (!input_state.mouse_bound);
}

void toggle_help_menu()
{
    input_state.help_menu = (!input_state.help_menu);
}

void toggle_inventory()
{
    input_state.inventory = (!input_state.inventory);
}

void toggle_scoreboard()
{
    input_state.scoreboard = (!input_state.scoreboard);
}

void toggle_map()
{
    input_state.map = (!input_state.map);
}

void toggle_chat()
{
    input_state.chat = (!input_state.chat);
}

void toggle_full_chat()
{
    input_state.full_chat = (!input_state.full_chat);
}

void toggle_hud()
{
    #if PRODUCTION
    input_state.hud = true;
    #else
    input_state.hud = (!input_state.hud);
    #endif
}

void toggle_debug()
{
    input_state.debug = (!input_state.debug);
}

void toggle_graphs()
{
    input_state.graphs = (!input_state.graphs);
}

void enable_jump()
{
    input_state.can_jump = true;
}

void toggle_confirm_quit()
{
    input_state.confirm_quit = (!input_state.confirm_quit);
}

void enable_quit()
{
    // release mouse
    unbind_mouse();
    input_state.quit = true;
}

void toggle_skeleton_editor()
{
#if PRODUCTION
    input_state.skeleton_editor = false;
#else
    input_state.skeleton_editor = (!input_state.skeleton_editor);
    if (input_state.skeleton_editor)
        printf("Skeleton editor enabled\n");
#endif
}

void toggle_input_mode()
{
    switch (input_state.input_mode)
    {
        case INPUT_STATE_AGENT:
            input_state.input_mode = INPUT_STATE_CAMERA;
            break;
        case INPUT_STATE_CAMERA:
            if (ClientState::playerAgent_state.you != NULL)
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
            input_state.camera_mode = INPUT_STATE_CAMERA;
            break;
        case INPUT_STATE_CAMERA:
            if (ClientState::playerAgent_state.you != NULL)
                input_state.camera_mode = INPUT_STATE_AGENT;
            break;
        default: break;
    }
}

void init_handlers()
{
    // set input_state defaults
#if PRODUCTION
    input_state.mouse_bound = true;
    input_state.debug = false;
    input_state.input_mode = INPUT_STATE_AGENT;
    input_state.camera_mode = INPUT_STATE_AGENT;
    input_state.graphs = false;
#else
    input_state.mouse_bound = false;
    input_state.debug = false;
    input_state.input_mode = INPUT_STATE_CAMERA;
    input_state.camera_mode = INPUT_STATE_CAMERA;
    input_state.graphs = true;
#endif

    input_state.help_menu = false;
    input_state.inventory = false;
    input_state.scoreboard = false;
    input_state.map = false;
    input_state.chat = false;
    input_state.full_chat = false;
    input_state.hud = true;
    input_state.skeleton_editor = false;

    input_state.has_focus = true;
    input_state.rebind_mouse = input_state.mouse_bound;

    input_state.can_jump = true;
    input_state.quit = false;

    // options
    input_state.invert_mouse = false;
    input_state.sensitivity = 100.0f;
}

// options
const float ZOOM_SENSITIVITY_SCALE = 0.7f;
void set_input_options(
    bool invert_mouse,
    float sensitivity
)
{
    input_state.invert_mouse = invert_mouse;
    input_state.sensitivity = sensitivity;
    if (Hud::hud_draw_settings.zoom)
        input_state.sensitivity *= ZOOM_SENSITIVITY_SCALE;
}

/* Chat buffer */

void chat_key_down_handler(SDL_Event* event)
{
    using namespace Hud;
    
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

/* Agent */

#include <c_lib/common/lua/lua.hpp>

void agent_key_down_handler(SDL_Event* event)
{

    switch (event->key.keysym.sym)
    {
        case SDLK_k:
            run_lua_test();
            break;

        case SDLK_r:
            ClientState::playerAgent_state.action.reload();
            break;
        
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
            if (ClientState::playerAgent_state.action.switch_weapon(1-1)) agent_camera->unzoom();
            break;
        case SDLK_2:
            if (ClientState::playerAgent_state.action.switch_weapon(2-1)) agent_camera->unzoom();
            break;
        case SDLK_3:
            if (ClientState::playerAgent_state.action.switch_weapon(3-1)) agent_camera->unzoom();
            break;
        case SDLK_4:
            if (ClientState::playerAgent_state.action.switch_weapon(4-1)) agent_camera->unzoom();
            break;
        case SDLK_5:
            if (ClientState::playerAgent_state.action.switch_weapon(5-1)) agent_camera->unzoom();
            break;
        case SDLK_6:
            if (ClientState::playerAgent_state.action.switch_weapon(6-1)) agent_camera->unzoom();
            break;
        case SDLK_7:
            if (ClientState::playerAgent_state.action.switch_weapon(7-1)) agent_camera->unzoom();
            break;
        case SDLK_8:
            if (ClientState::playerAgent_state.action.switch_weapon(8-1)) agent_camera->unzoom();
            break;
        case SDLK_9:
            if (ClientState::playerAgent_state.action.switch_weapon(9-1)) agent_camera->unzoom();
            break;
        case SDLK_0:
            if (ClientState::playerAgent_state.action.switch_weapon(10-1)) agent_camera->unzoom();
            break;

        default: break;
    }

}

void agent_key_up_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_SPACE:
            enable_jump();
            break;
            
        default: break;
    }
}

void agent_mouse_down_handler(SDL_Event* event)
{
    PlayerAgent_state* p = &ClientState::playerAgent_state;
    if (p->you == NULL) return;
    
    switch (event->button.button)
    {
        case SDL_BUTTON_LEFT:
            p->action.fire();
            break;

        case SDL_BUTTON_RIGHT:

            if (p->you->weapons.can_zoom())
                agent_camera->toggle_zoom();
            if (p->you->weapons.active == Weapons::TYPE_block_applier)
            {
                int block = p->action.select_block();
                if (block)
                    HudCubeSelector::cube_selector.set_active_id(block);
            }
            break;

        case SDL_BUTTON_MIDDLE:
            // nothing
            break;

        case 4: // scroll up
            if (p->action.switch_weapon(-1)) agent_camera->unzoom();
            break;

        case 5: // scroll down
            if (p->action.switch_weapon(-2)) agent_camera->unzoom();
            break;

        default: break;
    }
}

void agent_mouse_up_handler(SDL_Event* event){}
void agent_mouse_motion_handler(SDL_Event* event){}

void agent_key_state_handler(Uint8 *keystate, int numkeys,
    char *f, char *b, char *l, char *r,
    char *jet, char *jump, char *crouch, char *boost,
    char *m1, char *m2, char *m3
)
{
    if (keystate['w'])
        *f = 1;
    if (keystate['s'])
        *b = 1;
    if (keystate['a'])
        *l = 1;
    if (keystate['d'])
        *r = 1;
    if (keystate['z'])
        *jet = 1;
    if (keystate[' '])
        *jump = 1;
    if (keystate[306])  // LCTRL
        *crouch = 1;
}


/* Camera */

void camera_key_down_handler(SDL_Event* event)
{
    switch (event->key.keysym.sym)
    {
        case SDLK_p:    // jump to agent camera
            free_camera->copy_state_from(agent_camera);
            break;
        default: break;
    }
}

void camera_key_up_handler(SDL_Event* event){}
void camera_mouse_down_handler(SDL_Event* event)
{
    switch (event->button.button)
    {
        case SDL_BUTTON_RIGHT:
            free_camera->toggle_zoom();
            break;

        default: break;
    }
}

void camera_mouse_up_handler(SDL_Event* event){}
void camera_mouse_motion_handler(SDL_Event* event){}

void camera_key_state_handler(Uint8 *keystate, int numkeys)
{
    const float speed = 0.8f;
    
    if (keystate['w'])
        free_camera->move(speed, 0,0);
    if (keystate['s'])
        free_camera->move(-speed,0,0);
    if (keystate['a'])
        free_camera->move(0, speed, 0);
    if (keystate['d'])
        free_camera->move(0,-speed,0);
    if (keystate['r'])
        free_camera->move(0,0,speed);
    if (keystate['f'])
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

    if (input_state.chat)
        chat_key_down_handler(event);
    else
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_down_handler(event);
        else
            camera_key_down_handler(event);

    
        // these should occur for both Agent and Camera
        switch (event->key.keysym.sym)
        {
            case SDLK_b:
                if (input_state.debug)
                    Animations::agent_bleed(
                        ClientState::playerAgent_state.camera_state.x,
                        ClientState::playerAgent_state.camera_state.y,
                        ClientState::playerAgent_state.camera_state.z
                    );
                break;
                
            case SDLK_g:
                if (input_state.debug)
                    toggle_camera_mode();
                break;
                
            case SDLK_h:
                toggle_help_menu();
                break;

            case SDLK_m:
                toggle_map();
                break;

            case SDLK_n:
                if (input_state.confirm_quit)
                    toggle_confirm_quit();
                break;

            case SDLK_o:
                toggle_skeleton_editor();
                break;
            
            case SDLK_t:
                if (input_state.debug)
                    toggle_input_mode();
                else
                {
                    toggle_chat();
                    chat_client->use_global_channel();
                }
                break;
                
            case SDLK_u:
                toggle_mouse_bind();
                break;

            case SDLK_y:
                if (input_state.confirm_quit)
                {
                    enable_quit();
                    break;
                }
                toggle_chat();
                if (!input_state.debug)
                    chat_client->use_team_channel();
                break;

            case SDLK_TAB:
                toggle_scoreboard();
                break;

            case SDLK_RETURN:
                toggle_chat();
                chat_client->use_global_channel();
                break;

            case SDLK_SLASH:
                toggle_hud();
                break;

            case SDLK_LEFTBRACKET:
                if (input_state.debug)
                    ClientState::playerAgent_state.toggle_camera_mode();
                break;

            case SDLK_RIGHTBRACKET:
                if (input_state.debug)
                    Monsters::test(30);
                break;

            case SDLK_QUOTE:
                toggle_graphs();
                break;

            case SDLK_ESCAPE:
                if (NetClient::Server.connected)
                    toggle_confirm_quit();
                else
                    enable_quit();
                break;

            default: break;
        }
    }

    // these should occur for all of Chat, Agent and Camera
    switch (event->key.keysym.sym)
    {
        case SDLK_HOME:
            save_screenshot();
            break;

        case SDLK_F10:
            t_map::toggle_3d_texture_settings();
            break;

        case SDLK_F12:
            toggle_debug();
            break;

        case SDLK_LALT:
        case SDLK_RALT:
            input_state.rebind_mouse = input_state.mouse_bound;
            input_state.mouse_bound = false;
            break;

        case SDLK_BACKQUOTE:
            toggle_full_chat();
            break;
            
        default: break;
    }


    //for Dany0 (azerty testing)
    //while holding n, will show key struct info
    //if (keystate['r'] != 0)
    //{
        //printf("scancode = %d\n", (int)event->key.keysym.scancode);
        //printf("keysym = %d\n", (int)event->key.keysym.sym);
        //printf("\n");
    //}
}

// key up
void key_up_handler(SDL_Event* event)
{

    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_up_handler(event);
        return;
    }

    if (input_state.chat)
        chat_key_up_handler(event);
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
        case SDLK_LALT:
        case SDLK_RALT:
            if (input_state.has_focus)
                input_state.mouse_bound = input_state.rebind_mouse;
            break;

        default: break;
    }

}

// mouse down
void mouse_button_down_handler(SDL_Event* event)
{

    if (input_state.skeleton_editor)
    {
        SkeletonEditor::mouse_button_down_handler(event);
        return;
    }

    // chat doesnt affect mouse

    if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_down_handler(event);
    else
        camera_mouse_down_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
    
}

// mouse up
void mouse_button_up_handler(SDL_Event* event)
{

    if (input_state.skeleton_editor)
    {
        SkeletonEditor::mouse_button_up_handler(event);
        return;
    }

    // chat doesnt affect mouse

    if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_up_handler(event);
    else
        camera_mouse_up_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
}

// mouse motion
void mouse_motion_handler(SDL_Event* event)
{
    // chat doesnt affect mouse

    if (input_state.input_mode == INPUT_STATE_AGENT)
        agent_mouse_motion_handler(event);
    else
        camera_mouse_motion_handler(event);

    switch (event->button.button)
    {
        default: break;
    }
}

// quit
void quit_event_handler(SDL_Event* event)
{
    input_state.quit = true;
}

// keyboard state
void key_state_handler(Uint8 *keystate, int numkeys)
{
    if (input_state.skeleton_editor)
    {
        SkeletonEditor::key_state_handler(keystate, numkeys);
        return;
    }
    
    char f,b,l,r,jet,jump,crouch,boost,m1,m2,m3;
    f=b=l=r=jet=jump=crouch=boost=m1=m2=m3=0;
    
    if (!input_state.chat)
    {
        if (input_state.input_mode == INPUT_STATE_AGENT)
            agent_key_state_handler(keystate, numkeys, &f, &b, &l, &r, &jet, &jump, &crouch, &boost, &m1, &m2, &m3);
        else
            camera_key_state_handler(keystate, numkeys);
    }

    // always set control state
    ClientState::playerAgent_state.set_control_state(f,b,l,r,jet,jump,crouch,boost,m1,m2,m3, agent_camera->theta, agent_camera->phi);
}

// active event (window / input focus)
void active_event_handler(SDL_Event* event)
{
    Uint8 app_state = SDL_GetAppState();
    
    //if (app_state & SDL_APPMOUSEFOCUS)
        //printf("Mouse\n");
    //if (app_state & SDL_APPINPUTFOCUS)
        //printf("Keyboard\n");
    if (app_state & SDL_APPACTIVE)
    {
        if (event->active.gain)
        {
            if (!input_state.has_focus)
                input_state.mouse_bound = input_state.rebind_mouse;
            input_state.has_focus = true;
        }
        else
            input_state.has_focus = false;
    }
}
