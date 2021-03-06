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
#define __USE_XOPEN
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
#define CART_HEIGHT 10.0
#define CART_WIDTH 40.0
#define WHEEL_RADIUS 6.0
#define MASS_RADIUS 8.0

static double rod_length, track_width;

static xcb_connection_t *c;
static xcb_window_t window;
static int window_active = 0;

/* cart drawing surface */
static cairo_t *cws;
static cairo_t *cas;

/*
 * Thanks to Vincent Torri for his XCB tutorial on which
 * some of this code is based. You'd think I of all people
 * could remember this stuff...
 * 
 * Thanks also to LiteratePrograms.org for the Cairo tutorial
 * I followed.
 */

static void check_request(char *desc, xcb_void_cookie_t r) {
    xcb_generic_error_t *e = xcb_request_check(c, r);
    if (!e)
        return;
    fprintf(stderr, "xcb request check failed on %s:\n", desc);
    fprintf(stderr, "  error: %u\n", e->error_code);
    fprintf(stderr, "  opcode: %u/%u\n", e->major_code, e->minor_code);
    fprintf(stderr, "  sequence: %u\n", e->full_sequence);
    exit(1);
}

static void draw_background(cairo_t *cs) {
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

static void save_dims(double length, double width) {
    rod_length = length;
    track_width = width;
}

void init_window(double rod_length, double track_width) {
    assert(!window_active);
    save_dims(rod_length, track_width);

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

    xcb_visualtype_t *visualtype = xcb_aux_find_visual_by_id(screen, screen->root_visual);

    cairo_surface_t *window_surface = cairo_xcb_surface_create(c, window, visualtype, WIDTH, HEIGHT);

    cairo_surface_t *pixmap_surface = cairo_surface_create_similar(window_surface, CAIRO_CONTENT_COLOR, WIDTH, HEIGHT);

    /* get the window onscreen and rendered */
    result = xcb_map_window_checked(c, window);
    check_request("map_window", result);
    (void) xcb_aux_sync(c);

    /* set up the cart surface for later use */

    cws = cairo_create(window_surface);
    assert(cws);

    cas = cairo_create(pixmap_surface);
    assert(cas);

    cairo_set_source_surface(cws, pixmap_surface, 0, 0);

    window_active = 1;
}

void clear_cart(void) {
	cairo_set_source_rgb(cas, 1, 1, 1);
	cairo_paint(cas);
}

void draw_cart(double x, double theta) {
    clear_cart();

    draw_background(cas);

    /* position the cart */
    double cart_x =
        (x / track_width) * TRACK_WIDTH_PIXELS +
        TRACK_OFFSET - CART_WIDTH / 2.0;
    double cart_y =
        TRACK_HEIGHT - WHEEL_RADIUS;

    /* draw the cart wheels */
    cairo_set_source_rgb(cas, 0.0, 0.0, 0.0);
    cairo_arc(cas, cart_x + CART_WIDTH / 4.0, cart_y,
              WHEEL_RADIUS, 0, 2 * M_PI);
    cairo_arc(cas, cart_x + 3.0 * CART_WIDTH / 4.0, cart_y,
              WHEEL_RADIUS, 0, 2 * M_PI);
    cairo_fill(cas);

    /* position the cart rod */
    double rod_bottom_x = cart_x + CART_WIDTH / 2.0;
    double rod_bottom_y = cart_y - CART_HEIGHT / 2.0;
    double adj_length = rod_length * TRACK_WIDTH_PIXELS / track_width;
    double rod_top_x = rod_bottom_x + adj_length * sin(theta);
    double rod_top_y = rod_bottom_y - adj_length * cos(theta);
    if (rod_top_y > TRACK_HEIGHT - MASS_RADIUS - CART_HEIGHT / 2.0) {
        rod_top_y = TRACK_HEIGHT - MASS_RADIUS;
        double dy = rod_top_y - rod_bottom_y;
        double dx = sqrt(adj_length * adj_length - dy * dy);
        if (rod_top_x < rod_bottom_x)
            dx = -dx;
        rod_top_x = cart_x + dx;
    }

    /* draw the cart rod */
    cairo_move_to(cas, rod_bottom_x, rod_bottom_y);
    cairo_line_to(cas, rod_top_x, rod_top_y);
    cairo_stroke(cas);

    /* draw the cart mass */
    cairo_set_source_rgb(cas, 0.7, 0.4, 0.2);
    cairo_arc(cas, rod_top_x, rod_top_y, MASS_RADIUS, 0, 2 * M_PI);
    cairo_fill(cas);

    /* draw the cart body */
    cairo_set_source_rgb(cas, 0.3, 0.9, 0.3);
    cairo_rectangle(cas, cart_x, cart_y - CART_HEIGHT,
                    CART_WIDTH, CART_HEIGHT);
    cairo_fill(cas);

    cairo_paint(cws);
    
    /* push the cart drawing out */
    assert(cairo_status(cas) == 0);
    (void) xcb_flush(c);
}

void destroy_window(void) {
    if (!window_active)
        return;
    cairo_destroy(cas);
    (void) xcb_disconnect(c);
}
