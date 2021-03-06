/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#if GS_AWESOMIUM

#include <SDL/awesomium/_interface.hpp>
#include <auth/constants.hpp>
#include <auth/client.hpp>

namespace Awesomium
{

// Translates an SDLKey virtual key code to an Awesomium key code
int get_web_key_from_SDL_key(SDLKey key);
void inject_SDL_key_event(awe_webview* webView, const SDL_Event* event);
void inject_SDL_mouse_event(awe_webview* webView, const SDL_Event* event);

const char JS_OBJ_NAME[] = "Gnomescroll";
const char JS_OBJ_CREATE_URL_NAME[] = "create_url";
const char JS_OBJ_LOGIN_URL_NAME[] = "login_url";
const char JS_OBJ_TOKEN_URL_NAME[] = "token_url";
const char JS_OBJ_LOGIN_ERROR_NAME[] = "login_error";
const char JS_OBJ_GAME_TOKEN_NAME[] = "gstoken";
const char JS_OBJ_DEBUG_NAME[] = "DEBUG";
const char JS_OBJ_TOKEN_NAME_NAME[] = "token_name";
const char JS_OBJ_USERNAME_NAME[] = "gs_username";
const char JS_OBJ_PASSWORD_NAME[] = "gs_pass";
const char JS_OBJ_REMEMBER_PASSWORD_NAME[] = "remember_password";
const char JS_OBJ_VERSION_NAME[] = "version";

// js -> C callbacks (registered on the Gnomescroll object)
const char JS_CB_SET_MESSAGE_NAME[] = "set_message";
const char JS_CB_UNSET_MESSAGE_NAME[] = "clear_message";
const char JS_CB_SET_TOKEN_NAME[] = "set_token";
const char JS_CB_TOKEN_FAILURE_NAME[] = "token_failure";
const char JS_CB_LOGIN_REQUIRED_NAME[] = "login_required";
const char JS_CB_SAVE_USERNAME_NAME[] = "save_username";
const char JS_CB_SAVE_PASSWORD_NAME[] = "save_password";
const char JS_CB_SAVE_REMEMBER_PASSWORD_SETTING_NAME[] = "save_remember_password_setting";
const char JS_CB_CHANGE_SETTING_VALUE[] = "change_setting";

// C -> js callbacks (not registered, but defined in the js)
const char JS_CB_OPEN_TOKEN_PAGE_NAME[] = "gs_get_token";
const char JS_CB_SEND_JSON_SETTINGS[] = "gs_load_json_settings";

void begin_navigation_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name);
void begin_loading_cb(awe_webview* webView, const awe_string* _url, const awe_string* _frame_name, int status_code, const awe_string* _mime_type);
void finish_loading_cb(awe_webview* webView);
awe_resource_response* resource_request_cb(awe_webview* webView, awe_resource_request* request);
void resource_response_cb(awe_webview* webView, const awe_string* _url, int status_code, bool was_cached, int64 request_time_ms, int64 response_time_ms, int64 expected_content_size, const awe_string *_mime_type);
void web_view_crashed_cb(awe_webview* webView);
void js_console_message_callback(awe_webview *webView, const awe_string *message, int line_number, const awe_string *source);

// main handler that routes to other callbacks
void js_callback_handler(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);

// specific callbacks
void js_set_message_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);
void js_unset_message_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);
void js_set_token_callback(awe_webview* webView, const awe_string* _obj_name, const awe_string* _cb_name, const awe_jsarray* _args);


class ChromeViewport
{
    public:
        int id;
        int xoff;
        int yoff;
        int width;
        int height;
        bool in_focus;
        awe_webview* webView;
        awe_string* js_obj_name;
        GLuint tex;
        bool crashed;
        int priority;

    ChromeViewport() :
        id(-1), in_focus(false), js_obj_name(NULL), tex(0), crashed(false),
        priority(0)
    {
        this->xoff = int(_xresf * 0.125f);
        this->yoff = int(_yresf * 0.0625);    // from bottom
        this->width = int(_xresf * 0.75f);
        this->height = int(_yresf * 0.875f);

        this->init_webview();
        this->init_render_surface();

        this->set_callbacks();
        this->setup_whitelist();
        this->setup_javascript();
    }

    ~ChromeViewport()
    {
        if (this->js_obj_name != NULL) awe_string_destroy(this->js_obj_name);
        if (this->webView != NULL) awe_webview_destroy(this->webView);
    }

    bool position_in_window(int x, int y, int* sx, int* sy)
    {   // subtract top left of webview window
        int left = this->xoff;
        int top = _yres - (this->yoff + this->height);
        *sx = x - left;
        *sy = y - top;
        return ((*sx) >= 0 && (*sx) < this->width && (*sy) >= 0 && (*sy) < this->height);
    }


    void set_callbacks();

    void add_site_to_whitelist(const char* _url)
    {
        IF_ASSERT(this->webView == NULL) return;
        awe_string* url = get_awe_string(_url);
        awe_webview_add_url_filter(this->webView, url);
        awe_string_destroy(url);
    }

