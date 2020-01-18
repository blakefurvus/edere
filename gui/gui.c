#include "gui.h"


// Defaults for state elements
#define GUI_TEXT_DEFAULT { \
    NULL, "JetBrainsMono-Regular.ttf", 13, {0, 0, 0} \
}
#define GUI_DISPLAY_DEFAULT { \
    'f', {0, 0, 0, 0}, 'c', 's', 's' \
}
#define GUI_STYLE_DEFAULT { \
    GUI_TEXT_DEFAULT, \
    GUI_DISPLAY_DEFAULT \
}


// State
struct gui_state_t {

    // Library specific things
    struct {
        SDL_Window *window;
        SDL_Surface *surface;
        bool is_running;
        int width;
        int height;
    } app;

    // Events
    struct {
        int x;
        int y;
        bool is_down;
    } mouse;

    // Styling
    struct gui_style_t {

        // Text
        struct gui_text_t {
            TTF_Font *_font;
            char *family;
            int size;
            SDL_Color color;
        } text;

        // Display
        struct gui_display_t {
            char type;

            // Flex
            SDL_Rect frame;
            char direction;
            char justify_content;
            char align_items;

            // Fixed
            SDL_Rect position;
        } display;

    } style;

    // UI state
    struct {
        int active_item;
    } state;

}
// Initialize
gui = {
    {NULL, NULL, false, 800, 600},
    {0, 0, false},
    GUI_STYLE_DEFAULT,
    {0}
};


/*
 *  HELPER FUNCTIONS
 */

// Pushes other elements by x & y
// Depending on display type (if flex)
void push_frame(int x, int y)
{
    if (gui.style.display.type == 'f') {

        if (gui.style.display.direction == 'r') {
            gui.style.display.frame.x += x;
            gui.style.display.frame.w -= x;
        } else if (gui.style.display.direction == 'c') {
            gui.style.display.frame.y += y;
            gui.style.display.frame.h += y;
        }

    }
}

// Returns where to render the widget
SDL_Rect get_pos(int w, int h)
{
    // Get x and y depending on display type
    int x =
        gui.style.display.type == 'f'
            ? gui.style.display.frame.x
            : gui.style.display.position.x;
    int y =
        gui.style.display.type == 'f'
            ? gui.style.display.frame.y
            : gui.style.display.position.y;

    // Rectangle to return
    SDL_Rect ret = {x, y, w, h};

    // Calculate
    if (gui.style.display.type == 'f') {
        if (gui.style.display.direction == 'r') {

            if (gui.style.display.justify_content == 'c')
                ret.x = (gui.style.display.frame.w - w) / 2;
            else if (gui.style.display.justify_content == 'e')
                ret.x = gui.style.display.frame.w - w;

            if (gui.style.display.align_items == 'c')
                ret.y = (gui.style.display.frame.h - h) / 2;
            else if (gui.style.display.align_items == 'e')
                ret.y = gui.style.display.frame.h - h;

        } else if (gui.style.display.direction == 'c') {

            if (gui.style.display.justify_content == 'c')
                ret.y = (gui.style.display.frame.h - h) / 2;
            else if (gui.style.display.justify_content == 'e')
                ret.y = gui.style.display.frame.h - h;

            if (gui.style.display.align_items == 'c')
                ret.x = (gui.style.display.frame.w - w) / 2;
            else if (gui.style.display.align_items == 'e')
                ret.x = gui.style.display.frame.w - w;

        }

        push_frame(ret.w, ret.h);
    }

    return ret;
}

// Can be used to reset/initialize gui.style.text
bool reset_text() {
    if (gui.style.text._font)
        TTF_CloseFont(gui.style.text._font);
    gui.style.text = (struct gui_text_t)GUI_TEXT_DEFAULT;

    gui.style.text._font = TTF_OpenFont(
        gui.style.text.family, gui.style.text.size
    );

    if (!gui.style.text._font) {
        puts("Failed to load font");
        return false;
    }
    return true;
}

// Can be used to reset gui.style.display
// A version of GUI_DISPLAY_DEFAULT that doesn't mutate display.frame
void reset_display()
{
    gui.style.display.type = 'f';
    gui.style.display.position = (SDL_Rect){0, 0, 0, 0};
}

// Checks if mouse is hovering on a region
bool region_hit(int x, int y, int w, int h)
{
    // Check if mouse hits
    if (
        gui.mouse.x < x || gui.mouse.y < y
        ||
        gui.mouse.x >= x + w || gui.mouse.y >= y + h
    )
        return false;
    else
        return true;
}

