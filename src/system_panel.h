#ifndef SYSTEM_PANEL_H
#define SYSTEM_PANEL_H

#include <gtk/gtk.h>

#define SYSTEM_NAME_PATH ("/proc/sys/kernel/hostname")
#define OS_RELEASE_PATH ("/proc/version")
#define KERNEL_VERSION_PATH ("/proc/sys/kernel/osrelease")
#define MEMORY_PATH ("/proc/meminfo")

#define MAX_WIDTH_CHARS (10)
#define MAX_MEM_INFO_LABEL (32)

GtkWidget* get_system_panel();

GtkWidget* get_system_name_label();
GtkWidget* get_os_release_label();
GtkWidget* get_kernel_version_label();

#endif