    void setup_whitelist()
    {
        IF_ASSERT(this->webView == NULL) return;
        awe_webview_set_url_filtering_mode(this->webView, AWE_UFM_WHITELIST);
        this->add_site_to_whitelist("local://*");
        this->add_site_to_whitelist("file://*");
        this->add_site_to_whitelist ("http://gnomescroll.com/*");
        this->add_site_to_whitelist("https://gnomescroll.com/*");
        this->add_site_to_whitelist ("http://*.gnomescroll.com/*");
        this->add_site_to_whitelist("https://*.gnomescroll.com/*");
        //this->add_site_to_whitelist ("http://www.google-analytics.com/*");
        //this->add_site_to_whitelist("https://www.google-analytics.com/*");
        //this->add_site_to_whitelist ("http://ajax.googleapis.com/ajax/libs/jquery/*");
        //this->add_site_to_whitelist("https://ajax.googleapis.com/ajax/libs/jquery/*");
        #if !PRODUCTION
        this->add_site_to_whitelist ("http://127.0.0.1:5002/*");
        this->add_site_to_whitelist("https://127.0.0.1:5002/*");
        #endif
    }

    void setup_javascript()
    {
        IF_ASSERT(this->webView == NULL) return;

        this->js_obj_name = get_awe_string(JS_OBJ_NAME);
        awe_webview_create_object(this->webView, this->js_obj_name);

        #if PRODUCTION
        this->set_js_value(JS_OBJ_DEBUG_NAME, true);
        #else
        this->set_js_value(JS_OBJ_DEBUG_NAME, false);
        #endif

        this->set_js_value(JS_OBJ_LOGIN_URL_NAME, GNOMESCROLL_LOGIN_URL);
        this->set_js_value(JS_OBJ_CREATE_URL_NAME, GNOMESCROLL_CREATE_URL);
        this->set_js_value(JS_OBJ_TOKEN_URL_NAME, GNOMESCROLL_TOKEN_URL);
        this->set_js_value(JS_OBJ_TOKEN_NAME_NAME, Auth::AUTH_TOKEN_COOKIE_NAME);
        this->set_js_value(JS_OBJ_VERSION_NAME, GS_VERSION);

        // credentials
        char* username = NULL;
        char* password = NULL;
        get_credentials(&username, &password);
        if (username != NULL)
        {
            this->set_js_value(JS_OBJ_USERNAME_NAME, username);
            if (password != NULL)
                this->set_js_value(JS_OBJ_PASSWORD_NAME, password);
            else
                this->set_js_value(JS_OBJ_PASSWORD_NAME, "");
            free(password);
        }
        else
            this->set_js_value(JS_OBJ_USERNAME_NAME, "");
        free(username);

        this->set_js_value(JS_OBJ_REMEMBER_PASSWORD_NAME, get_remember_password_setting());

        // set some null values on the object
        this->set_js_value(JS_OBJ_LOGIN_ERROR_NAME);
        this->set_js_value(JS_OBJ_GAME_TOKEN_NAME);

        // callback for console.log
        awe_webview_set_callback_js_console_message(this->webView, &js_console_message_callback);

        this->register_js_callback(JS_CB_SET_MESSAGE_NAME);
        this->register_js_callback(JS_CB_UNSET_MESSAGE_NAME);
        this->register_js_callback(JS_CB_SET_TOKEN_NAME);
        this->register_js_callback(JS_CB_TOKEN_FAILURE_NAME);
        this->register_js_callback(JS_CB_LOGIN_REQUIRED_NAME);
        this->register_js_callback(JS_CB_SAVE_USERNAME_NAME);
        this->register_js_callback(JS_CB_SAVE_PASSWORD_NAME);
        this->register_js_callback(JS_CB_SAVE_REMEMBER_PASSWORD_SETTING_NAME);
        this->register_js_callback(JS_CB_CHANGE_SETTING_VALUE);

        // callbacks for error handling
        awe_webview_set_callback_js_callback(this->webView, &js_callback_handler);
    }

    void register_js_callback(const char* _name)
    {   // registers a javascript callback name on the global js obj
        IF_ASSERT(this->webView == NULL || this->js_obj_name == NULL) return;
        awe_string* name = get_awe_string(_name);
        awe_webview_set_object_callback(this->webView, this->js_obj_name, name);
        awe_string_destroy(name);
    }

    void set_js_value(const char* _name, bool _value)
    {   // sets value with name to the global js obj
        IF_ASSERT(this->webView == NULL || this->js_obj_name == NULL) return;
        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_bool_value(_value);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }

    void set_js_value(const char* _name, double _value)
    {   // sets value with name to the global js obj
        IF_ASSERT(this->webView == NULL || this->js_obj_name == NULL) return;
        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_double_value(_value);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }

    void set_js_value(const char* _name, int _value)
    {   // sets value with name to the global js obj
        IF_ASSERT(this->webView == NULL || this->js_obj_name == NULL) return;
        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_integer_value(_value);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }

    void set_js_value(const char* _name)
    {   // sets value with name to the global js obj
        IF_ASSERT(this->webView == NULL || this->js_obj_name == NULL) return;
        awe_string* name = get_awe_string(_name);
        awe_jsvalue* value = awe_jsvalue_create_null_value();
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_jsvalue_destroy(value);
    }

