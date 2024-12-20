#include <gtk/gtk.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <sys/sysinfo.h>

#include "processes_panel.h"

static gboolean show_user_only = FALSE;

void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
  GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
  GtkTreeIter iter;
  if (gtk_tree_model_get_iter(model, &iter, path)) {
    guint pid;
    gtk_tree_model_get(model, &iter, 2, &pid, -1);
    show_process_details(pid);
  }
}

void show_process_details(pid_t pid) {
  GtkWidget *dialog, *content_area, *label;
  char buffer[1024];
  FILE *file;
  char path[256], process_name[256], user[256], state[16], date_started[64];
  long unsigned int vsize, rss, shared;
  long unsigned int utime, stime, start_time;
  unsigned int uid;
  double cpu_time_seconds;
  time_t process_start_epoch;
  dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(dialog), "Process Details");
  gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);
  content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_add(GTK_CONTAINER(dialog), content_area);
  snprintf(path, sizeof(path), "/proc/%d/comm", pid);
  file = fopen(path, "r");
  if (file) {
    fgets(process_name, sizeof(process_name), file);
    fclose(file);
  }
  else {
    snprintf(process_name, sizeof(process_name), "Unknown");
  }
  snprintf(path, sizeof(path), "/proc/%d/status", pid);
  file = fopen(path, "r");
  if (file) {
    while (fgets(buffer, sizeof(buffer), file)) {
      if (sscanf(buffer, "Uid:\t%u", &uid) == 1) {
        struct passwd *pw = getpwuid(uid);
        if (pw) {
          snprintf(user, sizeof(user), "%s", pw->pw_name);
        } else {
          snprintf(user, sizeof(user), "Unknown");
        }
      }
      sscanf(buffer, "State:\t%15s", state);
    }
    fclose(file);
  }
  else {
    snprintf(user, sizeof(user), "Unknown");
    snprintf(state, sizeof(state), "Unknown");
  }
  snprintf(path, sizeof(path), "/proc/%d/stat", pid);
  file = fopen(path, "r");
  if (file) {
    fscanf(file,
        "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %lu",
        &utime, &stime, &start_time);
    fclose(file);
  }
  snprintf(path, sizeof(path), "/proc/%d/statm", pid);
  file = fopen(path, "r");
  if (file) {
    fscanf(file, "%lu %lu %lu", &vsize, &rss, &shared);
    fclose(file);
  }
  long ticks_per_second = sysconf(_SC_CLK_TCK);
  cpu_time_seconds = (utime + stime) / (double)ticks_per_second;
  file = fopen("/proc/uptime", "r");
  double system_uptime;
  if (file) {
    fscanf(file, "%lf", &system_uptime);
    fclose(file);
  }
  else {
    system_uptime = 0.0;
  }
  double process_start_seconds = start_time / (double)ticks_per_second;
  process_start_epoch = time(NULL) - (time_t)system_uptime + (time_t)process_start_seconds;
  strftime(date_started, sizeof(date_started), "%Y-%m-%d %H:%M:%S", localtime(&process_start_epoch));
  snprintf(buffer, sizeof(buffer), "Process Name: %s", process_name);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  snprintf(buffer, sizeof(buffer), "User: %s", user);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  snprintf(buffer, sizeof(buffer), "State: %s", state);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  snprintf(buffer, sizeof(buffer), "Memory (Virtual): %lu KB", vsize * 4);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  snprintf(buffer, sizeof(buffer), "Memory (Resident): %lu KB", rss * 4);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  snprintf(buffer, sizeof(buffer), "Memory (Shared): %lu KB", shared * 4);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  snprintf(buffer, sizeof(buffer), "CPU Time: %.2f seconds", cpu_time_seconds);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  snprintf(buffer, sizeof(buffer), "Start Time: %s", date_started);
  label = gtk_label_new(buffer);
  gtk_box_pack_start(GTK_BOX(content_area), label, FALSE, FALSE, 0);
  gtk_widget_show_all(dialog);
}

