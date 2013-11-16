/* Copyright © 2013 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 */

/* Cart-Pole graphics for Inverted Pendulum */

#include <assert.h>
#include <unistd.h>
#include <stdio.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

#define WIDTH 400
#define HEIGHT 300

xcb_connection_t *c;
xcb_window_t window;
int window_active = 0;

/*
 * Thanks to Vincent Torri for his XCB tutorial on which
 * some of this code is based. You'd think I of all people
 * could remember this stuff...
 * 
 * Thanks also to LiteratePrograms.org for the Cairo tutorial
 * I followed.
 */

void init_window(void) {
    assert(!window_active);

    /* set up the X connection */
    int nscreen = 0;
    c = xcb_connect(0, &nscreen);
    assert(c);
    xcb_screen_t *screen = xcb_aux_get_screen(c, nscreen);
    assert(screen);

    /* set up the display window and background pixmap */
    window = xcb_generate_id(c);
    xcb_pixmap_t background = xcb_generate_id(c);
    xcb_params_cw_t cw_params;
    cw_params.back_pixmap = background;
    cw_params.event_mask = XCB_EVENT_MASK_EXPOSURE;
    (void) xcb_aux_create_window_checked(c, XCB_COPY_FROM_PARENT,
                                         window, screen->root,
                                         0, 0, WIDTH, HEIGHT, 0,
                                         XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                         screen->root_visual,
                                         XCB_CW_BACK_PIXMAP |
                                         XCB_CW_EVENT_MASK,
                                         &cw_params);
    (void) xcb_create_pixmap_checked(c, XCB_COPY_FROM_PARENT,
                                     background, window, WIDTH, HEIGHT);

    /* create and paint the background surface */
    xcb_visualtype_t *visualtype =
        xcb_aux_find_visual_by_id(screen, screen->root_visual);
    assert(visualtype);
    cairo_surface_t *surface =
        cairo_xcb_surface_create(c, background, visualtype,
                                 WIDTH, HEIGHT);
    assert(surface);
    cairo_t *cs = cairo_create(surface);
    assert(cs);
    cairo_rectangle(cs, 0.0, 0.0, WIDTH, HEIGHT);
    cairo_set_source_rgb(cs, 0.0, 0.0, 0.5);
    cairo_fill(cs);
    cairo_move_to(cs, 10.0, HEIGHT - 10.0);
    cairo_line_to(cs, WIDTH - 10.0, HEIGHT - 10.0);
    cairo_destroy(cs);

    /* get the window onscreen and rendered */
    (void) xcb_map_window_checked(c, window);
    (void) xcb_aux_sync(c);

    do {
        fprintf(stderr, "waiting for exposure\n");
        xcb_generic_event_t * event = xcb_wait_for_event(c);
        if ((event->response_type & ~0x80) != XCB_EXPOSE)
            continue;
        fprintf(stderr, "exposure received\n");
        (void) xcb_clear_area_checked(c, 0, window, 0, 0, WIDTH, HEIGHT);
        (void) xcb_aux_sync(c);
    } while(0);

    window_active = 1;
}

void destroy_window(void) {
    if (!window_active)
        return;
    (void) xcb_disconnect(c);
}

int main() {
    init_window();
    sleep(5);
    destroy_window();
    return 0;
}
