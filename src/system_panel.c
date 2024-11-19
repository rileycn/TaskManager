#include "system_panel.h"

#include <gtk/gtk.h>

#include "misc.h"

GtkWidget* get_system_panel() {
    //Should include, at a minimum, the OS release version, kernel version, amount of memory, processor version, and disk storage.
    GtkWidget *tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(tab, 800, 600);
    gtk_container_add(GTK_CONTAINER(tab), get_system_name_label());
    gtk_container_add(GTK_CONTAINER(tab), get_os_release_label());
    gtk_container_add(GTK_CONTAINER(tab), get_kernel_version_label());
    return tab;
}

GtkWidget* get_system_name_label() {
    char* system_name = read_file_into_string(SYSTEM_NAME_PATH);
    if (system_name == NULL) {
        system_name = const_to_malloc("<ERROR>");
    }
    GtkWidget *label = gtk_label_new(NULL);
    const char *format = "<span style=\"oblique\" size=\"x-large\">\%s</span>";
    char *markup = g_markup_printf_escaped(format, system_name);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_widget_set_margin_top(label, 20);
    gtk_widget_set_margin_bottom(label, 20);
    g_free(markup);
    free(system_name);
    return label;
}

GtkWidget* get_os_release_label() {
    char* os_release = read_file_into_string(OS_RELEASE_PATH);
    if (os_release == NULL) {
        os_release = const_to_malloc("<ERROR>");
    }
    GtkWidget *label = gtk_label_new(NULL);
    const char *format = "<span size=\"small\">\%s</span>";
    char* print_string = paste_strings("OS Release: ", os_release);
    char *markup = g_markup_printf_escaped(format, print_string);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_set_margin_bottom(label, 10);
    g_free(markup);
    free(os_release);
    free(print_string);
    return label;
}

GtkWidget* get_kernel_version_label() {
    char* kernel_version = read_file_into_string(KERNEL_VERSION_PATH);
    if (kernel_version == NULL) {
        kernel_version = const_to_malloc("<ERROR>");
    }
    GtkWidget *label = gtk_label_new(NULL);
    const char *format = "<span size=\"small\">\%s</span>";
    char* print_string = paste_strings("Kernel Version: ", kernel_version);
    char *markup = g_markup_printf_escaped(format, print_string);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_set_margin_bottom(label, 10);
    g_free(markup);
    free(kernel_version);
    free(print_string);
    return label;
}

GtkWidget* get_memory_label() {
    FILE* fp = fopen(MEMORY_PATH, "r");
    char* memory_info = NULL;
    if (fp == NULL)  {
        memory_info = const_to_malloc("<ERROR>");
    }
    else {
        memory_info = const_to_malloc("<NOT IMPLEMENTED>");
    }
    GtkWidget *label = gtk_label_new(NULL);
    const char *format = "<span size=\"small\">\%s</span>";
    char* print_string = paste_strings("Memory: ", memory_info);
    char *markup = g_markup_printf_escaped(format, print_string);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_set_margin_bottom(label, 10);
    g_free(markup);
    free(memory_info);
    free(print_string);
    return label;
}