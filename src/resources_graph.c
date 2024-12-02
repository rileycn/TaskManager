#include "resources_graph.h"

#include <cairo.h>

#include "resources_data.h"

#define MAX_CPUS 256
#define HISTORY_SIZE 100

double cpu_usage_history[MAX_CPUS + 1][HISTORY_SIZE];  // +1 for total CPU
int history_index = 0;
int num_cpus = 0;

// Add these new arrays for memory/swap history
double memory_history[2][HISTORY_SIZE];  // [0] for memory, [1] for swap
const struct {
    double r, g, b;
} memory_colors[] = {
    {0.2, 0.7, 0.2},  // Memory - Green
    {0.7, 0.2, 0.2},  // Swap - Red
};

// Define some colors for different CPU lines
const struct {
    double r, g, b;
} cpu_colors[] = {
    {0.0, 0.6, 0.8},  // Total CPU - Blue
    {1.0, 0.0, 0.0},  // CPU0 - Red
    {0.0, 0.8, 0.0},  // CPU1 - Green
    {0.8, 0.0, 0.8},  // CPU2 - Purple
    {1.0, 0.6, 0.0},  // CPU3 - Orange
    {0.0, 0.6, 0.6},  // CPU4 - Teal
    {0.8, 0.4, 0.0},  // CPU5 - Brown
    {0.4, 0.4, 0.8},  // CPU6 - Indigo
    {0.8, 0.8, 0.0},  // CPU7 - Yellow
    {0.0, 0.4, 0.4},  // CPU8 - Dark Teal
    {1.0, 0.4, 0.4},  // CPU9 - Pink
    {0.4, 0.8, 0.4},  // CPU10 - Light Green
    {0.6, 0.4, 0.2},  // CPU11 - Light Brown
    {0.4, 0.4, 0.8},  // CPU12 - Light Purple
    {0.8, 0.2, 0.4},  // CPU13 - Dark Pink
    {0.2, 0.6, 0.4},  // CPU14 - Sea Green
};

// 
double network_history[2][HISTORY_SIZE]; 
const struct {
    double r, g, b;
} network_colors[] = {
    {0.0, 0.6, 0.8},  // Receiving - Blue
    {1.0, 0.0, 0.0},  // Sending - Red
};

