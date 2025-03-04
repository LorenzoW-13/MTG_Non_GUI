//Database Manager with queries
#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include "sqlite3.h"
#include <cstdlib>
#include <iostream>

//At first launch creates the tables for the cards database (db_manager.cpp 9 - 111)
int tables(sqlite3* db);

//New Album (db_manager.cpp 113 - 147)
int make_album(sqlite3* db, std::string name);

//New Cell (db_manager.cpp 149 - 234)
int make_cell(sqlite3* db, int album_id);

//New Card (db_manager.cpp 236 - 286)
int make_card(sqlite3* db, std::string card_name, std::string c_set, int card_number);

//Add new instance of recorded card (db_manager.cpp 288 - )
int find_and_insert(sqlite3* db, std::string card_name, std::string card_set, int card_number, int album_id);

//Record the data for a new card
int recorddata(sqlite3* db, std::string card_name);

//Execute a "Prepare" instruction, avoiding repetitions (db_manager.cpp 288 - )
int vprepare(sqlite3* db, const char* sql, sqlite3_stmt** stmt);
#endif