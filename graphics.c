/* Copyright © 2013 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 */

/* Cart-Pole graphics for Inverted Pendulum */

#include <assert.h>
#include <unistd.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

#define WIDTH 400
#define HEIGHT 300

xcb_connection_t *c;
xcb_window_t window;
int window_active = 0;

void init_window(void) {
    assert(!window_active);
    int nscreen = 0;
    c = xcb_connect(0, &nscreen);
    assert(c);
    xcb_screen_t *screen = xcb_aux_get_screen(c, nscreen);
    assert(screen);
    window = xcb_generate_id(c);
    (void) xcb_aux_create_window_checked(c, XCB_COPY_FROM_PARENT,
                                         window, screen->root,
                                         0, 0, WIDTH, HEIGHT, 0,
                                         XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                         screen->root_visual,
                                         0, 0);
    (void) xcb_map_window_checked(c, window);
    (void) xcb_aux_sync(c);
    window_active = 1;
}

void destroy_window(void) {
    if (!window_active)
        return;
    (void) xcb_disconnect(c);
}
