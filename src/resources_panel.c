#include "resources_panel.h"

#include <gtk/gtk.h>

#include "resources_graph.h"
#include "misc.h"

GtkWidget* get_resources_panel() {
    return create_main_window();
}

gboolean update_graphs(GtkWidget* user_data) {
    update_combined_graph(user_data);
    return TRUE; // Continue calling
}

GtkWidget* create_main_window() {
    // Create the combined graph
    GtkWidget *graph_area = create_combined_graph_tab();

    // Timer for periodic updates
    g_timeout_add(1000, update_graphs, graph_area);

    return graph_area;
}