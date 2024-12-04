#ifndef FILE_SYSTEMS_PANEL_H
#define FILE_SYSTEMS_PANEL_H

#include <gtk/gtk.h>

#define MAX_FILE_SYSTEM_INFO_LEN (32)
#define MAX_MOUNTS_SIZE (512)
#define MOUNTS_FILE_PATH ("/proc/mounts")

GtkWidget* get_file_systems_panel();
void add_to_file_system_tree(GtkListStore *store, char* device, char* dir, char* type);

#endif