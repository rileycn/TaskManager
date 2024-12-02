#ifndef RESOURCES_PANEL_H
#define RESOURCES_PANEL_H

#include <gtk/gtk.h>

GtkWidget* get_resources_panel();
GtkWidget* create_main_window();
gboolean update_graphs(GtkWidget* user_data);

#endif