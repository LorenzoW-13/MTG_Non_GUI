//database manager

//header
#include "Libraries/db_manager.h"

int tables(sqlite3* db) {
    //Table Album
    const char* Albums =    "CREATE TABLE IF NOT EXISTS Album ("
                            //Album id, progerssive
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            //Album name, will be same as color of contained cards
                            "name TEXT NOT NULL, "
                            "UNIQUE(name));";
    const char* Cells =     "CREATE TABLE IF NOT EXISTS Cells ("
                            //Cell id (not number), progressive
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            //Cell name, matches the saved cards name
                            "name TEXT, "
                            //Prent album id, as foreign key
                            "album_id INTEGER, "
                            //Cell number, unique inside the album
                            "number INTEGER NOT NULL, "
                            "FOREIGN KEY(album_id) REFERENCES Album(id), "
                            "UNIQUE(number, album_id));";
    const char* CellsC =    "CREATE TABLE IF NOT EXISTS CellCards ("
                            //Cell id, referencing Cell table as foreign key
                            "id INTEGER PRIMARY KEY, "
                            //Card saved in the first slot, by id number
                            "c1 INTEGER DEFAULT NULL, "
                            "c2 INTEGER DEFAULT NULL, "
                            "c3 INTEGER DEFAULT NULL, "
                            "c4 INTEGER DEFAULT NULL, "
                            "FOREIGN KEY (id) REFERENCES Cells(id), "
                            "FOREIGN KEY (c1) REFERENCES Cards(id), "
                            "FOREIGN KEY (c2) REFERENCES Cards(id), "
                            "FOREIGN KEY (c3) REFERENCES Cards(id), "
                            "FOREIGN KEY (c4) REFERENCES Cards(id));";
    const char* Cards =     "CREATE TABLE IF NOT EXISTS Cards ("
                            //Card id, progressive
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            //Card name, will match the storage cell's name
                            "name TEXT NOT NULL, "
                            //Card color identifier as W (white), B (black), U (blue), G (green), R (red) or N (colorless)
                            "color TEXT, "
                            //Card's set
                            "c_set TEXT NOT NULL, "
                            //Mana cost of the card, which uses the same symbols as the color
                            "cost TEXT NOT NULL, "
                            //Price of the card, currently set to null
                            "price DECIMAL DEFAULT NULL, "
                            //Cell of storage. NOTE: in future i will be adding a deck storing function
                            "cell_id INT NOT NULL, "
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