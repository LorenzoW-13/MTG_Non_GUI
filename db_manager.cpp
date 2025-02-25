//database manager

//header
#include "Libraries/db_manager.h"

int tables(sqlite3* db) {
    //Table Album
    const char* Albums =    "CREATE TABLE IF NOT EXISTS Album ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //Album id, progerssive
                            "name TEXT NOT NULL, " //Album name, will be same as color of contained cards
                            "UNIQUE(name));";
    const char* Cells =     "CREATE TABLE IF NOT EXISTS Cells ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //Cell id (not number), progressive
                            "name TEXT DEFAUL NULL, " //Cell name, matches the saved cards name
                            "album_id INTEGER, " //Prent album id, as foreign key
                            "number INTEGER NOT NULL, " //Cell number, unique inside the album
                            "filled INTEGER DEFAULT 0, " //If all 4 slots of the CellCards table's row corresponding to the current cell are filled, the value changes to 1
                            "FOREIGN KEY(album_id) REFERENCES Album(id), "
                            "UNIQUE(number, album_id));";
    const char* CellsC =    "CREATE TABLE IF NOT EXISTS CellCards ("
                            "id INTEGER PRIMARY KEY, " //Cell id, referencing Cell table as foreign key
                            "c1 INTEGER DEFAULT NULL, " //Card saved in the first slot, by id number
                            "c2 INTEGER DEFAULT NULL, "
                            "c3 INTEGER DEFAULT NULL, "
                            "c4 INTEGER DEFAULT NULL, "
                            "FOREIGN KEY (id) REFERENCES Cells(id), "
                            "FOREIGN KEY (c1) REFERENCES Cards(id), "
                            "FOREIGN KEY (c2) REFERENCES Cards(id), "
                            "FOREIGN KEY (c3) REFERENCES Cards(id), "
                            "FOREIGN KEY (c4) REFERENCES Cards(id));";
    const char* Cards =     "CREATE TABLE IF NOT EXISTS Cards ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //Card id, progressive
                            "name TEXT NOT NULL, " //Card name, will match the storage cell's name
                            "color TEXT, " //Card color identifier as W (white), B (black), U (blue), G (green), R (red) or N (colorless)
                            "c_set TEXT NOT NULL, " //Card's set
                            "cost TEXT NOT NULL, " //Mana cost of the card, which uses the same symbols as the color
                            "price DECIMAL DEFAULT NULL, " //Price of the card, currently set to null
                            "cell_id INT NOT NULL, " //Cell of storage. NOTE: in future i will be adding a deck storing function
                            "FOREIGN KEY (cell_id) REFERENCES CellCards(id));";
    //Error message for SQLite operations
    char* errormessage;

    //Execute all instructions in succession, building the various tables
    int dbo = sqlite3_exec(db, Albums, nullptr, nullptr, &errormessage);
    if(dbo != SQLITE_OK) {
        //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
        std::cerr << "SQL error: " << errormessage << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Album created/found successfully" << std::endl;
    }

    dbo = sqlite3_exec(db, Cells, nullptr, nullptr, &errormessage);
    if(dbo != SQLITE_OK) {
        //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
        std::cerr << "SQL error: " << errormessage << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Cells created/found successfully" << std::endl;
    }

    dbo = sqlite3_exec(db, CellsC, nullptr, nullptr, &errormessage);
    if(dbo != SQLITE_OK) {
        //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
        std::cerr << "SQL error: " << errormessage << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "CellCard created/found successfully" << std::endl;
    }

    dbo = sqlite3_exec(db, Cards, nullptr, nullptr, &errormessage);
    if(dbo != SQLITE_OK) {
        //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
        std::cerr << "SQL error: " << errormessage << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Cards created/found successfully" << std::endl;
    }

    return dbo;
}

int make_album(sqlite3* db, std::string name) {
    //Type recasting from std::string to const char* (c string)
    const char* a_name = name.c_str();

    //Statement declaration
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO Album (name) VALUES (?);";

    //Statement preparation
    int dbo = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if(dbo != SQLITE_OK) {
        //Error message thrown, function stops. main also stops
        std::cerr << "Statement preparation error" << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Statement created correctly" << std::endl;
    }

    sqlite3_bind_text(stmt, 1, a_name, -1, SQLITE_STATIC);

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        //Error message thrown
        std::cerr << "SQLITE error " << sqlite3_errmsg(db) << std::endl; 
    }
    else {
        //Debug check
        std::cout << "Album " << name << " recorded correctly" << std::endl;
    }

    sqlite3_finalize(stmt);

    return dbo;
}

int make_cell(sqlite3* db, int album_id) {
    //Get the biggest cell number in the needed album, passed by user
    const char* sql1 = "SELECT number FROM Cells WHERE album_id = ? ORDER BY number DESC LIMIT 1;";
    //Create the next cell in the selected album
    const char* sql2 = "INSERT INTO Cells (album_id, number) VALUES (?, ?);";
    //Create the CellCards entry
    const char* sql3 = "INSERT INTO CellCards (id) VALUES (?);";

    sqlite3_stmt* stmt;

    //Search for max cell number in given album
    int dbo = sqlite3_prepare_v2(db, sql1, -1, &stmt, nullptr);
    if(dbo != SQLITE_OK) {
        //Error message thrown, function stops
        std::cerr << "Statement preparation error" << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Search Statement created correctly" << std::endl;
    }

    sqlite3_bind_int(stmt, 1, album_id);
    int max_cell_number;
    
    dbo = sqlite3_step(stmt);
    //Execute the query to retrieve the highest card number 
    if(dbo == SQLITE_ROW) {
        max_cell_number = sqlite3_column_int(stmt, 0);
    }
    else if (dbo == SQLITE_DONE) {
        max_cell_number = 0;
    }

    //The next cell has number equal to the found value +1
    max_cell_number++;

    sqlite3_finalize(stmt);

    dbo = sqlite3_prepare_v2(db, sql2, -1, &stmt, nullptr);
    if(dbo != SQLITE_OK) {
        //Error message thrown, function stops
        std::cerr << "Statement preparation error" << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Cell Statement created correctly" << std::endl;
    }

    sqlite3_bind_int(stmt, 1, album_id);
    sqlite3_bind_int (stmt, 2, max_cell_number);

    //Id of the newly inserted key
    int id;

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        id = sqlite3_last_insert_rowid(db);
    }
    else {
        id = 1;
    }

    sqlite3_finalize(stmt);

    dbo = sqlite3_prepare_v2(db, sql3, -1, &stmt, nullptr);
    if(dbo != SQLITE_OK) {
        //Error message thrown, function stops
        std::cerr << "Statement preparation error" << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "CellsC Statement created correctly" << std::endl;
    }

    sqlite3_bind_int(stmt, 1, id);

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "New cell created successfully" << std::endl;
    }

    sqlite3_finalize(stmt);

    return dbo;
}