void stop_process(pid_t pid) {
  if (kill(pid, SIGSTOP)) {
    perror("Coudn't stop process");
  }
}

void continue_process(pid_t pid) {
  if (kill(pid, SIGCONT)) {
    perror("Couldn't continue process");
  }
}

void kill_process(pid_t pid) {
  if (kill(pid, SIGKILL)) {
    perror("Couldn't kill process");
  }
}

void list_mem_maps(pid_t pid) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/smaps", pid);
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("Failed to open smaps");
        return;
    }
    GtkWidget *dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "Memory Maps");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 800, 400);
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(dialog), scrolled_window);
    GtkListStore *list_store = gtk_list_store_new(10,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING
    );
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list_store));
    const char *column_titles[] = {
        "Filename", "VM Start", "VM End", "VM Size", "Flags",
        "VM Offset", "Private Clean", "Private Dirty",
        "Shared Clean", "Shared Dirty"
    };
    for (int i = 0; i < 10; i++) {
        GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
            column_titles[i], renderer, "text", i, NULL);
        gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
    }
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    char line[512];
    char vm_start[64], vm_end[64], flags[8], vm_offset[64], filename[256];
    unsigned long vm_size, private_clean, private_dirty, shared_clean, shared_dirty;
    vm_start[0] = vm_end[0] = flags[0] = vm_offset[0] = filename[0] = '\0';
    vm_size = private_clean = private_dirty = shared_clean = shared_dirty = 0;
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%64[^-]-%64s %8s %64s %*s %*s %255[^\n]",
                   vm_start, vm_end, flags, vm_offset, filename) >= 4) {
            if (strlen(filename) == 0) {
                strcpy(filename, "[anonymous]");
            }
            if (strlen(vm_start) > 0) {
                GtkTreeIter iter;
                gtk_list_store_append(list_store, &iter);
                gtk_list_store_set(list_store, &iter,
                                   0, filename,
                                   1, vm_start,
                                   2, vm_end,
                                   3, g_strdup_printf("%lu kB", vm_size),
                                   4, flags,
                                   5, vm_offset,
                                   6, g_strdup_printf("%lu kB", private_clean),
                                   7, g_strdup_printf("%lu kB", private_dirty),
                                   8, g_strdup_printf("%lu kB", shared_clean),
                                   9, g_strdup_printf("%lu kB", shared_dirty),
                                   -1);

            }
            vm_size = private_clean = private_dirty = shared_clean = shared_dirty = 0;
        }
        sscanf(line, "Size: %lu kB", &vm_size);
        sscanf(line, "Private_Clean: %lu kB", &private_clean);
        sscanf(line, "Private_Dirty: %lu kB", &private_dirty);
        sscanf(line, "Shared_Clean: %lu kB", &shared_clean);
        sscanf(line, "Shared_Dirty: %lu kB", &shared_dirty);
    }
    if (strlen(vm_start) > 0) {
        GtkTreeIter iter;
        gtk_list_store_append(list_store, &iter);
        gtk_list_store_set(list_store, &iter,
                           0, filename,
                           1, vm_start,
                           2, vm_end,
                           3, g_strdup_printf("%lu kB", vm_size),
                           4, flags,
                           5, vm_offset,
                           6, g_strdup_printf("%lu kB", private_clean),
                           7, g_strdup_printf("%lu kB", private_dirty),
                           8, g_strdup_printf("%lu kB", shared_clean),
                           9, g_strdup_printf("%lu kB", shared_dirty),
                           -1);
    }
    fclose(file);
    gtk_widget_show_all(dialog);
}

