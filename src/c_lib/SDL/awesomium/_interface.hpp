/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <common/settings_export.hpp>

#if GS_AWESOMIUM

#include <Awesomium/awesomium_capi.h>

namespace Awesomium
{

#define _BASE_USER_AGENT "Mozilla/5.0 AppleWebKit/533.3 (KHTML, like Gecko) Chrome/12.0.702.0 Safari/533.3 Awesomium/1.6.5"
const char BASE_USER_AGENT[] = _BASE_USER_AGENT;
const char USER_AGENT[] = _BASE_USER_AGENT " Gnomescroll/" GS_STR(GS_VERSION);

extern class ViewportManager* viewport_manager;

void init();
void teardown();
void update();
void draw();

void enable();
void disable();

void SDL_keyboard_event(const SDL_Event* event);
void SDL_mouse_event(const SDL_Event* event);

void open_token_page();
void send_json_settings(class SettingsExport* exporter);

awe_string* get_awe_string(const char* str);
char* get_str_from_awe(const awe_string* str);

char* get_cookies();    // use GNOMESCROLL_URL
char* get_cookies(const char* url);
char* get_auth_token();

void delete_cookie(const char* name);   // uses GNOMESCROLL_URL
void delete_cookie(const char* url, const char* name);
void delete_auth_token_cookie();
void delete_all_cookies();

void check_for_token_cookie(const awe_string* _url);

void set_game_token_cookie(const char* _token, time_t expiration_time);

void save_username(const char* username);
void save_password(const char* password);
void save_remember_password_setting(bool remember);
void get_credentials(char** _username, char** _password);   // MUST FREE THE RESULTS
bool get_remember_password_setting();

}   // Awesomium

#else
// stubs
namespace Awesomium
{

bool login_page_loaded() { return false; }

void init() {}
void teardown() {}
void update() {}
void draw() {}

// F1 toggles awesomium window
void enable() {}
void disable() {}

void SDL_keyboard_event(const SDL_Event* event) {}
void SDL_mouse_event(const SDL_Event* event) {}

void open_token_page() {}
void send_json_settings(class SettingsExport* exporter) {}

char* get_cookies() { return NULL; }
char* get_auth_token() { return NULL; }

void delete_cookie(const char* name) {}
void delete_auth_token_cookie() {}
void delete_all_cookies() {}

void set_game_token_cookie(const char* _token, time_t expiration_time) {}

}
#endif