void draw_cpu_graph(GtkWidget *widget, cairo_t *cr, double width, double height) {
    double graph_height = height / 3;
    
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);

    const double left_margin = 40;   
    const double bottom_margin = 20; 
    const double top_margin = 20;
    
  
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, width/2 - 40, top_margin/2);
    cairo_show_text(cr, "CPU History");
    
  
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);
    
 
    cairo_move_to(cr, left_margin, top_margin);
    cairo_line_to(cr, left_margin, graph_height - bottom_margin);
    
 
    cairo_move_to(cr, left_margin, graph_height - bottom_margin);
    cairo_line_to(cr, width - 10, graph_height - bottom_margin);
    cairo_stroke(cr);

    cairo_set_font_size(cr, 8);
    for (int i = 0; i <= 100; i += 20) {
        char label[10];
        snprintf(label, sizeof(label), "%d%%", i);
        cairo_move_to(cr, 5, graph_height - bottom_margin - (i * (graph_height - top_margin - bottom_margin) / 100.0));
        cairo_show_text(cr, label);
    }

    for (int i = 0; i <= 60; i += 10) {
        char label[10];
        snprintf(label, sizeof(label), "%ds", i);
        double x_pos = left_margin + ((60 - i) * (width - left_margin - 10) / 60.0);
        cairo_move_to(cr, x_pos - 10, graph_height - bottom_margin + 15);
        cairo_show_text(cr, label);
    }

    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.5);
    cairo_set_line_width(cr, 0.5);

    for (int i = 0; i <= 100; i += 20) {
        double y = graph_height - bottom_margin - (i * (graph_height - top_margin - bottom_margin) / 100.0);
        cairo_move_to(cr, left_margin, y);
        cairo_line_to(cr, width - 10, y);
    }
 
    for (int i = 0; i <= 60; i += 10) {
        double x = left_margin + ((60 - i) * (width - left_margin - 10) / 60.0);
        cairo_move_to(cr, x, top_margin);
        cairo_line_to(cr, x, graph_height - bottom_margin);
    }
    cairo_stroke(cr);
    

    for (int cpu = -1; cpu < num_cpus; cpu++) {
        int color_idx = cpu + 1;
        cairo_set_source_rgb(cr, 
            cpu_colors[color_idx % (sizeof(cpu_colors)/sizeof(cpu_colors[0]))].r,
            cpu_colors[color_idx % (sizeof(cpu_colors)/sizeof(cpu_colors[0]))].g,
            cpu_colors[color_idx % (sizeof(cpu_colors)/sizeof(cpu_colors[0]))].b);
        cairo_set_line_width(cr, 2);
        
        gboolean first = TRUE;
        for (int j = 59; j >= 0; j--) {
            int idx = (history_index - (59 - j) + HISTORY_SIZE) % HISTORY_SIZE;
            double x = left_margin + (j * (width - left_margin - 10) / 60.0);
            double y = graph_height - bottom_margin - 
                      (cpu_usage_history[cpu + 1][idx] * (graph_height - top_margin - bottom_margin) / 100.0);
            
            if (first) {
                cairo_move_to(cr, x, y);
                first = FALSE;
            } else {
                cairo_line_to(cr, x, y);
            }
        }
        cairo_stroke(cr);
    }

    cairo_set_font_size(cr, 10);
    double legend_y = top_margin;
    double legend_x_col1 = width - 170; 
    double legend_x_col2 = width - 80; 
    int items_per_column = (num_cpus + 2) / 2; 

    double legend_width = 170;
    double legend_height = (items_per_column * 15) + 10; 
    double legend_x = width - legend_width - 5;  

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.8);
    cairo_rectangle(cr, legend_x, legend_y - 10, legend_width, legend_height);
    cairo_fill(cr);

    for (int cpu = -1; cpu < num_cpus; cpu++) {
        int color_idx = cpu + 1;
        cairo_set_source_rgb(cr, 
            cpu_colors[color_idx % (sizeof(cpu_colors)/sizeof(cpu_colors[0]))].r,
            cpu_colors[color_idx % (sizeof(cpu_colors)/sizeof(cpu_colors[0]))].g,
            cpu_colors[color_idx % (sizeof(cpu_colors)/sizeof(cpu_colors[0]))].b);
        
        char label[30];  // Increased buffer size to accommodate percentage
        if (cpu == -1) {
            snprintf(label, sizeof(label), "Total CPU (%.1f%%)", cpu_usage_history[0][history_index > 0 ? history_index - 1 : HISTORY_SIZE - 1]);
        } else {
            snprintf(label, sizeof(label), "CPU%d (%.1f%%)", cpu, 
                    cpu_usage_history[cpu + 1][history_index > 0 ? history_index - 1 : HISTORY_SIZE - 1]);
        }

        double x_pos = (cpu + 1 < items_per_column) ? legend_x_col1 : legend_x_col2;
        double y_pos = legend_y + ((cpu + 1) % items_per_column) * 15;

        cairo_move_to(cr, x_pos, y_pos);
        cairo_show_text(cr, label);
    }
}

