/* Copyright © 2013 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file COPYING in the source
 * distribution of this software for license terms.
 */

/* Cart-Pole graphics for Inverted Pendulum */

#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define __USE_BSD
#include <math.h>

#include <cairo/cairo.h>
#include <cairo/cairo-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

#define WIDTH 400
#define HEIGHT 300
#define TRACK_HEIGHT (HEIGHT - 50.0)
#define TRACK_OFFSET 10.0
#define TRACK_WIDTH_PIXELS (WIDTH - 2 * TRACK_OFFSET)
#define CART_HEIGHT 20.0
#define CART_WIDTH 20.0
#define WHEEL_RADIUS 3.0

double rod_length, track_width;

xcb_connection_t *c;
xcb_window_t window;
int window_active = 0;

/* cart drawing surface */
cairo_t *cas;

/*
 * Thanks to Vincent Torri for his XCB tutorial on which
 * some of this code is based. You'd think I of all people
 * could remember this stuff...
 * 
 * Thanks also to LiteratePrograms.org for the Cairo tutorial
 * I followed.
 */

void check_request(char *desc, xcb_void_cookie_t r) {
    xcb_generic_error_t *e = xcb_request_check(c, r);
    if (!e)
        return;
    fprintf(stderr, "xcb request check failed on %s:\n", desc);
    fprintf(stderr, "  error: %u\n", e->error_code);
    fprintf(stderr, "  opcode: %u/%u\n", e->major_code, e->minor_code);
    fprintf(stderr, "  sequence: %u\n", e->full_sequence);
    exit(1);
}

void draw_background(cairo_t *cs) {
    /* draw the light blue background */
    cairo_rectangle(cs, -1.0, -1.0, WIDTH + 1.0, HEIGHT + 1.0);
    cairo_set_source_rgb(cs, 0.9, 0.9, 1.0);
    cairo_fill(cs);

    /* draw the red track */
    cairo_set_source_rgb(cs, 0.9, 0.3, 0.3);
    cairo_move_to(cs, 10.0, TRACK_HEIGHT);
    cairo_line_to(cs, WIDTH - 10.0, TRACK_HEIGHT);
    cairo_stroke(cs);
}

void init_window(double length, double width) {
    assert(!window_active);
    track_width = TRACK_WIDTH_PIXELS / width;
    rod_length = length / track_width;

    /* set up the X connection */
    int nscreen = 0;
    c = xcb_connect(0, &nscreen);
    assert(c);
    xcb_screen_t *screen = xcb_aux_get_screen(c, nscreen);
    assert(screen);

    /* set up the display window and background pixmap */
    window = xcb_generate_id(c);
    xcb_void_cookie_t result =
        xcb_aux_create_window_checked(c, XCB_COPY_FROM_PARENT,
                                      window, screen->root,
                                      0, 0, WIDTH, HEIGHT, 0,
                                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                                      screen->root_visual,
                                      0, 0);
    check_request("create_window", result);
    xcb_pixmap_t background = xcb_generate_id(c);
    result = xcb_create_pixmap_checked(c, 24, background, window,
                                       WIDTH, HEIGHT);
    check_request("create_pixmap", result);
    xcb_params_cw_t cw_params;
    cw_params.back_pixmap = background;
    result = xcb_aux_change_window_attributes_checked(c, window,
                                                      XCB_CW_BACK_PIXMAP,
                                                      &cw_params);
    check_request("change_window_attributes", result);

    /* create and paint the background surface */
    xcb_visualtype_t *visualtype =
        xcb_aux_find_visual_by_id(screen, screen->root_visual);
    assert(visualtype);
    cairo_surface_t *surface =
        cairo_xcb_surface_create(c, background, visualtype, WIDTH, HEIGHT);
    assert(surface);
    cairo_t *cs = cairo_create(surface);
    assert(cs);
    draw_background(cs);
    cairo_destroy(cs);

    /* get the window onscreen and rendered */
    result = xcb_clear_area_checked(c, 0, window, 0, 0, WIDTH, HEIGHT);
    check_request("clear_area", result);
    result = xcb_map_window_checked(c, window);
    check_request("map_window", result);
    (void) xcb_aux_sync(c);

    /* set up the cart surface for later use */
    surface = 
        cairo_xcb_surface_create(c, window, visualtype, WIDTH, HEIGHT);
    assert(surface);
    cas = cairo_create(surface);
    assert(cas);

    window_active = 1;
}

void draw_wheel(double x) {
    double y = TRACK_HEIGHT - WHEEL_RADIUS;
    fprintf(stderr, "%g %g\n", x, y);
    cairo_arc(cas, x, y, WHEEL_RADIUS, 0, 2 * M_PI);
}

void draw_cart(double x, double theta) {
    /* draw the cart wheels */
    double cart_x =
        (x / track_width) * TRACK_WIDTH_PIXELS +
        TRACK_OFFSET - CART_WIDTH / 2.0;
    draw_wheel(cart_x + CART_WIDTH / 3.0);
    draw_wheel(cart_x + 2 * CART_WIDTH / 3.0);

    /* push the cart drawing out */
    cairo_set_source_rgb(cas, 0.3, 0.9, 0.3);
    cairo_fill(cas);
    assert(cairo_status(cas) == 0);
    (void) xcb_aux_sync(c);
}

void destroy_window(void) {
    if (!window_active)
        return;
    cairo_destroy(cas);
    (void) xcb_disconnect(c);
}

int main() {
    init_window(20.0, 10.0);
    draw_cart(5.0, 0.0);
    sleep(5);
    destroy_window();
    return 0;
}
