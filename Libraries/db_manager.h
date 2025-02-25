//Database Manager with queries
#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include "sqlite3.h"
#include <cstdlib>
#include <iostream>

//At first launch creates the tables for the cards database
int tables(sqlite3* db);

int make_album(sqlite3* db, std::string name);

int make_cell(sqlite3* db, int album_id);

#endif