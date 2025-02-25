//Non-GUI Manager app

//SQLite header
#include "Libraries/sqlite3.h"

//Standard C++ and I/O Libraries
#include <cstdlib>
#include <iostream>

//Databse manager class
#include "Libraries/db_manager.h"

//Test libraries
#include <ctime>
#include <string>

int main() {
    //Random number generator for quick testing
    srand(time(0));
    int random_n = rand();


    //Database handle and name
    sqlite3* db;
    const char* db_name = "test.db";

    //Database creation/opening and error handling
    int dbo = sqlite3_open(db_name, &db);
    if(dbo != SQLITE_OK) {
        //If there's an error in the DB the program closes to prevent data loss
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //debug controls
        std::cout << "Database ok" << std::endl;
    }

    dbo = tables(db);
    if (dbo != SQLITE_OK) {
        //Error message already thrown by db_manager
        sqlite3_close(db);
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Proceding. . ." << std::endl;
    }

    for(int i = 0; i < 2; i++) {
        std::string name = "Album_" + std::to_string(random_n + i);

        dbo = make_album(db, name);
        if (dbo != SQLITE_OK) {
        //Error message thrown by db_manager
        sqlite3_close(db);
        return dbo;
    }
    }

    for(int i = 0; i < 10; i++) {
        dbo = make_cell(db, 1);
    }

    for(int i = 0; i < 10; i++) {
        dbo = make_cell(db, 2);
    }
    
    return 0;
    
}
