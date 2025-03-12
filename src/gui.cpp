#include "gui.h"
#include <gtk/gtk.h>

static void on_open_image(GtkWidget *widget, gpointer data) {
    
}

void create_main_window() {
    GtkWidget *window, *vbox, *open_button;

    window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "PicTag");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    open_button = gtk_button_new_with_label("Open Image");
    g_signal_connect(open_button, "clicked", G_CALLBACK(on_open_image), NULL);
    gtk_box_append(GTK_BOX(vbox), open_button);

    gtk_widget_show(window);
}
