#include <gtk/gtk.h>
#include "db.h"

typedef struct {
    GtkBuilder *builder;
    DBContext db;
    GtkWidget *tree_view;
    GtkWidget *query_entry;
    GtkWidget *table_combo; /* combo для выбора таблицы в главном окне */
} App;

App *app;

static GtkWidget *tables_box;
static GtkWidget *result_view;
static GtkListStore *result_store;
static QueryResult *current_result = NULL;
static char *current_table = NULL;
static GtkWidget *status_label;
static GtkWidget *sql_textview;

static void set_status(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char *s = g_strdup_vprintf(fmt, ap);
	va_end(ap);
	gtk_label_set_text(GTK_LABEL(status_label), s);
	g_free(s);
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    db_cleanup(&app->db);
    gtk_main_quit();
}

void on_cell_edited(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, gpointer data) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(app->tree_view));
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter_from_string(model, &iter, path_string)) {
        gint column = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "column"));
        gtk_list_store_set(GTK_LIST_STORE(model), &iter, column, new_text, -1);
    }
}

void on_execute_clicked(GtkWidget *btn, gpointer data) {
    const char *sql = gtk_entry_get_text(GTK_ENTRY(app->query_entry));
    if (!sql || !*sql) return;

    if (db_execute_query(&app->db, sql) == 0) {
        // Обновить столбцы в treeview
        GtkTreeView *tv = GTK_TREE_VIEW(app->tree_view);
        gtk_tree_view_set_model(tv, NULL);

        // Удалить старые столбцы
        GList *cols = gtk_tree_view_get_columns(tv);
        for (GList *l = cols; l; l = l->next) {
            gtk_tree_view_remove_column(tv, l->data);
        }
        g_list_free(cols);

        // Добавить новые
        for (int i = 0; i < app->db.column_count; i++) {
            GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
            g_object_set(renderer, "editable", TRUE, NULL);
            g_object_set_data(G_OBJECT(renderer), "column", GINT_TO_POINTER(i));
            g_signal_connect(renderer, "edited",
                G_CALLBACK(on_cell_edited), NULL);
            GtkTreeViewColumn *col = gtk_tree_view_column_new_with_attributes(
                app->db.column_names[i], renderer, "text", i, NULL);
            gtk_tree_view_append_column(tv, col);
        }

        gtk_tree_view_set_model(tv, GTK_TREE_MODEL(app->db.store));
    }
}

