#include <sqlite3.h>
#include <stdio.h>
#include "lab04_sqlite.h"

int callback(void *, int, char **, char **);
void sqlite_get_data()
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // Query to get animals with their type info, feed and price
    char *sql = "SELECT \
                  animals.id, \
                  animal_types.name AS animal_type_name, \
                  animals.tag AS animal_tag, \
                  feeds.name AS feed_name, \
                  feeds.price AS feed_price \
              FROM \
                  animals \
              INNER JOIN animal_types ON animals.type_id = animal_types.id \
              INNER JOIN feeds ON animal_types.id = feeds.type_id;";
    rc = sqlite3_exec(db, sql, callback, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
    }
    sqlite3_close(db);
}

void sqlite_update(int animal_id, char *animal_tag, float feed_price) // feed_price is now used for updating feed price
{
    sqlite3 *db;
    char *err_msg = 0;
    int rc;
    if (SQLITE_OK != (rc = sqlite3_open(DB_FILE, &db)))
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }

    // First update animal tag
    char sql_animal[255];
    snprintf(sql_animal, sizeof(sql_animal), "UPDATE animals SET tag = '%s' WHERE id = %d;", animal_tag, animal_id);
    rc = sqlite3_exec(db, sql_animal, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to update data in animals\n");
        fprintf(stderr, "SQLite error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    // Then update corresponding feed price based on animal type
    // Get the animal's type first
    int animal_type_id = 0;
    sqlite3_stmt *stmt;
    const char *get_type_sql = "SELECT type_id FROM animals WHERE id = ?;";
    rc = sqlite3_prepare_v2(db, get_type_sql, -1, &stmt, NULL);
    if (rc == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, animal_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            animal_type_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (animal_type_id > 0) {
        char sql_feed[255];
        snprintf(sql_feed, sizeof(sql_feed), "UPDATE feeds SET price = %.2f WHERE type_id = %d;", feed_price, animal_type_id);
        rc = sqlite3_exec(db, sql_feed, NULL, NULL, &err_msg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to update data in feeds\n");
            fprintf(stderr, "SQLite error: %s\n", err_msg);
            sqlite3_free(err_msg);
        }
    }

    sqlite3_close(db);
}