void draw_memory_graph(GtkWidget *widget, cairo_t *cr, double width, double height) {
    double graph_height = height / 3;
    double graph_y_offset = height / 3;
    
    const double left_margin = 40;   
    const double bottom_margin = 20; 
    const double top_margin = 20;
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, width/2 - 60, graph_y_offset + top_margin/2);
    cairo_show_text(cr, "Memory and Swap History");
    
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr, left_margin, graph_y_offset + top_margin);
    cairo_line_to(cr, left_margin, graph_y_offset + graph_height - bottom_margin);
    cairo_move_to(cr, left_margin, graph_y_offset + graph_height - bottom_margin);
    cairo_line_to(cr, width - 10, graph_y_offset + graph_height - bottom_margin);
    cairo_stroke(cr);

    cairo_set_font_size(cr, 8);
    for (int i = 0; i <= 100; i += 20) {
        char label[10];
        snprintf(label, sizeof(label), "%d%%", i);
        cairo_move_to(cr, 5, graph_y_offset + graph_height - bottom_margin - 
                     (i * (graph_height - top_margin - bottom_margin) / 100.0));
        cairo_show_text(cr, label);
    }

    for (int i = 0; i <= 60; i += 10) {
        char label[10];
        snprintf(label, sizeof(label), "%ds", i);
        double x_pos = left_margin + ((60 - i) * (width - left_margin - 10) / 60.0);
        cairo_move_to(cr, x_pos - 10, graph_y_offset + graph_height - bottom_margin + 15);
        cairo_show_text(cr, label);
    }

    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.5);
    cairo_set_line_width(cr, 0.5);

    for (int i = 0; i <= 100; i += 20) {
        double y = graph_y_offset + graph_height - bottom_margin - 
                  (i * (graph_height - top_margin - bottom_margin) / 100.0);
        cairo_move_to(cr, left_margin, y);
        cairo_line_to(cr, width - 10, y);
    }

    for (int i = 0; i <= 60; i += 10) {
        double x = left_margin + (i * (width - left_margin - 10) / 60.0);
        cairo_move_to(cr, x, graph_y_offset + top_margin);
        cairo_line_to(cr, x, graph_y_offset + graph_height - bottom_margin);
    }
    cairo_stroke(cr);
    for (int i = 0; i < 2; i++) {
        cairo_set_source_rgb(cr, memory_colors[i].r, memory_colors[i].g, memory_colors[i].b);
        cairo_set_line_width(cr, 2);
        
        gboolean first = TRUE;
        for (int j = 59; j >= 0; j--) {
            int idx = (history_index - (59 - j) + HISTORY_SIZE) % HISTORY_SIZE;
            double x = left_margin + (j * (width - left_margin - 10) / 60.0);
            double y = graph_y_offset + graph_height - bottom_margin - 
                      (memory_history[i][idx] * (graph_height - top_margin - bottom_margin) / 100.0);
            
            if (first) {
                cairo_move_to(cr, x, y);
                first = FALSE;
            } else {
                cairo_line_to(cr, x, y);
            }
        }
        cairo_stroke(cr);
    }

    cairo_set_font_size(cr, 10);
    double legend_y = graph_y_offset + top_margin;
    double legend_width = 170;
    double legend_height = 40;
    double legend_x = width - legend_width - 5;

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.8);
    cairo_rectangle(cr, legend_x, legend_y - 10, legend_width, legend_height);
    cairo_fill(cr);

    const char *labels[] = {"Memory", "Swap"};
    for (int i = 0; i < 2; i++) {
        cairo_set_source_rgb(cr, memory_colors[i].r, memory_colors[i].g, memory_colors[i].b);
        char label[40];
        snprintf(label, sizeof(label), "%s (%.1f%%)", labels[i], 
                memory_history[i][history_index > 0 ? history_index - 1 : HISTORY_SIZE - 1]);
        cairo_move_to(cr, legend_x + 10, legend_y + i * 15);
        cairo_show_text(cr, label);
    }
}