// Maps the value n between start1 and stop1 to start2 and stop2
float p5map(float n, float start1, float stop1, float start2, float stop2)
{
    return ((n - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}


/*
 * LIBRARY FUNCTIONS
 */

// Initializes library
bool gui_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        puts("Failed to initialize SDL2");
        return false;
    }

    if (TTF_Init() < 0) {
        puts("Failed to initialize SDL_ttf");
        return false;
    }

    gui.app.window = SDL_CreateWindow(
        "Edere",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        gui.app.width, gui.app.height,
        SDL_WINDOW_SHOWN
    );
    if (gui.app.window == NULL) {
        puts("Failed to create window");
        return false;
    }
    SDL_SetWindowResizable(gui.app.window, true);

    gui.app.is_running = true;
    atexit(gui_halt);
    return true;
}

// Deinitialize library
void gui_halt()
{
    gui.app.is_running = false;

    TTF_CloseFont(gui.style.text._font);
    SDL_DestroyWindow(gui.app.window);

    TTF_Quit();
    SDL_Quit();
}

// Getters for some state data
bool gui_running() { return gui.app.is_running; }
int gui_width() { return gui.app.width; }
int gui_height() { return gui.app.height; }

// Clears screen and resets display styling
// Also, text styling
void gui_clear()
{
    reset_text();

    // We are doing this, beacuse when window resizes
    // surface's size does not update
    gui.app.surface = SDL_GetWindowSurface(gui.app.window);

    gui.style.display = (struct gui_display_t)GUI_DISPLAY_DEFAULT;
    gui.style.display.frame = (SDL_Rect){0, 0, gui_width(), gui_height()};

    SDL_FillRect(gui.app.surface, NULL, 0xffffff);
}

// Polls events from SDL
// Then updates state
void gui_poll_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        // Mouse position
        case SDL_MOUSEMOTION:
            gui.mouse.x = e.motion.x;
            gui.mouse.y = e.motion.y;
            break;

        // Mouse is_down
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button = SDL_BUTTON_LEFT)
                gui.mouse.is_down = true;
            break;
        case SDL_MOUSEBUTTONUP:
            if (e.button.button = SDL_BUTTON_LEFT)
                gui.mouse.is_down = false;
            break;

        // Resize
        case SDL_WINDOWEVENT:
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                SDL_GetWindowSize(
                    gui.app.window,
                    &gui.app.width,
                    &gui.app.height
                );
            break;

        // Quit
        case SDL_QUIT:
            gui.app.is_running = false;
            break;

        // Default
        default:
            break;

        }
    }
}

// Well.. too obvius (really?)
void gui_update()
{
    SDL_UpdateWindowSurface(gui.app.window);
}


/*
 * STYLING
 *
 * This section is setters for the states in gui.style
 * No description for functions here
 */

void gui_style_text_size(int size)
{
    if (gui.style.text._font)
        TTF_CloseFont(gui.style.text._font);

    gui.style.text.size = size;
    gui.style.text._font = TTF_OpenFont(
        gui.style.text.family,
        gui.style.text.size
    );
}

void gui_style_display(char t) { gui.style.display.type = t; }

void gui_style_position(int x, int y)
{
    gui.style.display.position = (SDL_Rect){x, y};
}

void gui_style_justify_content(char v)
{
    gui.style.display.justify_content = v;
}

void gui_style_align_items(char v)
{
    gui.style.display.align_items = v;
}


/*
 * WIDGETS
 *
 * These are widgets
 * Simplest one is _gui_text
 * Reading it carefully itself is a briefing tbh
 */

void _gui_text(int id, const char *s)
{
    // Render s to surface
    SDL_Surface *surface = TTF_RenderText_Blended(
        gui.style.text._font,
        s,
        gui.style.text.color
    );

    // Render
    SDL_Rect pos = get_pos(surface->w, surface->h);
    SDL_BlitSurface(surface, NULL, gui.app.surface, &pos);
    SDL_FreeSurface(surface);

    // Reset styling
    reset_text();
}

