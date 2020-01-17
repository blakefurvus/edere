#include "gui.h"

struct gui_state_t {

    // Library specific things
    struct {
        SDL_Window *window;
        SDL_Surface *surface;
        bool is_running;
        int width;
        int height;
    } app;

    // Event state
    struct {
        int x;
        int y;
        bool is_down;
    } mouse;

    // Styling state
    struct {
        TTF_Font *font;
        SDL_Color color;
    } style;

    // UI state
    struct {
        int active_item;
    } state;

} gui = {
    {NULL, NULL, false, 800, 600},
    {0, 0, false},
    {NULL, {0, 0, 0}},
    {0}
};


bool gui_init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return false;

    if (TTF_Init() < 0)
        return false;

    gui.app.window = SDL_CreateWindow(
        "Edere",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        gui.app.width, gui.app.height,
        SDL_WINDOW_SHOWN
    );
    if (gui.app.window == NULL)
        return false;

    gui.style.font = TTF_OpenFont("JetBrainsMono-Regular.ttf", 13);
    if (gui.style.font == NULL)
        return false;

    gui.app.is_running = true;
    return true;
}

void gui_halt()
{
    gui.app.is_running = false;

    SDL_DestroyWindow(gui.app.window);
    TTF_CloseFont(gui.style.font);

    TTF_Quit();
    SDL_Quit();
}

bool gui_running()
{
    return gui.app.is_running;
}

int gui_width()
{
    return gui.app.width;
}

int gui_height()
{
    return gui.app.height;
}

void gui_reset_surface()
{
    gui.app.surface = SDL_GetWindowSurface(gui.app.window);
    SDL_FillRect(gui.app.surface, NULL, 0xffffff);
}

void gui_handle_events()
{
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        switch (e.type) {

        // Mouse position
        case SDL_MOUSEMOTION:
            gui.mouse.x = e.motion.x;
            gui.mouse.y = e.motion.y;
            break;

        // Mouse is_down?
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button = SDL_BUTTON_LEFT)
                gui.mouse.is_down = true;
            break;
        case SDL_MOUSEBUTTONUP:
            if (e.button.button = SDL_BUTTON_LEFT)
                gui.mouse.is_down = false;
            break;

        // Resize
        case SDL_WINDOWEVENT_RESIZED:
            SDL_GetWindowSize(gui.app.window, &gui.app.width, &gui.app.height);
            break;

        // Quit event
        case SDL_QUIT:
            gui.app.is_running = false;
            break;

        // Default
        default:
            break;

        }
    }
}

void gui_update()
{
    SDL_UpdateWindowSurface(gui.app.window);
}

// Helper functions for widgets
bool gui_regionhit(int x, int y, int w, int h)
{
    if (gui.mouse.x < x ||
        gui.mouse.y < y ||
        gui.mouse.x >= x + w ||
        gui.mouse.y >= y + h)
        return false;
    return true;
}
float p5map(float n, float start1, float stop1, float start2, float stop2)
{
    return ((n - start1) / (stop1 - start1)) * (stop2 - start2) + start2;
}

bool _gui_button(int id, char *s, int x, int y)
{
    // Button state
    static const int padding = 15;
    Uint32 bg = 0xcecece;
    SDL_Color clr = {0, 0, 0};
    bool ret = false;

    // Text
    char ss[120];
    unsigned int si = 0;
    while (*s)
        ss[si++] = toupper(*s++);
    ss[si] = '\0';

    int w, h;
    TTF_SizeText(gui.style.font, ss, &w, &h);
    bool regionhit = gui_regionhit(x, y, w + padding * 2, h + padding * 2);

    // Hot
    if (regionhit) {
        bg = 0x00bfff;
        clr = (SDL_Color){255, 255, 255};

        if (gui.mouse.is_down && gui.state.active_item == 0)
            gui.state.active_item = id;
    }
    else if (!regionhit && gui.state.active_item == id)
        gui.state.active_item = 0;

    // Clicked
    if (gui.state.active_item == id && !gui.mouse.is_down) {
        gui.state.active_item = 0;
        ret = true;
    }

    // Render
    SDL_Surface *text = TTF_RenderText_Blended(gui.style.font, ss, clr);
    SDL_Rect rect = {x, y, w + padding * 2, h + padding * 2};
    SDL_FillRect(gui.app.surface, &rect, bg);

    SDL_Rect temp = {x + padding, y + padding, text->w, text->h};
    SDL_BlitSurface(text, NULL, gui.app.surface, &temp);
    SDL_FreeSurface(text);

    return ret;
}

void _gui_text(int id, char align, char *s, int x, int y, int w , int h)
{
    // Render to surface
    SDL_Surface *surface = TTF_RenderText_Blended(gui.style.font, s, gui.style.color);

    // Alignment
    SDL_Rect temp = {
        x,
        y + (h - surface->h) / 2,
        w,
        h,
    };

    if (align == 'c')
        temp.x += (w - surface->w) / 2;
    else if (align == 'r')
        temp.x += w - surface->w;

    // Blit to screen
    SDL_BlitSurface(surface, NULL, gui.app.surface, &temp);
    SDL_FreeSurface(surface);
}

void _gui_vslider(int id, int x, int y, int w, int h, float *p, float mn, float mx)
{
    Uint32 color = 0xaaaaaa;
    bool regionhit = gui_regionhit(x, y, w, h);

    // Hot
    if (regionhit) {
        color = 0x00bfff;

        if (gui.mouse.is_down && gui.state.active_item == 0)
            gui.state.active_item = id;
    }

    // Clicked
    if (gui.state.active_item == id && gui.mouse.is_down) {
        int pos = gui.mouse.y - y - w / 2;

        if (pos < 0) pos = 0;
        else if (pos > h - w) pos = h - w;
        *p = p5map(pos, 0, h - w, mn, mx);
        color = 0x00bfff;
    }
    else if (gui.state.active_item == id && !gui.mouse.is_down)
        gui.state.active_item = 0;

    // Render
    SDL_Rect temp = {x, y, w, h};
    SDL_FillRect(gui.app.surface, &temp, 0xcecece);

    temp.y = p5map(*p, mn, mx, 0, h - w) + y;
    temp.w = w;
    temp.h = w;
    SDL_FillRect(gui.app.surface, &temp, color);
}

void _gui_hslider(int id, int x, int y, int w, int h, float *p, float mn, float mx)
{
    Uint32 color = 0xaaaaaa;
    bool regionhit = gui_regionhit(x, y, w, h);

    // Hot
    if (regionhit) {
        color = 0x00bfff;

        if (gui.mouse.is_down && gui.state.active_item == 0)
            gui.state.active_item = id;
    }

    // Clicked
    if (gui.state.active_item == id && gui.mouse.is_down) {
        int pos = gui.mouse.x - x - h / 2;

        if (pos < 0) pos = 0;
        else if (pos > w - h) pos = w - h;
        *p = p5map(pos, 0, w - h, mn, mx);
        color = 0x00bfff;
    }
    else if (gui.state.active_item == id && !gui.mouse.is_down)
        gui.state.active_item = 0;

    // Render
    SDL_Rect temp = {x, y, w, h};
    SDL_FillRect(gui.app.surface, &temp, 0xcecece);

    temp.x = p5map(*p, mn, mx, 0, w - h) + x;
    temp.w = h;
    temp.h = h;
    SDL_FillRect(gui.app.surface, &temp, color);
}

void _gui_vborder(int id, int *x, int y, int h, int mn, int mx)
{
    Uint32 color = 0xcecece;
    bool regionhit = gui_regionhit(*x, y, 5, h);

    if (regionhit) {
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