int parse_process(int pid, process_info *info) {
  char path[256];
  sprintf(path, "/proc/%d/stat", pid);
  FILE *fp = fopen(path, "r");
  if (fp) {
    fscanf(fp, "%d %s %*c %d", &info->pid, info->name, &info->ppid);
    fclose(fp);
    int info_len = strlen(info->name);
    memmove(info->name, info->name + 1, info_len - 2);
    info->name[info_len - 2] = '\0';
    return 1;
  }
  return 0;
}

void list_open_files(pid_t pid) {
  GtkWidget *dialog, *tree_view, *store;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeIter iter;
  char fd_path[256];
  char target[1024];
  ssize_t len;
  dialog = gtk_dialog_new_with_buttons("Open Files",
      NULL,
      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
      "Close",
      GTK_RESPONSE_CLOSE,
      NULL);
  gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 400);
  store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd", pid);
  DIR *dir = opendir(fd_path);
  if (dir) {
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if (entry->d_type == DT_LNK) {
        char type[16] = "unknown";
        snprintf(fd_path, sizeof(fd_path), "/proc/%d/fd/%s", pid, entry->d_name);
        len = readlink(fd_path, target, sizeof(target) - 1);
        if (len != -1) {
          target[len] = '\0';
          if (strstr(target, "socket:")) {
            strcpy(type, "local socket");
          }
          else if (strstr(target, "/dev")) {
            strcpy(type, "device");
          }
          else {
            strcpy(type, "file");
          }
          gtk_list_store_append(store, &iter);
          gtk_list_store_set(store, &iter,
              0, entry->d_name,
              1, type,
              2, target,
              -1);
        }
      }
    }
    closedir(dir);
  }
  tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
  g_object_unref(store);
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("FD", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  column = gtk_tree_view_column_new_with_attributes("Object", renderer, "text", 2, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))),
      tree_view, TRUE, TRUE, 5);
  gtk_widget_show_all(dialog);
  g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

void show_context_menu(GtkWidget *widget, GdkEventButton *event, pid_t pid) {
  GtkWidget *menu = gtk_menu_new();
  GtkWidget *stop_button = gtk_menu_item_new_with_label("Stop");
  g_signal_connect_swapped(stop_button, "activate", G_CALLBACK(stop_process), GINT_TO_POINTER(pid));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), stop_button);
  GtkWidget *continue_button = gtk_menu_item_new_with_label("Continue");
  g_signal_connect_swapped(continue_button, "activate", G_CALLBACK(continue_process), GINT_TO_POINTER(pid));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), continue_button);
  GtkWidget *kill_button = gtk_menu_item_new_with_label("Kill");
  g_signal_connect_swapped(kill_button, "activate", G_CALLBACK(kill_process), GINT_TO_POINTER(pid));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), kill_button);
  GtkWidget *maps_button = gtk_menu_item_new_with_label("Memory Maps");
  g_signal_connect_swapped(maps_button, "activate", G_CALLBACK(list_mem_maps), GINT_TO_POINTER(pid));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), maps_button);
  GtkWidget *files_button = gtk_menu_item_new_with_label("Open Files");
  g_signal_connect_swapped(files_button, "activate", G_CALLBACK(list_open_files), GINT_TO_POINTER(pid));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), files_button);
  gtk_widget_show_all(menu);
  gtk_menu_popup_at_pointer(GTK_MENU(menu), (GdkEvent *) event);
}

gboolean on_tree_view_button_press(GtkWidget *tree_view, GdkEventButton *event, gpointer user_data) {
  if (event->type == GDK_BUTTON_PRESS && event->button == 3) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    GtkTreeModel *model;
    GtkTreeIter iter;
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
      guint pid;
      gtk_tree_model_get(model, &iter, 2, &pid, -1);
      show_context_menu(tree_view, event, pid);
      return TRUE;
    }
  }
  return FALSE;
}

