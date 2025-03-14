//Changing version: the finalize is to be executed by the caller, the close is to be executed by main or a direct delegate (like error handling)

int card_rec(sqlite3* db, std::string name, std::string set, int number, int cell_id, int cell_fill) {
    //Insert statement and handle
    const char* sql = "INSERT INTO Cards (name, set, cell_id) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if(vprepare(db, sql, &stmt) != SQLITE_OK) {
        //Prepare Error
        return -112;
    }

    if(text_bind(db, &stmt, 1, name) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 3, cell_id) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    //Execute the step
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        //Step Error
        sqlite3_finalize(stmt);
        return -113;
    }

    //Operation successfull, record the inserted card id
    int c_id = sqlite_last_insert_rowid(db);

    //Return the id
    return c_id;

}



int int_bind(sqlite3* db, sqlite3_stmt** stmt, int position, int value) {
    //If binding for the first column of a statement, reset the statement as a safety measure
    if(position == 1) {
        sqlite_reset(*stmt);
    }

    //Execute the integer binding routine
    if(sqlite_bind_int(*stmt, position, value) != SQLITE_OK) {
        //Signal the binding has not succeeded and throw the error message
        std::cerr << "Binding error: " << sqlite3_errmsg(db) << std::endl;
        return -114;
    }

    //If binding is successfull, signal the success
    return SQLITE_OK;
}

int text_bind(sqlite3* db, sqlite3_stmt** stmt, int position, int value) {
    //If binding for the first column of a statement, reset the statement as a safety measure
    if(position == 1) {
        sqlite3_reset(*stmt);
    }

    //Execute the string binding rutine
    if(sqlite_bind_text(*stmt, position, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        //Signal the binding has not succeeded and throw the error message
        std::cerr << "Binding error: " << sqlite _errmsg(db) << std::endl;
        return -114;
    }c
}

int vprepare(sqlite3* db, const char* sql, sqlite3_stmt** stmt) {
    //Prepare the statement
    if(sqlite_prepare_v2(db, sql, -1, stmt, nullptr) != SQLITE_OK) {
        //Signal preparation has failed and throw error message
        std::cerr << "Binding error: " << sqlite_errmsg(db) << std::endl;
        return -112;
    }

    //If preparation is successfull, signal the success
    return SQLITE_OK;
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
    //dbo = tables(db);
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

    /*do {
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
    }while(menu);*/

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
        //dbo = make_cell(db, 1);
    }

    card_rec(db, "3", "2", 1);

    for(int i = 0; i < 5; ++i) {
        int id = new_cell(db, 1);
        std::cout << "inserted cell id: " << id << std::endl;
    }

    /*for(int i = 0; i < 10; i++) {
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

    //recorddata(db, "Card");

    return 0;
}