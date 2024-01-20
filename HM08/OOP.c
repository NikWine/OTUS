#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <errno.h>
#include <string.h>

#define COLUMN_TITLE 0
#define COLUMN_PATH 1

// Функция для заполнения модели дерева
void populate_model(GtkTreeStore *store, GtkTreeIter *iter, gchar *path) {
    GDir *dir;
    const gchar *name;
    GtkTreeIter child_iter;

    if (iter == NULL) {
        // Создание нового ряда в дереве
        gtk_tree_store_append(store, &child_iter, NULL);
    } else {
        // Добавление нового созданного ряда в качестве потомка 'iter'
        gtk_tree_store_append(store, &child_iter, iter);
    }

    // Добавление вновь созданного ряда в хранилище дерева
    gtk_tree_store_set(store, &child_iter,
                       COLUMN_TITLE, g_path_get_basename(path),
                       COLUMN_PATH, path, -1);

    dir = g_dir_open(path, 0, NULL);

    if (dir != NULL) {
        // Чтение каждого элемента в директории
        while ((name = g_dir_read_name(dir))) {
            gchar *next_path = g_build_filename(path, name, NULL);

            if (g_file_test(next_path, G_FILE_TEST_IS_DIR)) {
                // Если элемент является директорией, то заполняем модель данными из этой директории
                populate_model(store, &child_iter, next_path);
            } else {
                // Если элемент является файлом, добавляем его в дерево
                gtk_tree_store_append(store, &child_iter, NULL);
                gtk_tree_store_set(store, &child_iter,
                                   COLUMN_TITLE, g_path_get_basename(next_path),
                                   COLUMN_PATH, next_path, -1);
            }
            g_free(next_path);
        }
        g_dir_close(dir);
    }
}

// Функция для создания модели дерева
GtkTreeModel* create_model() {
    GtkTreeStore *store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    populate_model(store, NULL, g_get_current_dir());
    return GTK_TREE_MODEL(store);
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *view, *scrollwin;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    gtk_init(&argc, &argv);

    // Создание окна и указание его размера
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    view = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), create_model());

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Files", renderer, "text", COLUMN_TITLE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    scrollwin = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollwin), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrollwin), view);

    gtk_container_add(GTK_CONTAINER(window), scrollwin);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}