void draw_network_graph(GtkWidget *widget, cairo_t *cr, double width, double height) {
    double graph_height = height / 3;
    double graph_y_offset = height * 2 / 3; 
    
    const double left_margin = 40;   
    const double bottom_margin = 20; 
    const double top_margin = 20;
    
    // Find the maximum value in the network history
    double max_value = 20.0;  // Default maximum
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < HISTORY_SIZE; j++) {
            if (network_history[i][j] > max_value) {
                max_value = network_history[i][j];
            }
        }
    }
    
    max_value = (((int)(max_value + 3.99)) / 4) * 4;
    
  
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 12);
    cairo_move_to(cr, width/2 - 60, graph_y_offset + top_margin/2);
    cairo_show_text(cr, "Network History");
    
     cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, 1);
    cairo_move_to(cr, left_margin, graph_y_offset + top_margin);
    cairo_line_to(cr, left_margin, graph_y_offset + graph_height - bottom_margin);
    cairo_move_to(cr, left_margin, graph_y_offset + graph_height - bottom_margin);
    cairo_line_to(cr, width - 10, graph_y_offset + graph_height - bottom_margin);
    cairo_stroke(cr);

    cairo_set_font_size(cr, 8);
    for (int i = 0; i <= 5; i++) {
        char label[15];
        double value = (max_value * i) / 5.0;
        snprintf(label, sizeof(label), "%.1f KiB/s", value);
        cairo_move_to(cr, 5, graph_y_offset + graph_height - bottom_margin - 
                     (i * (graph_height - top_margin - bottom_margin) / 5.0));
        cairo_show_text(cr, label);
    }

   for (int i = 0; i <= 60; i += 10) {
        char label[10];
        snprintf(label, sizeof(label), "%ds", i);
        double x_pos = left_margin + ((60 - i) * (width - left_margin - 10) / 60.0);
        cairo_move_to(cr, x_pos - 10, graph_y_offset + graph_height - bottom_margin + 15);
        cairo_show_text(cr, label);
    }
    cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 0.5);
    cairo_set_line_width(cr, 0.5);


    for (int i = 0; i <= 20; i += 4) {
        double y = graph_y_offset + graph_height - bottom_margin - 
                  (i * (graph_height - top_margin - bottom_margin) / 20.0);
        cairo_move_to(cr, left_margin, y);
        cairo_line_to(cr, width - 10, y);
    }


    for (int i = 0; i <= 60; i += 10) {
        double x = left_margin + (i * (width - left_margin - 10) / 60.0);
        cairo_move_to(cr, x, graph_y_offset + top_margin);
        cairo_line_to(cr, x, graph_y_offset + graph_height - bottom_margin);
    }
    cairo_stroke(cr);

    for (int i = 0; i < 2; i++) {
        cairo_set_source_rgb(cr, network_colors[i].r, network_colors[i].g, network_colors[i].b);
        cairo_set_line_width(cr, 2);
        
        gboolean first = TRUE;
        for (int j = 59; j >= 0; j--) {
            int idx = (history_index - (59 - j) + HISTORY_SIZE) % HISTORY_SIZE;
            double x = left_margin + (j * (width - left_margin - 10) / 60.0);
            
            
            double scaled_value = network_history[i][idx];
            if (scaled_value > max_value) {
                scaled_value = max_value;
            }
            
            double y = graph_y_offset + graph_height - bottom_margin - 
                      (scaled_value * (graph_height - top_margin - bottom_margin) / max_value);
            
            if (first) {
                cairo_move_to(cr, x, y);
                first = FALSE;
            } else {
                cairo_line_to(cr, x, y);
            }
        }
        cairo_stroke(cr);
    }

    cairo_set_font_size(cr, 10);
    double legend_y = graph_y_offset + top_margin;
    double legend_width = 170;
    double legend_height = 40;
    double legend_x = width - legend_width - 5;

    cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.8);
    cairo_rectangle(cr, legend_x, legend_y - 10, legend_width, legend_height);
    cairo_fill(cr);

    const char *labels[] = {"Receiving", "Sending"};
    for (int i = 0; i < 2; i++) {
        cairo_set_source_rgb(cr, network_colors[i].r, network_colors[i].g, network_colors[i].b);
        char label[40];
        snprintf(label, sizeof(label), "%s (%.1f KiB/s)", labels[i], 
                network_history[i][history_index > 0 ? history_index - 1 : HISTORY_SIZE - 1]);
        cairo_move_to(cr, legend_x + 10, legend_y + i * 15);
        cairo_show_text(cr, label);
    }
}

gboolean draw_combined_graph(GtkWidget *widget, cairo_t *cr, gpointer data) {
    double width = gtk_widget_get_allocated_width(widget);
    double height = gtk_widget_get_allocated_height(widget);
    
    draw_cpu_graph(widget, cr, width, height);
    draw_memory_graph(widget, cr, width, height);
    draw_network_graph(widget, cr, width, height);
    
    return FALSE;
}

GtkWidget* create_combined_graph_tab() {
    GtkWidget *drawing_area = gtk_drawing_area_new();
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_combined_graph), NULL);
    return drawing_area;
}

void update_combined_graph(GtkWidget *drawing_area) {
    if (num_cpus == 0) {
        num_cpus = get_cpu_count();
    }
    
    // Update total CPU
    cpu_usage_history[0][history_index] = get_cpu_usage(-1) * 100.0;

    // Update individual CPUs
    for (int i = 0; i < num_cpus; i++) {
        cpu_usage_history[i + 1][history_index] = get_cpu_usage(i) * 100.0;
    }

    // Add memory and swap updates
    memory_history[0][history_index] = get_memory_usage() * 100.0;
    memory_history[1][history_index] = get_swap_usage() * 100.0;

    // Add network updates
    network_history[0][history_index] = get_network_speed(0);
    network_history[1][history_index] = get_network_speed(1);

    history_index = (history_index + 1) % HISTORY_SIZE;
    
    gtk_widget_queue_draw(drawing_area);
} 