void on_add_record_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_OK) {
        // Get the table name and values from dialog
        GtkWidget *table_combo = g_object_get_data(G_OBJECT(dialog), "table_combo");
        GtkWidget *entry_grid = g_object_get_data(G_OBJECT(dialog), "entry_grid");

        const char *selected_table = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(table_combo));

        // Count the number of entry widgets in the grid
        GList *children = gtk_container_get_children(GTK_CONTAINER(entry_grid));
        int entry_count = 0;
        GList *current = children;
        while (current) {
            if (GTK_IS_ENTRY(GTK_WIDGET(current->data))) {
                entry_count++;
            }
            current = g_list_next(current);
        }

        // Collect values from the grid entries
        const char **values = g_malloc0((entry_count + 1) * sizeof(char*));
        current = children;
        int i = 0;

        while (current) {
            GtkWidget *child = GTK_WIDGET(current->data);
            if (GTK_IS_ENTRY(child)) {
                values[i] = gtk_entry_get_text(GTK_ENTRY(child));
                i++;
            }
            current = g_list_next(current);
        }
        g_list_free(children);

        // Insert the record into the database
        if (db_insert_row(&app->db, selected_table, values) == 0) {
            // Refresh the view
            const char *refresh_sql = gtk_entry_get_text(GTK_ENTRY(app->query_entry));
            if (refresh_sql && strlen(refresh_sql) > 0) {
                on_execute_clicked(NULL, NULL);
            }
        } else {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Ошибка добавления записи");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }

        g_free(values);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

// Function to populate fields based on selected table
void populate_fields_for_table(GtkWidget *combo, gpointer data) {
    GtkWidget *grid = GTK_WIDGET(data);
    const char *selected_table = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

    if (!selected_table) return;

    // Clear the grid
    GList *children = gtk_container_get_children(GTK_CONTAINER(grid));
    GList *current = children;
    while (current) {
        gtk_widget_destroy(GTK_WIDGET(current->data));
        current = g_list_next(current);
    }
    g_list_free(children);

    // Get column names for the selected table (use temporary context)
    DBContext temp_ctx;
    if (db_init(&temp_ctx, "zoo.db") != 0) return;

    char temp_sql[256];
    snprintf(temp_sql, sizeof(temp_sql), "SELECT * FROM \"%s\" LIMIT 1;", selected_table);

    if (db_execute_query(&temp_ctx, temp_sql) == 0) {
        for (int i = 0; i < temp_ctx.column_count; i++) {
            GtkWidget *label = gtk_label_new(temp_ctx.column_names[i]);
            GtkWidget *entry = gtk_entry_new();

            gtk_grid_attach(GTK_GRID(grid), label, 0, i, 1, 1);
            gtk_grid_attach(GTK_GRID(grid), entry, 1, i, 1, 1);
        }
    }

    db_cleanup(&temp_ctx);
    gtk_widget_show_all(grid);
}

void on_add_clicked(GtkWidget *btn, gpointer data) {
    // Create a dialog for adding records
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Добавить запись",
        GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
        GTK_DIALOG_MODAL,
        "Отмена", GTK_RESPONSE_CANCEL,
        "Добавить", GTK_RESPONSE_OK,
        NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Create a combo box to select table
    GtkWidget *table_label = gtk_label_new("Таблица:");
    GtkWidget *table_combo = gtk_combo_box_text_new();

    // Populate the combo box with table names
    char **tables = NULL;
    int table_count = 0;
    if (db_get_tables(&app->db, &tables, &table_count) == 0) {
        for (int i = 0; i < table_count; i++) {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(table_combo), tables[i]);
            free(tables[i]);
        }
        free(tables);
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(table_combo), 0); // Select first table by default

    GtkWidget *combo_box_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(combo_box_vbox), table_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(combo_box_vbox), table_combo, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(content_area), combo_box_vbox, FALSE, FALSE, 10);

    // Grid for input fields - will be populated based on selected table
    GtkWidget *entry_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(entry_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(entry_grid), 5);
    g_object_set_data(G_OBJECT(dialog), "table_combo", table_combo);
    g_object_set_data(G_OBJECT(dialog), "entry_grid", entry_grid);

    gtk_box_pack_start(GTK_BOX(content_area), entry_grid, FALSE, FALSE, 10);

    // Connect the combo box to populate fields based on selected table
    g_signal_connect(table_combo, "changed", G_CALLBACK(populate_fields_for_table), entry_grid);

    // Initially populate fields for the default table
    if (table_count > 0) {
        g_signal_emit_by_name(table_combo, "changed");
    }

    // Connect response signal
    g_signal_connect(dialog, "response", G_CALLBACK(on_add_record_dialog_response), NULL);

    gtk_widget_show_all(dialog);
}

