#ifndef LAB04_SQLITE_H
#define LAB04_SQLITE_H

#define DB_FILE "mydb.db"

void sqlite_get_data();
void sqlite_update(int animal_id, char *animal_tag, float dummy_price);

#endif
