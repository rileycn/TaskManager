#ifndef RESOURCES_GRAPH_H
#define RESOURCES_GRAPH_H

#include <gtk/gtk.h>


GtkWidget* create_combined_graph_tab();

void update_combined_graph(GtkWidget *drawing_area);
void init_cpu_monitoring();
gboolean draw_combined_graph(GtkWidget *widget, cairo_t *cr, gpointer data);
void draw_network_graph(GtkWidget *widget, cairo_t *cr, double width, double height);
void draw_memory_graph(GtkWidget *widget, cairo_t *cr, double width, double height);
void draw_cpu_graph(GtkWidget *widget, cairo_t *cr, double width, double height);

#endif
