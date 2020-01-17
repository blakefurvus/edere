#define GUI_IMPLEMENTATION
#include "gui.h"

struct {
    int sidebar_x;
    float slider_value;
} app_state = {250, 0};

void draw_sidebar()
{
    // Border
    gui_vborder(&app_state.sidebar_x, 0, gui_height(), 200, gui_width());

    // Text
    gui_text('c', "Project", 0, 0, app_state.sidebar_x, 40);
}

bool draw_editor()
{
    // Print value to string
    char buf[200];
    sprintf(buf, "Value is %d!", (int)app_state.slider_value);

    // Button
    if (gui_button(buf, app_state.sidebar_x + 20, 50))
        return false;

    // Slider
    gui_vslider(gui_width() - 15, 0, 15, gui_height(), &app_state.slider_value, 0, 100);

    return true;
}

int main()
{
    assert(gui_init());

    while (gui_running()) {
        gui_reset_surface();

        draw_sidebar();

        if (!draw_editor()) {
            gui_halt();
            break;
        }

        gui_handle_events();
        gui_update();
    }

    gui_halt();
}