void on_edit_record_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_OK) {
        // Get the table name, rowid, and values from dialog
        GtkWidget *table_combo = g_object_get_data(G_OBJECT(dialog), "table_combo");
        GtkWidget *entry_grid = g_object_get_data(G_OBJECT(dialog), "entry_grid");
        int *rowid = g_object_get_data(G_OBJECT(dialog), "rowid");

        const char *selected_table = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(table_combo));

        // Count the number of entry widgets in the grid
        GList *children = gtk_container_get_children(GTK_CONTAINER(entry_grid));
        int entry_count = 0;
        GList *current = children;
        while (current) {
            if (GTK_IS_ENTRY(GTK_WIDGET(current->data))) {
                entry_count++;
            }
            current = g_list_next(current);
        }

        // Collect values from the grid entries
        const char **values = g_malloc0((entry_count + 1) * sizeof(char*));
        current = children;
        int i = 0;

        while (current) {
            GtkWidget *child = GTK_WIDGET(current->data);
            if (GTK_IS_ENTRY(child)) {
                values[i] = gtk_entry_get_text(GTK_ENTRY(child));
                i++;
            }
            current = g_list_next(current);
        }
        g_list_free(children);

        // Update the record in the database
        if (db_update_row(&app->db, selected_table, "rowid", *rowid, values) == 0) {
            // Refresh the view
            const char *refresh_sql = gtk_entry_get_text(GTK_ENTRY(app->query_entry));
            if (refresh_sql && strlen(refresh_sql) > 0) {
                on_execute_clicked(NULL, NULL);
            }
        } else {
            GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Ошибка обновления записи");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }

        g_free(values);
        g_free(rowid);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void on_edit_clicked(GtkWidget *btn, gpointer data) {
    // Get the selected row from the tree view
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(app->tree_view));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        // Get the rowid (we'll use the position as rowid for simplicity)
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        int rowid = atoi(gtk_tree_path_to_string(path));
        gtk_tree_path_free(path);

        // Get the table name - for now, we'll use a simple approach by extracting it from the current query
        const char *current_sql = gtk_entry_get_text(GTK_ENTRY(app->query_entry));
        // Simple parsing to extract table name from SELECT query
        char table_name[256] = {0};
        if (strstr(current_sql, "SELECT") && strstr(current_sql, "FROM")) {
            const char *from_pos = strstr(current_sql, "FROM") + 4;
            while (*from_pos == ' ' || *from_pos == '\t') from_pos++;
            const char *end = from_pos;
            while (*end && *end != ' ' && *end != ';' && *end != '\n') end++;
            int len = end - from_pos;
            if ((size_t)len < sizeof(table_name)) {
                strncpy(table_name, from_pos, len);
                table_name[len] = '\0';
            }
        } else {
            // If no table name found, show error
            GtkWidget *error_dialog = gtk_message_dialog_new(
                GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Невозможно определить таблицу из текущего запроса");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            return;
        }

        // Create a dialog for editing records
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Редактировать запись",
            GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
            GTK_DIALOG_MODAL,
            "Отмена", GTK_RESPONSE_CANCEL,
            "Сохранить", GTK_RESPONSE_OK,
            NULL);

        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

        // Create a label for table name
        char title[512];
        snprintf(title, sizeof(title), "Таблица: %s, ID: %d", table_name, rowid);
        GtkWidget *table_label = gtk_label_new(title);
        gtk_box_pack_start(GTK_BOX(content_area), table_label, FALSE, FALSE, 10);

        // Grid for input fields - will be populated based on selected table
        GtkWidget *entry_grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(entry_grid), 5);
        gtk_grid_set_column_spacing(GTK_GRID(entry_grid), 5);
        g_object_set_data(G_OBJECT(dialog), "table_combo", NULL); // Not needed for edit
        g_object_set_data(G_OBJECT(dialog), "entry_grid", entry_grid);

        // Store the rowid to use later
        int *stored_rowid = g_malloc(sizeof(int));
        *stored_rowid = rowid;
        g_object_set_data(G_OBJECT(dialog), "rowid", stored_rowid);

        gtk_box_pack_start(GTK_BOX(content_area), entry_grid, FALSE, FALSE, 10);

        // Get current values for the selected row
        char **current_values = NULL;
        // Get column names for the selected table (we'll refresh the view temporarily)
        DBContext temp_ctx;
        db_init(&temp_ctx, "zoo.db");

        char temp_sql[256];
        snprintf(temp_sql, sizeof(temp_sql), "SELECT * FROM %s LIMIT 1;", table_name);

        if (db_execute_query(&temp_ctx, temp_sql) == 0) {
            // Get current values for the selected row
            if (db_get_row_by_id(&app->db, table_name, rowid, &current_values) == 0) {
                for (int i = 0; i < temp_ctx.column_count; i++) {
                    GtkWidget *label = gtk_label_new(temp_ctx.column_names[i]);
                    GtkWidget *entry = gtk_entry_new();

                    // Set the initial value from the database
                    if (current_values && i < temp_ctx.column_count) {
                        gtk_entry_set_text(GTK_ENTRY(entry), current_values[i]);
                    }

                    gtk_grid_attach(GTK_GRID(entry_grid), label, 0, i, 1, 1);
                    gtk_grid_attach(GTK_GRID(entry_grid), entry, 1, i, 1, 1);
                }

                // Free current values
                if (current_values) {
                    for (int i = 0; i < temp_ctx.column_count; i++) {
                        if (current_values[i]) free(current_values[i]);
                    }
                    free(current_values);
                }
            } else {
                // If we can't get current values, just create empty fields
                for (int i = 0; i < temp_ctx.column_count; i++) {
                    GtkWidget *label = gtk_label_new(temp_ctx.column_names[i]);
                    GtkWidget *entry = gtk_entry_new();

                    gtk_grid_attach(GTK_GRID(entry_grid), label, 0, i, 1, 1);
                    gtk_grid_attach(GTK_GRID(entry_grid), entry, 1, i, 1, 1);
                }
            }
        }

        db_cleanup(&temp_ctx);

        gtk_widget_show_all(entry_grid);

        // Connect response signal
        g_signal_connect(dialog, "response", G_CALLBACK(on_edit_record_dialog_response), NULL);

        gtk_widget_show_all(dialog);
    } else {
        GtkWidget *error_dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Пожалуйста, выберите запись для редактирования");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
    }
}

