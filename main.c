/* mandelbrot-viewer -- Mandelbrot set viewer
   Copyright (C) 2025 Joaquim

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* #include <stdio.h> */
#include <gtk/gtk.h>
#include <math.h>
#include <complex.h>
#include <tgmath.h>

/* TODO:
 * Allow for reloading the image without recompiling the program.
 * Why are all the colors blue?
 * Check if periodicity check is being called
 * Threading
 */

#define WIDTH  1000
#define HEIGHT 1000

struct WidgetHolder {
    uint32_t *pixels;
    GtkWidget *drawing_area;

    float x, y;
};

static void init_pixels(uint32_t *pixels)
{
    for (int i = 0; i < WIDTH * HEIGHT; i++)
        pixels[i] = 0xFF000000;
}

static void set_pixel(uint32_t *pixels, int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) {
        return;
    }

    pixels[y * WIDTH + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
}

static void on_draw(__attribute__((unused)) GtkDrawingArea *area,
                    cairo_t *cr,
                    __attribute__((unused)) int width,
                    __attribute__((unused)) int height,
                    gpointer data)
{
    cairo_surface_t *surface = cairo_image_surface_create_for_data(
        (unsigned char *)data,
        CAIRO_FORMAT_ARGB32,
        WIDTH, HEIGHT,
        WIDTH * 4
    );

    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    cairo_surface_destroy(surface);
}

/* I don't know how good this coloring function is yet. */
int colors(int i)
{
    int ret = 0xFF000000;
    int r = (((i % 2) + 1)<<5) - 1;
    int g = (((i % 4) + 1)<<5) - 1;
    int b = (((i % 8) + 1)<<5) - 1;

    return (ret | r<<15 | g<<7 | b);
}

int escape(double complex z_0, int iterations)
{
    /* Counters: */
    int i = 0;
    double complex n = z_0;
    double x, y;

    /* Wikipedia.org: Plotting algorithms for the Mandelbrot set
     * Under the heading "Periodicity checking" -- no author cited */
    double x_old = 0, y_old = 0;
    int period = 0;
    int period_steps = 10;
    double period_tolerance = 0.01;

    /* Return the number of iterations to signal
     * that the number zero did NOT escape */
    if (z_0 == 0) {
        return iterations;
    }

    do {
        x = creal(n);
        y = cimag(n);

        n = (n*n + z_0);

        if (fabs(x_old - x) < period_tolerance &&
            fabs(y_old - y) < period_tolerance)
        {
            return iterations;
        }

        if (period > period_steps) {
            period = 0;
            x_old = x;
            y_old = y;
        }

        ++period;
    } while ((x*x + y*y <= 4) && (++i < iterations));

    return i;
}

void render_frame(uint32_t *pixels, double x_min, double y_min, double x_max, double y_max)
{
    int width = WIDTH, height = HEIGHT;
    double x, y;
    int x_pix, y_pix;
    /* double x_min = -2, y_min = -2;
    double x_max = 2,  y_max = 2; */
    double dx = fabs(x_max - x_min) / width;
    double dy = fabs(y_max - y_min) / height;

    printf("(%f, %f)\n", dx, dy);

    int escape_success;
    int steps;
    int c;
    
    double complex j;
    const int iter = 1000;

    for (y = y_min, y_pix = 0; y < y_max; y += dy, ++y_pix) {
        for (x = x_min, x_pix = 0; x < x_max; x += dx, ++x_pix) {
            j = (x + y*I);

            steps = escape(j, iter);
            c = colors(steps);
            escape_success = (steps != iter);

            if (escape_success) {
                set_pixel(pixels, x_pix, y_pix, (c & 0x00FF0000)>>15, (c & 0x0000FF00)>>7, c & 0x000000FF);
            } else {
                set_pixel(pixels, x_pix, y_pix, 0, 0, 0);
            }
        }
    }
}

inline float new_limits(float old_limit)
{
    return (old_limit - 500) / 250;
}

static gboolean zoom_loop(gpointer data)
{
    struct WidgetHolder *z = (struct WidgetHolder *) data;
    static int delta = 450;

    render_frame(z->pixels,
                new_limits(z->x - delta), new_limits(z->y - delta),
                new_limits(z->x + delta), new_limits(z->y + delta));
    gtk_widget_queue_draw((GtkWidget *) z->drawing_area);

    delta -= 10;
    if (delta <= 0) {
        g_print("Zoom done\n");
        return 0;
    }

    return 1;
}

static void on_click(__attribute__((unused)) GtkGestureClick *gesture,
        __attribute__((unused)) int n_press,
        double x, double y,
        gpointer user_data)
{
    struct WidgetHolder *zoom_holder = (struct WidgetHolder *) user_data;
    zoom_holder->x = x;
    zoom_holder->y = y;

    g_timeout_add(100, zoom_loop, zoom_holder);
}

static void activate
(
    GtkApplication *app,
    __attribute__((unused)) gpointer user_data
)
{
    static uint32_t pixels[WIDTH * HEIGHT];
    static GtkWidget *drawing_area = NULL;

    struct WidgetHolder *cb_data = g_new0(struct WidgetHolder, 1);
    cb_data->pixels = pixels;
    /* g_free(cb_data); */

    init_pixels(pixels);

    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Mandelbrot");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);

    drawing_area = gtk_drawing_area_new();
    cb_data->drawing_area = drawing_area;
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(drawing_area), WIDTH);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(drawing_area), HEIGHT);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area), on_draw, pixels, NULL);

    /* Add click handler */
    GtkGesture *click = gtk_gesture_click_new();
    g_signal_connect(click, "pressed", G_CALLBACK(on_click), cb_data);
    gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(click));

    gtk_window_set_child(GTK_WINDOW(window), drawing_area);

    /* gtk_window_fullscreen(GTK_WINDOW(window)); */
    render_frame(pixels, -2, -2, 2, 2);
    gtk_widget_show(window);
}

int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.birdideas.mandelbrot", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);
    return status;
}

