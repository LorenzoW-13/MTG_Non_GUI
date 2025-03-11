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

    if(text_bind(db, &stmt, 2, set) != SQLITE_OK) {
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
    }

    //If binding is successfull, signal the success
    return SQLITE_OK;
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