//Database Manager with queries
#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include "sqlite3.h"
#include <cstdlib>
#include <iostream>

int tables(sqlite3* db);

//Prepare a statement
int vprepare(sqlite3* db, const char* sql, sqlite3_stmt** stmt);

//Bind an integer value to a statement
int int_bind(sqlite3* db, sqlite3_stmt** stmt, int positiion, int value);

//Bind a text value to a statement
int text_bind(sqlite3* db, sqlite3_stmt** stmt, int position, std::string value);

//Check if the Album table exisit. Yhis function checks if it's the first time the application is executed and populates the database with the data essential to function
int existence(void* data, int argc, char** argv, char** colNames);

//Record the informations of a newly inserted card
int rec_data_c(sqlite3* db, std::string name);

//Record the cards information depending on the typing
//Creature (or vehicle) with supertype
int record_stats(sqlite3* db, std::string name, std::string color, std::string cost, std::string sutype, std::string type, std::string stats, std::string wording, int album_id);
//Creature (or vehicle) without supertype
int record_nosup(sqlite3* db, std::string name, std::string color, std::string cost, std::string type, std::string stats, std::string wording, int album_id);
//Non creature with supertype
int record_nostats(sqlite3* db, std::string name, std::string color, std::string cost, std::string sutype, std::string type, std::string wording, int album_id);
//Noncreature without supertype
int record_nostats_nosup(sqlite3* db, std::string name, std::string color, std::string cost, std::string type, std::string wording, int album_id);

//Create a new completely empty cell
int new_cell(sqlite3* db, int album_id);

//Cell function to assign a name to a cell and its correspective cell-card bridge
int assign_name(sqlite3* db, std::string name, int cell_id);

//Insert the card in the Cards table
int card_rec(sqlite3* db, std::string name, std::string set, int cell_id);

//Cell filling update
int fill_update(sqlite3* db, int cell_id, int fill);

//Remove a card from the cards database table
int remove_card(sqlite3* db, int card_id);

//Record a card into the given cell's correct position, given the card and the cell respective IDs
int insert_into(sqlite3* db, int cell_id, int card_id, int position = 0);

//Create a new cell and record the passed card
int in_new_cell(sqlite3* db, int album_id, int number, std::string name, std::string set);

//Inert in a passed cell
int insert_to_cell(sqlite3* db, std::string name, std::string set, int cell_id, int number, int counter = 0, int is_root = 1);

//Insert a card never inserted before
int first_insert_card(sqlite3* db, std::string name, std::string set, int number);


















/*//At first launch creates the tables for the cards database (db_manager.cpp 9 - 111)
int tables(sqlite3* db);

//New Album (db_manager.cpp 113 - 147)
int make_album(sqlite3* db, std::string name);

//New Cell (db_manager.cpp 149 - 234)
int make_cell(sqlite3* db, int album_id);

//New Card (db_manager.cpp 236 - 286)
int make_card(sqlite3* db, std::string card_name, std::string c_set, int card_number);

//Add new instance of recorded card (db_manager.cpp 288 - )
int find_and_insert(sqlite3* db, std::string card_name, std::string card_set, int card_number, int album_id);

//Insert new cards
int insert(sqlite3* db, std::string name, std::string set, int number, int cell, int filling);

//Record the data for a new card
int recorddata(sqlite3* db, std::string card_name);

//Bind data to statement
int text_bind(sqlite3* db, sqlite3_stmt** stmt, int position, std::string value);

int int_bind(sqlite3* db, sqlite3_stmt** stmt, int position, int value);

//Execute a "Prepare" instruction, avoiding repetitions (db_manager.cpp 288 - )
int vprepare(sqlite3* db, const char* sql, sqlite3_stmt** stmt);

int card_rec(sqlite3* db, std::string name, std::string set, int cell_id);

int new_cell(sqlite3* db, int album_id);*/
#endif