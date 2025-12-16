// db.c
#include "db.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib.h>

static sqlite3 *db = NULL;

int db_open(const char *path, char **err) {
	if (db) return 0;
	if (sqlite3_open(path, &db) != SQLITE_OK) {
		if (err) *err = sqlite3_mprintf("%s", sqlite3_errmsg(db));
		sqlite3_close(db); db = NULL;
		return -1;
	}
	return 0;
}

void db_close(void) {
	if (!db) return;
	sqlite3_close(db); db = NULL;
}

static void free_query_result(QueryResult *r) {
	if (!r) return;
	for (int i = 0; i < r->ncols; ++i) g_free(r->cols[i]);
	g_free(r->cols);
	for (int i = 0; i < r->nrows; ++i) {
		for (int j = 0; j < r->ncols; ++j) g_free(r->rows[i][j]);
		g_free(r->rows[i]);
	}
	g_free(r->rows);
	free(r);
}

void db_free_result(QueryResult *r) {
	free_query_result(r);
}

int db_query(const char *sql, QueryResult **out, char **err) {
	if (!db) { if (err) *err = g_strdup("Database not opened"); return -1; }
	sqlite3_stmt *stmt = NULL;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
		if (err) *err = g_strdup(sqlite3_errmsg(db));
		return -1;
	}
	int ncols = sqlite3_column_count(stmt);
	QueryResult *res = calloc(1, sizeof(QueryResult));
	res->ncols = ncols;
	res->cols = calloc(ncols, sizeof(char*));
	for (int i = 0; i < ncols; ++i) {
		const char *name = sqlite3_column_name(stmt, i);
		res->cols[i] = g_strdup(name ? name : "");
	}
	// fetch rows
	int rc;
	int cap = 16;
	res->rows = calloc(cap, sizeof(char**));
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		if (res->nrows >= cap) {
			cap *= 2;
			res->rows = realloc(res->rows, cap * sizeof(char**));
		}
		res->rows[res->nrows] = calloc(ncols, sizeof(char*));
		for (int i = 0; i < ncols; ++i) {
			const unsigned char *txt = sqlite3_column_text(stmt, i);
			res->rows[res->nrows][i] = g_strdup(txt ? (const char*)txt : "");
		}
		res->nrows++;
	}
	if (rc != SQLITE_DONE) {
		if (err) *err = g_strdup(sqlite3_errmsg(db));
		free_query_result(res);
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);
	*out = res;
	return 0;
}

int db_exec_nonquery(const char *sql, char **err) {
	if (!db) { if (err) *err = g_strdup("Database not opened"); return -1; }
	char *local_err = NULL;
	if (sqlite3_exec(db, sql, NULL, NULL, &local_err) != SQLITE_OK) {
		if (err) *err = g_strdup(local_err ? local_err : "Unknown error");
		sqlite3_free(local_err);
		return -1;
	}
	return 0;
}

int db_list_tables(char ***names, int *count, char **err) {
	const char *sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name";
	QueryResult *r = NULL;
	if (db_query(sql, &r, err) != 0) return -1;
	*count = r->nrows;
	*names = calloc(*count, sizeof(char*));
	for (int i = 0; i < r->nrows; ++i) (*names)[i] = g_strdup(r->rows[i][0]);
	db_free_result(r);
	return 0;
}

static int get_table_columns(sqlite3 *handle, const char *table, char ***cols_out, int **pks_out, int *ncols_out) {
    char *sql = sqlite3_mprintf("PRAGMA table_info('%q')", table);
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(handle, sql, -1, &stmt, NULL) != SQLITE_OK) {
        sqlite3_free(sql);
        return -1;
    }
    sqlite3_free(sql);
    int cap = 8, n = 0;
    char **cols = g_malloc0(cap * sizeof(char*));
    int *pks = g_malloc0(cap * sizeof(int));
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (n >= cap) { cap *= 2; cols = g_realloc(cols, cap * sizeof(char*)); pks = g_realloc(pks, cap * sizeof(int)); }
        const unsigned char *name = sqlite3_column_text(stmt, 1);
        cols[n] = g_strdup(name ? (const char*)name : "");
        pks[n] = sqlite3_column_int(stmt, 5); /* pk flag */
        n++;
    }
    sqlite3_finalize(stmt);
    *cols_out = cols; *pks_out = pks; *ncols_out = n;
    return 0;
}

