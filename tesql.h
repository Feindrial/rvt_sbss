#pragma once

#include "sqlite/sqlite3.h"

#include <map>
#include <vector>
#include <string>

extern sqlite3 *db;
extern std::map<unsigned int, std::string> errors;
extern std::map<unsigned int, bool> indexes;
extern std::vector<int> sql_cb_return;
extern int db_length;

void draw_db_gui(sqlite3 *db);
int sql_dml_callback(void *data, int argc, char **argv, char **col_name);
int sql_init_callback(void *data, int argc, char **argv, char **col_name);