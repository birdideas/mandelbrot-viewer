#include <gtk/gtk.h>
#include <cairo.h>
#include <stdint.h>

#define WIDTH  1920
#define HEIGHT 1080

// We'll keep an in-memory pixel buffer (ARGB32 format)
static uint32_t pixels[WIDTH * HEIGHT];

static gboolean on_draw
(
    __attribute__((unused)) GtkWidget *widget,
    cairo_t *cr,
    __attribute__((unused)) gpointer data
)
{

    // Create a Cairo image surface from our pixel buffer
    cairo_surface_t *surface = cairo_image_surface_create_for_data(
        (unsigned char *)pixels,
        CAIRO_FORMAT_ARGB32,
        WIDTH, HEIGHT,
        WIDTH * 4  // stride (bytes per row)
    );

    // Paint the surface onto the widget
    cairo_set_source_surface(cr, surface, 0, 0);
    cairo_paint(cr);

    cairo_surface_destroy(surface);
    return FALSE;
}

static void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;
    // ARGB format — alpha = 255
    uint32_t color = (0xFF << 24) | (r << 16) | (g << 8) | b;
    pixels[y * WIDTH + x] = color;
}

static gboolean on_button_press
(
    GtkWidget *widget,
    GdkEventButton *event,
    __attribute__((unused)) gpointer user_data
)
{
    int x = (int)event->x;
    int y = (int)event->y;

    // Draw a small yellow dot at the click location
    for (int dy = -3; dy <= 3; dy++) {
        for (int dx = -3; dx <= 3; dx++) {
            set_pixel(x + dx, y + dy, 255, 255, 0);
        }
    }

    // Queue a redraw so the new pixels appear
    gtk_widget_queue_draw(widget);

    return TRUE;  // event handled
}

static void init_pixels(void)
{
    // Fill background white
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        pixels[i] = 0xFF000000;
    }
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *drawing_area = gtk_drawing_area_new();

    gtk_widget_set_size_request(drawing_area, WIDTH, HEIGHT);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(on_button_press), NULL);

    gtk_widget_add_events(drawing_area, GDK_BUTTON_PRESS_MASK);
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    gtk_window_maximize(GTK_WINDOW(window));

    init_pixels();  // set pixel data before drawing

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

