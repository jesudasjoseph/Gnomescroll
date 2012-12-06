#pragma once

#include <hud/font.hpp>
#include <hud/text.hpp>
#include <hud/jetpack_meter.hpp>

namespace Hud
{

const char dead_text[] = "You died.";
const char fps_format[] = "%3.2ffps";
const char ping_format[] = "%dms";
const char location_format[] = "x: %f\ny: %f\nz: %f";
const char no_agent_text[] = "No Agent Assigned";
const char health_format[] = "ENERGY %02d";
const char health_color_string[] = "ENERGY";
const char confirm_quit_text[] = "Really quit? Y/N";
const char press_help_text[] = "Press H for help";
const char error_subtitle_text[] = "Press ESC to exit";
const char open_login_text[] = "Press F1 to log in.";

const struct Color HEALTH_GREEN = color_init(10,240,10);
const struct Color HEALTH_GREY = color_init(100,100,100);
const struct Color HEALTH_WHITE = color_init(255,255,255);
const struct Color HEALTH_RED = color_init(240,10,10);

using namespace HudText;
using HudFont::font;
using HudFont::start_font_draw;
using HudFont::end_font_draw;
using HudFont::set_texture;

const int CHAT_MESSAGE_RENDER_MAX = 8;
char CHAT_NAME_DEFAULT_SEPARATOR[] = ": ";
const int CHAT_NAME_SEPARATOR_LENGTH_MAX = (int)strlen(CHAT_NAME_DEFAULT_SEPARATOR);

class ChatRender
{
    public:
        bool inited;
        Text* messages[CHAT_MESSAGE_RENDER_MAX];
        Text* input;
        int paging_offset;
        void init();
        void draw_messages();
        void draw_input();
        void draw_cursor();

        float cursor_x, cursor_y, cursor_w, cursor_h;
        void set_cursor(const char* text, float x, float y);
        void update(bool timeout=true);
        //void page_up();
        //void page_down();

    ChatRender();
    ~ChatRender();
};

const int N_STATS = 2;
class Scoreboard
{

    public:
        bool inited;

        Text* tags[N_STATS];
        Text* ids[PLAYERS_MAX];
        Text* names[PLAYERS_MAX];

        void update();
        void init();
        void draw();

    Scoreboard();
    ~Scoreboard();
    
};

class HUD
{
    public:
    bool inited;
    
    // text objects
    Text* help;
    Text* dead;
    Text* fps;
    Text* ping;
    Text* reliable_ping;
    Text* location;
    Text* look;
    AnimatedText* health;
    Text* confirm_quit;
    Text* prompt;
    Text* error;
    Text* error_subtitle;
    Text* awesomium_message;

    // scoreboard needs rewritten logic
    // will be its own class, also holding text objects
    Scoreboard* scoreboard;
    
    // chat queue
    ChatRender* chat;    // contains text objects

    void init();
    HUD();
    ~HUD();
};

// meter
MeterGraphic meter_graphic;
void set_color_from_ratio(float ratio, float alpha = 175, bool invert_color_for_damage = false);

void set_prompt(const char* msg);
void clear_prompt(const char* msg);

void set_awesomium_message(const char* msg);
void clear_awesomium_message(const char* msg);
void clear_awesomium_message();
void draw_awesomium_message();

}   // Hud
