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

int error_eval(int error) {
    if(error == -111 || error == -112 || error == -113 || error == -114)
        return 1;
    
    return 0;
}

int main() {
    sqlite3* db;

    int dbo = sqlite3_open("Debug.db", &db);
    if(dbo != SQLITE_OK){
        //Error handling
        std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //Debug checks
        std::cout << "Database ok" << std::endl;
    }

    dbo = tables(db);

    dbo = in_new_cell(db, 2, 7, "Tesst_Card1", "Test_Set1");

    return 0;
    
}
