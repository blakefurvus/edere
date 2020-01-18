#ifndef GUI_H
#define GUI_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * LIBRARIES
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Whole state (yeah :p)
struct gui_state_t;


/*
 * MACROS
 */

// Generate unique id
#ifdef GUI_SRC_ID
#define GEN_ID ((GUI_SRC_ID) + (__LINE__))
#else
#define GEN_ID (__LINE__)
#endif


/*
 * LIBRARY FUNCTIONS
 */

// Window stuff
bool gui_init();
void gui_halt();
bool gui_running();
int gui_width();
int gui_height();

// Events
void gui_clear();
void gui_poll_events();
void gui_update();


/*
 * STYLING
 */

void gui_style_text_size(int size);
void gui_style_display(char t);
void gui_style_position(int x, int y);
void gui_style_justify_content(char v);
void gui_style_align_items(char v);


/*
 * WIDGETS
 */

#define gui_text(...) _gui_text(GEN_ID, __VA_ARGS__)
void _gui_text(int id, const char *s);

#define gui_button(...) _gui_button(GEN_ID, __VA_ARGS__)
bool _gui_button(int id, char *s);

#define gui_vslider(...) _gui_vslider(GEN_ID, __VA_ARGS__)
void _gui_vslider(int id, int h, float *p, float mn, float mx);

#define gui_hslider(...) _gui_hslider(GEN_ID, __VA_ARGS__)
void _gui_hslider(int id, int w, float *p, float mn, float mx);

#define gui_vborder(...) _gui_vborder(GEN_ID, __VA_ARGS__)
void _gui_vborder(int id, int *x, int y, int h, int mn, int mx);


// Implementation
#ifdef GUI_IMPLEMENTATION
#include "gui.c"
#endif


#ifdef __cplusplus
}
#endif

#endif