#include "box.h"

#include <gtk/gtk.h>

#include "system_panel.h"
#include "processes_panel.h"
#include "resources_panel.h"
#include "file_systems_panel.h"

GtkWidget* get_main_box() {
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    GtkWidget *notebook = gtk_notebook_new();
    gtk_widget_set_hexpand(notebook, TRUE);
    gtk_widget_set_vexpand(notebook, TRUE);
    gtk_notebook_append_page(GTK_CONTAINER(notebook), get_system_panel(), gtk_label_new("System"));
    gtk_notebook_append_page(GTK_CONTAINER(notebook), get_processes_panel(), gtk_label_new("Processes"));
    gtk_notebook_append_page(GTK_CONTAINER(notebook), get_resources_panel(), gtk_label_new("Resources"));
    gtk_notebook_append_page(GTK_CONTAINER(notebook), get_file_systems_panel(), gtk_label_new("File Systems"));
    gtk_container_add(GTK_CONTAINER(box), notebook);
    return box;
}

GtkWidget* get_placeholder_panel(char* placeholder) {
    GtkWidget *tab_content = gtk_label_new(placeholder);
    return tab_content;
}
