#ifndef PROCESSES_PANEL_H
#define PROCESSES_PANEL_H

#include <gtk/gtk.h>

typedef struct {
  int pid;
  int ppid;
  char name[256];
} process_info;


GtkWidget* get_processes_panel();
void build_tree(GtkTreeStore *store, GtkTreeIter *parent, int ppid, gboolean filter_user);
int parse_process(int pid, process_info *info);
void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void show_process_details(pid_t pid);

#endif
