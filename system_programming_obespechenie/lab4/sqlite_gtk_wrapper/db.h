#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#include <gtk/gtk.h>

typedef struct {
	int ncols;
	char **cols;    // column names
	int nrows;
	char ***rows;   // rows[row][col] -> string
} QueryResult;

int db_open(const char *path, char **err);
void db_close(void);

int db_query(const char *sql, QueryResult **out, char **err); // SELECT style
void db_free_result(QueryResult *r);

int db_exec_nonquery(const char *sql, char **err); // INSERT/UPDATE/DELETE etc.

int db_list_tables(char ***names, int *count, char **err);
int db_table_info(const char *table, char ***cols, int *ncols, char **err);

int db_delete_row_by_rowid(const char *table, long rowid, char **err);
int db_update_row_by_rowid(const char *table, const char **cols, const char **values, int ncols, long rowid, char **err);

typedef struct {
    sqlite3 *handle;
    GtkListStore *store;    /* модель последнего SELECT (все колонки как строки) */
    int column_count;
    char **column_names;    /* массив строк, length == column_count */
} DBContext;

/* Контекстный API для main.c */
int db_init(DBContext *ctx, const char *path);
void db_cleanup(DBContext *ctx);
int db_execute_query(DBContext *ctx, const char *sql); /* создает ctx->store и ctx->column_* */
int db_get_tables(DBContext *ctx, char ***names, int *count);
int db_insert_row(DBContext *ctx, const char *table, const char **values);
int db_update_row(DBContext *ctx, const char *table, const char *id_column, int id_value, const char **values);
int db_get_row_by_id(DBContext *ctx, const char *table, int id, char ***out_values);
int db_delete_row(DBContext *ctx, const char *table, const char *id_column, int id);

#endif // DB_H