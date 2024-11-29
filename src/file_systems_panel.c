#include "file_systems_panel.h"

#include <gtk/gtk.h>
#include <sys/statvfs.h>

#include "misc.h"

GtkWidget* get_file_systems_panel() {
    //GtkWidget *tab = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    //gtk_widget_set_size_request(tab, 800, 600);
    FILE* fp = fopen(MOUNTS_FILE_PATH, "r");
    if (fp == NULL)  {
        return gtk_label_new("<MOUNTS PATH MISSING>");
    }
    //device, directory, type, total, free, available, used
    GtkListStore *store = gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    char* device_buffer = malloc(MAX_MOUNTS_SIZE);
    char* dir_buffer = malloc(MAX_MOUNTS_SIZE);
    char* type_buffer = malloc(MAX_MOUNTS_SIZE);
    while (fscanf(fp, "%511s %511s %511s %*s %*d %*d\n", device_buffer, dir_buffer, type_buffer) == 3) {
        add_to_file_system_tree(store, device_buffer, dir_buffer, type_buffer);
    }
    free(device_buffer);
    free(dir_buffer);
    free(type_buffer);

    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    GtkCellRenderer *renderer = NULL;
    GtkTreeViewColumn *column = NULL;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Device", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Directory", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Total", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Free", renderer, "text", 4, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Available", renderer, "text", 5, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Used", renderer, "text", 6, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scroll), tree);
    
    //gtk_container_add(GTK_CONTAINER(tab), scroll);
    return scroll;
}

void add_to_file_system_tree(GtkListStore *store, char* device, char* dir, char* type) {
    struct statvfs fs_info = {0};
    if (statvfs(dir, &fs_info) != 0) {
        printf("Filesystem error for %s, using dir: %s\n", device, dir);
        return;
    }
    char* total = malloc(MAX_FILE_SYSTEM_INFO_LEN);
    char* available = malloc(MAX_FILE_SYSTEM_INFO_LEN);
    char* free = malloc(MAX_FILE_SYSTEM_INFO_LEN);
    char* used = malloc(MAX_FILE_SYSTEM_INFO_LEN);
    sprintf(total, "%ld", fs_info.f_bsize * fs_info.f_blocks);
    sprintf(available, "%ld", fs_info.f_bsize * fs_info.f_bavail);
    sprintf(free, "%ld", fs_info.f_bsize * fs_info.f_bfree);
    sprintf(used, "%ld", fs_info.f_bsize * (fs_info.f_blocks - fs_info.f_bfree));
    GtkTreeIter iter = {0};
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, device,
                                     1, dir,
                                     2, type,
                                     3, total,
                                     4, free,
                                     5, available,
                                     6, used,
                                     -1);
}


