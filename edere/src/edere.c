#define GUI_IMPLEMENTATION
#include "gui.h"

// struct {
//     int sidebar_x;
//     float value;
// } state = {250, 0};
//
// void draw_sidebar()
// {
//     // Border
//     gui_vborder(&state.sidebar_x, 0, gui_height(), 200, gui_width());
//
//     // Text
//     gui_text('c', "Project", 0, 0, state.sidebar_x, 40);
// }
//
// bool draw_editor()
// {
//     // Print value to string
//     char buf[200];
//     sprintf(buf, "Value is %d!", (int)state.value);
//
//     // Button
//     if (gui_button(buf, state.sidebar_x + 20, 50))
//         return false;
//
//     // Slider
//     gui_vslider(gui_width() - 15, 0, 15, gui_height(), &state.value, 0, 100);
//
//     return true;
// }

int main()
{
    assert(gui_init());

    // State
    float value = 20;

    while (gui_running()) {
        gui_clear();

        // draw_sidebar();
        //
        // if (!draw_editor()) {
        //     gui_halt();
        //     break;
        // }

        gui_style_display('x');
        if (gui_button("Click me to exit!"))
            break;

        gui_style_display('x');
        gui_style_position(gui_width() - 50, gui_height() / 4);
        gui_vslider(gui_height() / 2, &value, 0, 100);

        gui_style_justify_content('c');
        gui_style_align_items('c');
        gui_style_text_size((int)value);
        gui_text("Hello World!");

        // gui_style_text_size((int)value / 2);
        // gui_text("Hi World!");

        gui_poll_events();
        gui_update();
    }
}