int db_init(DBContext *ctx, const char *path) {
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(*ctx));
    if (sqlite3_open(path, &ctx->handle) != SQLITE_OK) {
        return -1;
    }
    ctx->store = NULL;
    ctx->column_count = 0;
    ctx->column_names = NULL;
    return 0;
}

void db_cleanup(DBContext *ctx) {
    if (!ctx) return;
    if (ctx->store) g_object_unref(ctx->store), ctx->store = NULL;
    if (ctx->column_names) {
        for (int i = 0; i < ctx->column_count; ++i) g_free(ctx->column_names[i]);
        g_free(ctx->column_names);
        ctx->column_names = NULL;
    }
    if (ctx->handle) sqlite3_close(ctx->handle), ctx->handle = NULL;
    ctx->column_count = 0;
}

int db_execute_query(DBContext *ctx, const char *sql) {
    if (!ctx || !ctx->handle || !sql) return -1;
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(ctx->handle, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    int ncols = sqlite3_column_count(stmt);

    /* Free previous column names/store */
    if (ctx->store) g_object_unref(ctx->store), ctx->store = NULL;
    if (ctx->column_names) {
        for (int i = 0; i < ctx->column_count; ++i) g_free(ctx->column_names[i]);
        g_free(ctx->column_names); ctx->column_names = NULL;
    }
    ctx->column_count = ncols;
    if (ncols > 0) {
        ctx->column_names = g_malloc0(ncols * sizeof(char*));
        for (int i = 0; i < ncols; ++i) ctx->column_names[i] = g_strdup(sqlite3_column_name(stmt, i));
    } else {
        ctx->column_names = NULL;
    }

    /* Create list store with ncols string columns */
    GType *types = g_malloc0(sizeof(GType) * (ncols ? ncols : 1));
    for (int i = 0; i < (ncols ? ncols : 1); ++i) types[i] = G_TYPE_STRING;
    ctx->store = GTK_LIST_STORE(gtk_list_store_newv(ncols ? ncols : 1, types));
    g_free(types);

    /* Fetch rows */
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        GtkTreeIter iter;
        gtk_list_store_append(ctx->store, &iter);
        for (int i = 0; i < ncols; ++i) {
            const unsigned char *txt = sqlite3_column_text(stmt, i);
            gtk_list_store_set(ctx->store, &iter, i, txt ? (const char*)txt : "", -1);
        }
    }
    sqlite3_finalize(stmt);
    return 0;
}

int db_get_tables(DBContext *ctx, char ***names, int *count) {
    if (!ctx || !ctx->handle) return -1;
    const char *sql = "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%' ORDER BY name";
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(ctx->handle, sql, -1, &stmt, NULL) != SQLITE_OK) return -1;
    int cap = 8, n = 0;
    char **arr = g_malloc0(cap * sizeof(char*));
    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (n >= cap) { cap *= 2; arr = g_realloc(arr, cap * sizeof(char*)); }
        const unsigned char *txt = sqlite3_column_text(stmt, 0);
        arr[n++] = g_strdup(txt ? (const char*)txt : "");
    }
    sqlite3_finalize(stmt);
    *names = arr; *count = n;
    return 0;
}

