#include "file_systems_panel.h"

#include <gtk/gtk.h>
#include <sys/statvfs.h>

#include "misc.h"

gboolean filter_func(GtkTreeModel *model, GtkTreeIter *iter, gpointer data) {
    const gchar *search_text = gtk_entry_get_text(GTK_ENTRY(((search_data *)data)->search_entry));
    gchar* row_text;
    gtk_tree_model_get(model, iter, 0, &row_text, -1);
    gboolean visible = g_strstr_len(row_text, -1, search_text) != NULL;
    g_free(row_text);
    return visible;
}

void on_search_changed(GtkSearchEntry *entry, GtkTreeModelFilter *filter) {
    gtk_tree_model_filter_refilter(filter);
}

GtkWidget* get_file_systems_panel() {
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget* search_bar = gtk_search_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), search_bar, FALSE, FALSE, 0);
    FILE* fp = fopen(MOUNTS_FILE_PATH, "r");
    if (fp == NULL)  {
        return gtk_label_new("<MOUNTS PATH MISSING>");
    }
    GtkListStore *store = gtk_list_store_new(7, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeModelFilter* filter = GTK_TREE_MODEL_FILTER(gtk_tree_model_filter_new(GTK_TREE_MODEL(store), NULL));
    search_data* sd = malloc(sizeof(search_data));
    sd->list_store = GTK_TREE_MODEL(store);
    sd->search_entry = GTK_SEARCH_ENTRY(search_bar);
    char* device_buffer = malloc(MAX_MOUNTS_SIZE);
    char* dir_buffer = malloc(MAX_MOUNTS_SIZE);
    char* type_buffer = malloc(MAX_MOUNTS_SIZE);
    while (fscanf(fp, "%511s %511s %511s %*s %*d %*d\n", device_buffer, dir_buffer, type_buffer) == 3) {
        add_to_file_system_tree(store, device_buffer, dir_buffer, type_buffer);
    }
    free(device_buffer);
    free(dir_buffer);
    free(type_buffer);
    GtkWidget *tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(filter));
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
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);
    gtk_tree_model_filter_set_visible_func(filter, (GtkTreeModelFilterVisibleFunc) filter_func, sd, NULL);
    g_signal_connect(search_bar, "search-changed", G_CALLBACK(on_search_changed), filter);
    fclose(fp);
    return vbox;
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


