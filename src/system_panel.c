#include "system_panel.h"

#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

#include "misc.h"

GtkWidget* get_system_panel() {
    GtkWidget *tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(tab), FALSE);
    gtk_widget_set_halign(tab, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(tab, 800, 600);
    gtk_container_add(GTK_CONTAINER(tab), get_system_name_label());
    gtk_container_add(GTK_CONTAINER(tab), get_os_release_label());
    gtk_container_add(GTK_CONTAINER(tab), get_kernel_version_label());
    gtk_container_add(GTK_CONTAINER(tab), get_memory_label());
    gtk_container_add(GTK_CONTAINER(tab), get_processor_label());
    gtk_container_add(GTK_CONTAINER(tab), get_disk_storage_label());
    gtk_box_pack_start(GTK_CONTAINER(tab), get_dark_mode_widget(), FALSE, FALSE, 0);
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
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_line_wrap_mode(GTK_LABEL(label), PANGO_WRAP_WORD);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_label_set_yalign(GTK_LABEL(label), 0.5);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    const char *format = "<span size=\"small\">\%s</span>";
    char* print_string = paste_strings("OS Release: ", os_release);
    char *markup = g_markup_printf_escaped(format, print_string);
    gtk_label_set_markup(GTK_LABEL(label), markup);
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
        char* mem_label = malloc(MAX_MEM_INFO_LABEL);
        unsigned long mem_value = 0;
        char* mem_unit = malloc(MAX_MEM_INFO_LABEL);
        bool success_find = false;
        while (fscanf(fp, "%31s %lu %31s\n", mem_label, &mem_value, mem_unit) == 3) {
            mem_label[strlen(mem_label) - 1] = '\0';
            if (strcmp(mem_label, "MemTotal") == 0) {
                memory_info = malloc(2 * MAX_MEM_INFO_LABEL);
                sprintf(memory_info, "%lu%s", mem_value, mem_unit);
                success_find = true;
                break;
            }
        }
        if (!success_find) {
            memory_info = const_to_malloc("<MEMTOTAL NOT FOUND>");
        }
        free(mem_label);
        free(mem_unit);
        fclose(fp);
    }
    GtkWidget *label = gtk_label_new(NULL);
    const char *format = "<span size=\"small\">\%s</span>";
    char* print_string = paste_strings("Total Memory: ", memory_info);
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

GtkWidget* get_processor_label() {
    FILE* fp = fopen(PROCESSOR_PATH, "r");
    char* processor_info = NULL;
    if (fp == NULL)  {
        processor_info = const_to_malloc("<ERROR>");
    }
    else {
        char* proc_label = malloc(MAX_PROC_INFO_LABEL);
        char* proc_value = malloc(MAX_PROC_INFO_LABEL);
        bool success_find = false;
        while (fscanf(fp, "%1023[^:]: %1023[^\n]\n", proc_label, proc_value) == 2) {
            char* comp_string = "model name";
            if (strlen(proc_label) > strlen(comp_string)) {
            	proc_label[strlen(comp_string)] = '\0';
            }
            if (strcmp(proc_label, "model name") == 0) {
                processor_info = const_to_malloc(proc_value);
                success_find = true;
                break;
            }
        }
        if (!success_find) {
            processor_info = const_to_malloc("<MODEL NAME NOT FOUND>");
        }
        free(proc_label);
        free(proc_value);
        fclose(fp);
    }
    GtkWidget *label = gtk_label_new(NULL);
    const char *format = "<span size=\"small\">\%s</span>";
    char* print_string = paste_strings("Processor: ", processor_info);
    char *markup = g_markup_printf_escaped(format, print_string);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_set_margin_bottom(label, 10);
    g_free(markup);
    free(processor_info);
    free(print_string);
    return label;
}

GtkWidget* get_disk_storage_label() {
    struct statvfs fs_info = {0};
    char* disk_info = NULL;
    if (statvfs(DISK_STORAGE_PATH, &fs_info) != 0) {
        disk_info = const_to_malloc("<STAT ERROR>");
    }
    else {
        long available_space = fs_info.f_bavail * fs_info.f_frsize;
        disk_info = malloc(MAX_MEM_INFO_LABEL);
        sprintf(disk_info, "%ld bytes", available_space);
    }
    GtkWidget *label = gtk_label_new(NULL);
    const char *format = "<span size=\"small\">\%s</span>";
    char* print_string = paste_strings("Available disk space: ", disk_info);
    char *markup = g_markup_printf_escaped(format, print_string);
    gtk_label_set_markup(GTK_LABEL(label), markup);
    gtk_label_set_xalign(GTK_LABEL(label), 0.5);
    gtk_widget_set_margin_top(label, 10);
    gtk_widget_set_margin_bottom(label, 10);
    g_free(markup);
    free(disk_info);
    free(print_string);
    return label;
}

void toggle_dark_mode(GtkToggleButton *toggle_button, gpointer user_data) {
    GtkSettings *settings = gtk_settings_get_default();
    gboolean active = gtk_toggle_button_get_active(toggle_button);
    g_object_set(settings, "gtk-application-prefer-dark-theme", active, NULL);
}

GtkWidget* get_dark_mode_widget() {
    GtkWidget *checkbox = gtk_check_button_new_with_label("Dark Mode");
    GtkSettings *settings = gtk_settings_get_default();
    gboolean prefer_dark_theme = FALSE;
    g_object_get(settings, "gtk-application-prefer-dark-theme", &prefer_dark_theme, NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), prefer_dark_theme);
    gtk_widget_set_halign(checkbox, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(checkbox, GTK_ALIGN_CENTER);
    g_signal_connect(checkbox, "toggled", G_CALLBACK(toggle_dark_mode), NULL);
    return checkbox;
}
