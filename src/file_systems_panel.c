#include "file_systems_panel.h"

#include <gtk/gtk.h>

#include "misc.h"

GtkWidget* get_file_systems_panel() {
    return gtk_label_new("(File Systems Content)");
}