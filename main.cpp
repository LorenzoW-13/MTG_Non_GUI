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
    //DEBUG: RNG for testing
    srand(time(0));
    int random_n = rand();

    //DB handle and name
    sqlite3* db;
    const char* db_name = "Test.db";

    //DB creation/opening
    int dbo = sqlite3_open(db_name, &db);
    if(dbo != SQLITE_OK){
        //Error handling
        std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //Debug checks
        std::cout << "Database ok" << std::endl;
    }

    //Tables creation at first launch
    dbo = tables(db);
    if(dbo != SQLITE_OK) {
        //Error handling
        sqlite3_close(db); //Error messages thrown by tables function
        return dbo;
    }
    else {
        //Debug checks
        std::cout << "Proceding . . ." << std::endl;
    }

    //Test "make_album"
    /*for(int i = 0; i < 3; i++) {
        std::string name = "Album_" + std::to_string(random_n + i);
        dbo = make_album(db, name);

        if(dbo != SQLITE_OK) {
            //Error handling
            sqlite3_close(db); //Error message thrown by make_album function
            return dbo;
        }
    }*/
    

    for(int i = 0; i < 10; i++) {
        dbo = make_cell(db, 1);
    }

    for(int i = 0; i < 10; i++) {
        dbo = make_cell(db, 2);
    }

    int j = 0;

    for(int i = 0; i < 1; i++) {
        if(i%2) {
            j++;
        }
        std::string name_C = "Card_" + std::to_string(random_n + j);

        dbo = make_card(db, name_C, "set", 1);
    }

    return 0;
    
}