void on_delete_clicked(GtkWidget *btn, gpointer data) {
    // Get the selected row from the tree view
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(app->tree_view));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        // Get the rowid
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        int rowid = atoi(gtk_tree_path_to_string(path));
        gtk_tree_path_free(path);

        // Get the table name - extract it from the current query
        const char *current_sql = gtk_entry_get_text(GTK_ENTRY(app->query_entry));
        char table_name[256] = {0};
        if (strstr(current_sql, "SELECT") && strstr(current_sql, "FROM")) {
            const char *from_pos = strstr(current_sql, "FROM") + 4;
            while (*from_pos == ' ' || *from_pos == '\t') from_pos++;
            const char *end = from_pos;
            while (*end && *end != ' ' && *end != ';' && *end != '\n') end++;
            int len = end - from_pos;
            if ((size_t)len < sizeof(table_name)) {
                strncpy(table_name, from_pos, len);
                table_name[len] = '\0';
            }
        } else {
            // If no table name found, show error
            GtkWidget *error_dialog = gtk_message_dialog_new(
                GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Невозможно определить таблицу из текущего запроса");
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
            return;
        }

        // Confirm deletion with user
        GtkWidget *confirm_dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "Вы уверены, что хотите удалить запись с ID %d из таблицы %s?", rowid, table_name);

        gint result = gtk_dialog_run(GTK_DIALOG(confirm_dialog));
        gtk_widget_destroy(confirm_dialog);

        if (result == GTK_RESPONSE_YES) {
            // Delete the record from the database
            if (db_delete_row(&app->db, table_name, "rowid", rowid) == 0) {
                // Refresh the view
                const char *refresh_sql = gtk_entry_get_text(GTK_ENTRY(app->query_entry));
                if (refresh_sql && strlen(refresh_sql) > 0) {
                    on_execute_clicked(NULL, NULL);
                }
            } else {
                GtkWidget *error_dialog = gtk_message_dialog_new(
                    GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_ERROR,
                    GTK_BUTTONS_OK,
                    "Ошибка удаления записи");
                gtk_dialog_run(GTK_DIALOG(error_dialog));
                gtk_widget_destroy(error_dialog);
            }
        }
    } else {
        GtkWidget *error_dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_builder_get_object(app->builder, "main_window")),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Пожалуйста, выберите запись для удаления");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
    }
}

/* Обновить список таблиц в основном combo */
static void refresh_tables_main(void) {
    if (!app || !app->table_combo) return;
    char **tables = NULL;
    int table_count = 0;
    gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(app->table_combo));
    if (db_get_tables(&app->db, &tables, &table_count) == 0) {
        for (int i = 0; i < table_count; ++i) {
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->table_combo), tables[i]);
            free(tables[i]);
        }
        free(tables);
        if (table_count > 0) gtk_combo_box_set_active(GTK_COMBO_BOX(app->table_combo), 0);
    }
}

void on_refresh_tables_clicked(GtkWidget *btn, gpointer data) {
    refresh_tables_main();
    set_status("Список таблиц обновлён");
}

void on_browse_clicked(GtkWidget *btn, gpointer data) {
    gchar *tbl = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(app->table_combo));
    if (!tbl) { set_status("Таблица не выбрана"); return; }
    char q[512];
    snprintf(q, sizeof(q), "SELECT rowid, * FROM \"%s\";", tbl);
    gtk_entry_set_text(GTK_ENTRY(app->query_entry), q);
    on_execute_clicked(NULL, NULL);
    g_free(tbl);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    app = g_new0(App, 1);

    app->builder = gtk_builder_new_from_file("ui.glade");
    gtk_builder_connect_signals(app->builder, NULL);

    app->tree_view = GTK_WIDGET(gtk_builder_get_object(app->builder, "tree_view"));
    app->query_entry = GTK_WIDGET(gtk_builder_get_object(app->builder, "query_entry"));
    app->table_combo = GTK_WIDGET(gtk_builder_get_object(app->builder, "table_combo_main"));
    status_label = GTK_WIDGET(gtk_builder_get_object(app->builder, "status_label"));

    GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(app->builder, "main_window"));
    gtk_widget_show_all(window);

    // Инициализация БД
    if (db_init(&app->db, "zoo.db") != 0) {
        g_error("Не удалось открыть zoo.db");
        return 1;
    }

    /* заполним список таблиц в главном окне */
    refresh_tables_main();

    // Запустить тестовый запрос
    gtk_entry_set_text(GTK_ENTRY(app->query_entry), "SELECT * FROM Вид_животного;");
    on_execute_clicked(NULL, NULL);

    gtk_main();

    g_object_unref(app->builder);
    g_free(app);
    return 0;
}