bool _gui_button(int id, char *s)
{
    // Button state
    static const int padding = 15;
    Uint32 bg = 0xcecece;
    SDL_Color clr = {0, 0, 0};
    bool ret = false;

    // Process text
    char ss[120];
    unsigned int si = 0;
    while (*s)
        ss[si++] = toupper(*s++);
    ss[si] = '\0';

    // Get dimensions
    int w, h;
    TTF_SizeText(gui.style.text._font, ss, &w, &h);
    SDL_Rect pos = get_pos(w + padding * 2, h + padding * 2);
    bool hovered = region_hit(pos.x, pos.y, pos.w, pos.h);

    // Hot
    if (hovered) {
        bg = 0x00bfff;
        clr = (SDL_Color){255, 255, 255};

        if (gui.mouse.is_down && gui.state.active_item == 0)
            gui.state.active_item = id;
    }
    else if (!hovered && gui.state.active_item == id)
        gui.state.active_item = 0;

    // Clicked
    if (gui.state.active_item == id && !gui.mouse.is_down) {
        gui.state.active_item = 0;
        ret = true;
    }

    // Render
    SDL_Surface *text = TTF_RenderText_Blended(gui.style.text._font, ss, clr);
    SDL_FillRect(gui.app.surface, &pos, bg);

    pos.x += padding;
    pos.y += padding;
    pos.w -= padding;
    pos.h -= padding;
    SDL_BlitSurface(text, NULL, gui.app.surface, &pos);
    SDL_FreeSurface(text);

    return ret;
}

void _gui_vslider(int id, int h, float *p, float mn, float mx)
{
    static const int w = 15;
    Uint32 color = 0xaaaaaa;

    SDL_Rect xny =
        gui.style.display.type == 'f'
            ? gui.style.display.frame
            : gui.style.display.position;
    bool hovered = region_hit(xny.x, xny.y, w, h);

    // Hot
    if (hovered) {
        color = 0x00bfff;

        if (gui.mouse.is_down && gui.state.active_item == 0)
            gui.state.active_item = id;
    }

    // Clicked
    if (gui.state.active_item == id && gui.mouse.is_down) {
        int pos = gui.mouse.y - xny.y - w / 2;

        if (pos < 0) pos = 0;
        else if (pos > h - w) pos = h - w;
        *p = p5map(pos, 0, h - w, mn, mx);
        color = 0x00bfff;
    }
    else if (gui.state.active_item == id && !gui.mouse.is_down)
        gui.state.active_item = 0;

    // Render
    xny.w = w;
    xny.h = h;
    SDL_FillRect(gui.app.surface, &xny, 0xcecece);

    xny.y += p5map(*p, mn, mx, 0, h - w);
    xny.w = w;
    xny.h = w;
    SDL_FillRect(gui.app.surface, &xny, color);

    // Positioning
    push_frame(w, h);
    reset_display();
}

void _gui_hslider(int id, int w, float *p, float mn, float mx)
{
    static const int h = 15;
    Uint32 color = 0xaaaaaa;

    SDL_Rect xny =
        gui.style.display.type == 'f'
            ? gui.style.display.frame
            : gui.style.display.position;
    bool hovered = region_hit(xny.x, xny.y, w, h);

    // Hot
    if (hovered) {
        color = 0x00bfff;

        if (gui.mouse.is_down && gui.state.active_item == 0)
            gui.state.active_item = id;
    }

    // Clicked
    if (gui.state.active_item == id && gui.mouse.is_down) {
        int pos = gui.mouse.x - xny.x - h / 2;

        if (pos < 0) pos = 0;
        else if (pos > w - h) pos = w - h;
        *p = p5map(pos, 0, w - h, mn, mx);
        color = 0x00bfff;
    }
    else if (gui.state.active_item == id && !gui.mouse.is_down)
        gui.state.active_item = 0;

    // Render
    xny.w = w;
    xny.h = h;
    SDL_FillRect(gui.app.surface, &xny, 0xcecece);

    xny.x += p5map(*p, mn, mx, 0, w - h);
    xny.w = h;
    xny.h = h;
    SDL_FillRect(gui.app.surface, &xny, color);

    // Positioning
    push_frame(w, h);
    reset_display();
}

void _gui_vborder(int id, int *x, int y, int h, int mn, int mx)
{
    Uint32 color = 0xcecece;
    bool hovered = region_hit(*x, y, 5, h);

    if (hovered) {
        if (gui.mouse.is_down)
            gui.state.active_item = id;
    }

    if (gui.state.active_item == id && gui.mouse.is_down) {
        color = 0x00bfff;
        *x = gui.mouse.x;
        if (*x < mn) *x = mn;
        else if (*x > mx) *x = mx;
    }
    else if (gui.state.active_item == id && !gui.mouse.is_down)
        gui.state.active_item = 0;

    SDL_Rect temp = {*x, y, 5, h};
    SDL_FillRect(gui.app.surface, &temp, color);
}