void build_tree(GtkTreeStore *store, GtkTreeIter *parent, int ppid, gboolean filter_user) {
  DIR *proc_dir = opendir("/proc");
  if (!proc_dir) {
    perror("opendir");
    return;
  }
  uid_t current_user_uid = getuid();
  struct dirent *entry;
  GtkTreeIter iter;
  while ((entry = readdir(proc_dir)) != NULL) {
    if (entry->d_type == DT_DIR && g_ascii_isdigit(entry->d_name[0]) || filter_user) {
      int pid = atoi(entry->d_name);
      process_info pinfo = {};
      if ((parse_process(pid, &pinfo) && pinfo.ppid == ppid) || filter_user) {      
        char stat_path[256], comm_path[256], status_path[256];
        snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);
        snprintf(comm_path, sizeof(comm_path), "/proc/%s/comm", entry->d_name);
        snprintf(status_path, sizeof(status_path), "/proc/%s/status", entry->d_name);
        FILE *stat_file = fopen(stat_path, "r");
        FILE *comm_file = fopen(comm_path, "r");
        FILE *status_file = fopen(status_path, "r");
        if (stat_file && comm_file && status_file) {
          char name[256];
          long pid;
          char state;
          long rss;
          uid_t uid;
          if (fgets(name, sizeof(name), comm_file)) {
            name[strcspn(name, "\n")] = '\0';
          }
          char line[256];
          while (fgets(line, sizeof(line), status_file)) {
            if (strncmp(line, "Uid:", 4) == 0) {
              sscanf(line, "Uid:\t%u", &uid);
              break;
            }
          }
          if (filter_user && uid != current_user_uid) {
            fclose(stat_file);
            fclose(comm_file);
            fclose(status_file);
            continue;
          }
          fscanf(stat_file, "%ld %*s %c %*d %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %ld",
              &pid, &state, &rss);
          fclose(stat_file);
          fclose(comm_file);
          fclose(status_file);
          float memory = rss * 4 / 1024.0;
          gtk_tree_store_append(store, &iter, parent);
          gtk_tree_store_set(store, &iter,
              0, name,
              1, state == 'R' ? "Running" : "Sleeping",
              2, pid,
              3, memory,
              -1);
        }
        if (!filter_user) {
          build_tree(store, &iter, pid, filter_user);
        }
      }
    }
  }
  closedir(proc_dir);
}

static void populate_process_list(GtkTreeStore *store, gboolean filter_user) {
  gtk_tree_store_clear(store);
  build_tree(store, NULL, 0, filter_user);
}

void on_refresh_button_clicked(GtkWidget* button, gpointer data) {
  GtkTreeStore *store = GTK_TREE_STORE(data);
  populate_process_list(store, show_user_only);
}

void on_toggle_button_clicked(GtkWidget *button, gpointer data) {
  GtkTreeStore *store = GTK_TREE_STORE(data);
  show_user_only = !show_user_only;
  gtk_button_set_label(GTK_BUTTON(button), show_user_only ? "Show All Processes" : "Show My Processes");
  populate_process_list(store, show_user_only);
}

GtkWidget *get_processes_panel() {
  GtkTreeStore *store = gtk_tree_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_FLOAT);
  populate_process_list(store, show_user_only);
  GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Process Name", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Status", renderer, "text", 1, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("PID", renderer, "text", 2, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("Memory (MB)", renderer, "text", 3, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);
  g_signal_connect(tree_view, "button-press-event", G_CALLBACK(on_tree_view_button_press), NULL);
  g_signal_connect(tree_view, "row-activated", G_CALLBACK(on_row_activated), NULL);
  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *refresh_button = gtk_button_new_with_label("Refresh");
  g_signal_connect(refresh_button, "clicked", G_CALLBACK(on_refresh_button_clicked), store);
  GtkWidget *toggle_button = gtk_button_new_with_label("Show My Processes");
  g_signal_connect(toggle_button, "clicked", G_CALLBACK(on_toggle_button_clicked), store);
  gtk_box_pack_start(GTK_BOX(vbox), refresh_button, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), toggle_button, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);
  return vbox;
}
