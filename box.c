#include <gtk/gtk.h>

#include "box.h"

GtkWidget* get_main_box() {
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);

    GtkWidget *notebook = gtk_notebook_new();
    gtk_widget_set_hexpand(notebook, TRUE);
    gtk_widget_set_vexpand(notebook, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), get_system_panel(), gtk_label_new("System"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), get_processes_panel(), gtk_label_new("Processes"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), get_resources_panel(), gtk_label_new("Resources"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), get_file_systems_panel(), gtk_label_new("File Systems"));

    gtk_box_append(GTK_BOX(box), notebook);
    

    return box;
}

GtkWidget* get_system_panel() {
    GtkWidget *tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *title_label = gtk_label_new("System Content Page");
    gtk_widget_set_margin_top(title_label, 20);
    gtk_widget_set_margin_bottom(title_label, 20);
    gtk_box_append(GTK_BOX(tab), title_label);
    gtk_box_append(GTK_BOX(tab),  gtk_label_new("System info"));
    return tab;
}

GtkWidget* get_processes_panel() {
    GtkWidget *tab_content = gtk_label_new("(Processes Content Page)");
    return tab_content;
}

GtkWidget* get_resources_panel() {
    GtkWidget *tab_content = gtk_label_new("(Resources Content Page)");
    return tab_content;
}

GtkWidget* get_file_systems_panel() {
    GtkWidget *tab_content = gtk_label_new("(File Systems Content Page)");
    return tab_content;
}