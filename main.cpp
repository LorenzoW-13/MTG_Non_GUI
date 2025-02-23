//Non-GUI Manager app

//SQLite header
#include "Libraries/sqlite3.h"

//Standard C++ and I/O Libraries
#include <cstdlib>
#include <iostream>

//Databse manager class
#include "Libraries/db_manager.h"

int main() {
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

    
}