int db_insert_row(DBContext *ctx, const char *table, const char **values) {
    if (!ctx || !ctx->handle || !table) return -1;
    char **cols = NULL; int *pks = NULL; int ncols = 0;
    if (get_table_columns(ctx->handle, table, &cols, &pks, &ncols) != 0) return -1;
    /* build INSERT for non-pk columns */
    GString *sql = g_string_new(NULL);
    g_string_printf(sql, "INSERT INTO \"%s\" (", table);
    int first = 1, bind_cnt = 0;
    for (int i = 0; i < ncols; ++i) {
        if (pks[i]) continue;
        if (!first) g_string_append(sql, ", ");
        g_string_append_printf(sql, "\"%s\"", cols[i]);
        first = 0; bind_cnt++;
    }
    g_string_append(sql, ") VALUES (");
    for (int i = 0; i < bind_cnt; ++i) { if (i) g_string_append(sql, ", "); g_string_append(sql, "?"); }
    g_string_append(sql, ")");
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(ctx->handle, sql->str, -1, &stmt, NULL) != SQLITE_OK) {
        g_string_free(sql, TRUE);
        for (int i = 0; i < ncols; ++i) g_free(cols[i]);
        g_free(cols); g_free(pks);
        return -1;
    }
    /* bind values (values array follows same column order) */
    int bind_index = 1;
    for (int i = 0; i < ncols; ++i) {
        if (pks[i]) continue;
        const char *v = values[i] ? values[i] : "";
        sqlite3_bind_text(stmt, bind_index++, v, -1, SQLITE_TRANSIENT);
    }
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    g_string_free(sql, TRUE);
    for (int i = 0; i < ncols; ++i) g_free(cols[i]);
    g_free(cols); g_free(pks);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int db_update_row(DBContext *ctx, const char *table, const char *id_column, int id_value, const char **values) {
    if (!ctx || !ctx->handle || !table || !id_column) return -1;
    char **cols = NULL; int *pks = NULL; int ncols = 0;
    if (get_table_columns(ctx->handle, table, &cols, &pks, &ncols) != 0) return -1;
    GString *sql = g_string_new(NULL);
    g_string_printf(sql, "UPDATE \"%s\" SET ", table);
    int first = 1;
    for (int i = 0; i < ncols; ++i) {
        if (pks[i]) continue;
        if (!first) g_string_append(sql, ", ");
        g_string_append_printf(sql, "\"%s\" = ?", cols[i]);
        first = 0;
    }
    if (g_ascii_strcasecmp(id_column, "rowid") == 0)
        g_string_append(sql, " WHERE rowid = ?");
    else
        g_string_append_printf(sql, " WHERE \"%s\" = ?", id_column);
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(ctx->handle, sql->str, -1, &stmt, NULL) != SQLITE_OK) {
        g_string_free(sql, TRUE);
        for (int i = 0; i < ncols; ++i) g_free(cols[i]);
        g_free(cols); g_free(pks);
        return -1;
    }
    int bind_index = 1;
    for (int i = 0; i < ncols; ++i) {
        if (pks[i]) continue;
        const char *v = values[i] ? values[i] : "";
        sqlite3_bind_text(stmt, bind_index++, v, -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_int(stmt, bind_index, id_value);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    g_string_free(sql, TRUE);
    for (int i = 0; i < ncols; ++i) g_free(cols[i]);
    g_free(cols); g_free(pks);
    return (rc == SQLITE_DONE) ? 0 : -1;
}

int db_get_row_by_id(DBContext *ctx, const char *table, int id, char ***out_values) {
    if (!ctx || !ctx->handle || !table || !out_values) return -1;
    char *sql = sqlite3_mprintf("SELECT * FROM \"%w\" WHERE rowid = ? LIMIT 1", table);
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(ctx->handle, sql, -1, &stmt, NULL) != SQLITE_OK) { sqlite3_free(sql); return -1; }
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) { sqlite3_finalize(stmt); sqlite3_free(sql); return -1; }
    int ncols = sqlite3_column_count(stmt);
    char **vals = g_malloc0(sizeof(char*) * ncols);
    for (int i = 0; i < ncols; ++i) {
        const unsigned char *t = sqlite3_column_text(stmt, i);
        vals[i] = t ? g_strdup((const char*)t) : g_strdup("");
    }
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    *out_values = vals;
    return 0;
}

int db_delete_row(DBContext *ctx, const char *table, const char *id_column, int id) {
    if (!ctx || !ctx->handle || !table || !id_column) return -1;
    char *sql;
    if (g_ascii_strcasecmp(id_column, "rowid") == 0)
        sql = sqlite3_mprintf("DELETE FROM \"%w\" WHERE rowid = ?", table);
    else
        sql = sqlite3_mprintf("DELETE FROM \"%w\" WHERE \"%w\" = ?", table, id_column);
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(ctx->handle, sql, -1, &stmt, NULL) != SQLITE_OK) { sqlite3_free(sql); return -1; }
    sqlite3_bind_int(stmt, 1, id);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_free(sql);
    return (rc == SQLITE_DONE) ? 0 : -1;
}