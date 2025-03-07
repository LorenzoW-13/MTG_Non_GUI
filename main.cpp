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

    //Switch menu index
    int menu = 0;

    //Card parameters
    std::string name;
    std::string set;
    int number;

    do {
        std::cout << "1: Load card\n0: Quit\n. . . ";
        std::cin >> menu;

        switch(menu) {
            case 1:
                //Input data
                std::cout << "Insert name: ";
                std::getline(std::cin, name);
                std::cout << "Insert set: ";
                std::getline(std::cin, set);
                std::cout << "Insert number of copies: ";
                std::cin >> number;

                dbo = make_card(db, name, set, number);
                //Break the application if at any point an error is returned by the higher layers
                if(error_eval(dbo)) {
                    //Close the database and shut the application
                    sqlite3_close(db);
                    return 1;
                }
                break;

            case 0:
                //0 is already the value and the cycle breaks by itself
                break;

            default:
                //Repeat the cycle asking for a correct input
                std::cout << "Please insert acceptable values: ";
                menu = -1;
                break;
        }
    }while(menu);

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
    

    /*for(int i = 0; i < 10; i++) {
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
    }**/

    recorddata(db, "Card");

    return 0;
    
}