    void set_js_value(const char* _name, const char* _value)
    {   // sets value with name to the global js obj
        IF_ASSERT(this->webView == NULL || this->js_obj_name == NULL) return;
        awe_string* name = get_awe_string(_name);
        awe_string* val = get_awe_string(_value);
        awe_jsvalue* value = awe_jsvalue_create_string_value(val);
        awe_webview_set_object_property(this->webView, this->js_obj_name, name, value);
        awe_string_destroy(name);
        awe_string_destroy(val);
        awe_jsvalue_destroy(value);
    }

    void load_url(const char* _url)
    {
        IF_ASSERT(this->webView == NULL) return;
        awe_string* url = get_awe_string(_url);
        awe_webview_load_url(this->webView,
                             url,  // url
                             awe_string_empty(),    // frame name
                             awe_string_empty(),    // username
                             awe_string_empty());   // password
        awe_string_destroy(url);
    }

    void load_file(const char* _file)
    {
        IF_ASSERT(this->webView == NULL) return;
        awe_string* file = get_awe_string(_file);
        awe_webview_load_file(this->webView,
                              file,                 // filename
                              awe_string_empty());  // frame name
        awe_string_destroy(file);
    }

    void init_webview()
    {
        this->webView = awe_webcore_create_webview(this->width, this->height, false);
        //default loading

        IF_ASSERT(this->webView == NULL) return;

        awe_webview_set_transparent(this->webView, 1); ///preserve transpanency of window
        //Sets whether or not pages should be rendered with transparency preserved.
        //(ex, for pages with style="background-color:transparent")
    }

    void init_render_surface()
    {
        GS_ASSERT(this->tex == 0);
        const awe_renderbuffer* renderBuffer = awe_webview_render(webView);

        IF_ASSERT(renderBuffer == NULL) return;

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &this->tex);

        glBindTexture(GL_TEXTURE_2D, this->tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //awe_renderbuffer_get_buffer(renderBuffer),
        //awe_renderbuffer_get_width(renderBuffer),
        //awe_renderbuffer_get_height(renderBuffer),
        //awe_renderbuffer_get_rowspan(renderBuffer)

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA,
                     GL_UNSIGNED_BYTE,
                     (void*)awe_renderbuffer_get_buffer(renderBuffer));

        glDisable(GL_TEXTURE_2D);

        CHECK_GL_ERROR();
    }

    void update()
    {
        if (!awe_webview_is_dirty(this->webView) || awe_webview_is_loading_page(this->webView)) return;
        //awe_rect rect = awe_webview_get_dirty_bounds(webView);
        IF_ASSERT(this->tex == 0) return;

        const awe_renderbuffer* renderBuffer = awe_webview_render(webView);

        IF_ASSERT(renderBuffer == NULL)
        {
            this->tex = 0;
            return;
        }

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA,
                     GL_UNSIGNED_BYTE,
                     (void*) awe_renderbuffer_get_buffer(renderBuffer));
        glDisable(GL_TEXTURE_2D);
    }

    void draw()
    {
        if (this->tex == 0) return;

        glColor4ub(255,255,255,255);

        GL_ASSERT(GL_DEPTH_TEST, false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, this->tex);

        const float z = -0.5f;
        float x0 = xoff;
        float y0 = yoff;
        float x1 = xoff + width;
        float y1 = yoff + height;

        glBegin(GL_QUADS);
        glTexCoord2i(0, 1);
        glVertex3f(x0, y0, z);
        glTexCoord2i(1, 1);
        glVertex3f(x1, y0, z);
        glTexCoord2i(1, 0);
        glVertex3f(x1, y1, z);
        glTexCoord2i(0, 0);
        glVertex3f(x0, y1, z);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }

    void focus()
    {
        IF_ASSERT(this->webView == NULL) return;
        awe_webview_focus(this->webView);
        this->in_focus = true;
    }

    void unfocus()
    {
        IF_ASSERT(this->webView == NULL) return;
        awe_webview_unfocus(this->webView);
        this->in_focus = false;
    }

    void crash()
    {
        this->crashed = true;
        this->webView = NULL;
    }

    void set_html(char* html)
    {
        awe_string* html_str = awe_string_create_from_ascii(html, strlen(html));
        awe_webview_load_html(webView, html_str,awe_string_empty());
        awe_string_destroy(html_str);
    }

    void set_url(char* url)
    {
        awe_string* url_str = awe_string_create_from_ascii(url, strlen(url));
        awe_webview_load_url(webView, url_str, awe_string_empty(), awe_string_empty(), awe_string_empty());
        awe_string_destroy(url_str);
    }

    void process_key_event(const SDL_Event* event);

    void process_mouse_event(const SDL_Event* event)
    {
        this->inject_SDL_mouse_event(event);
    }

    private:
    void inject_SDL_key_event(const SDL_Event* event);
    void inject_SDL_mouse_event(const SDL_Event* event);
};

}   // Awesomium

#endif
