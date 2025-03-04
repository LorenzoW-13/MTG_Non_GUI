//database manager

//header
#include "Libraries/db_manager.h"

//Libraries
#include <vector>

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
                            "FOREIGN KEY (c1) REFERENCES Cards(id) ON DELETE SET NULL, "
                            "FOREIGN KEY (c2) REFERENCES Cards(id) ON DELETE SET NULL, "
                            "FOREIGN KEY (c3) REFERENCES Cards(id) ON DELETE SET NULL, "
                            "FOREIGN KEY (c4) REFERENCES Cards(id) ON DELETE SET NULL);";
    const char* Cards =     "CREATE TABLE IF NOT EXISTS Cards ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //Card id, progressive
                            "name TEXT NOT NULL, " //Card name, will match the storage cell's name
                            "c_set TEXT NOT NULL, " //Card's set
                            "price DECIMAL DEFAULT NULL, " //Price of the card, currently set to null
                            "cell_id INT NOT NULL, " //Cell of storage. NOTE: in future i will be adding a deck storing function
                            "FOREIGN KEY (cell_id) REFERENCES CellCards(id));";
    const char* CData =     "CREATE TABLE IF NOT EXISTS CData ("
                            "name TEXT PRIMARY KEY, " //Cards are saved and idicised by their name
                            "color TEXT, " //Card color identifier as W (white), B (black), U (blue), G (green), R (red) or N (colorless)
                            "cost TEXT NOT NULL, " //Mana cost of the card, which uses the same symbols as the color
                            "sutype TEXT DEFAULT NULL, " //Card Supertype, ex: Basic, Legendary, Snow and World
                            "typing TEXT NOT NULL, " //Card Type (Land, Creature, Artifact, Enchantment, Planeswalker, Battle, Instant, Sorcery) and Subtype
                            "stats TEXT DEFAULT NULL, " //If creature, vehicle ecc, the strenght and toughness in form S/T (ex: 1/1)
                            "wording TEXT DEFAULT NULL, " //Text of the card. SET TO DEFAULT NULL FOR DEBUG ONLY; CHANGE TO NOT NULL IN RELEASE
                            "album_id INTEGER NOT NULL, " //Album in which the card need to be saved
                            "FOREIGN KEY(album_id) REFERENCES Album(id));";
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

    dbo = sqlite3_exec(db, CData, nullptr, nullptr, &errormessage);
    if(dbo != SQLITE_OK) {
        //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
        std::cerr << "SQL error: " << errormessage << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Card data created/found successfully" << std::endl;
    }

    return dbo;
}

int make_album(sqlite3* db, std::string name) {
    //Type recasting from std::string to const char* (c string)
    const char* a_name = name.c_str();

    //Statement declaration
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO Album (name) VALUES (?);";

    //Prepare the statement
    int dbo = vprepare(db, sql, &stmt);
    //Error handling
    if(dbo != SQLITE_OK) {
        //Error message thrown by vprepare, signal the error to main
        return dbo;
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

    sqlite3_stmt* stmt = nullptr;

    //Search for max cell number in given album
    //Prepare the statement to insert card name
    int dbo = vprepare(db, sql1, &stmt);
    //Error handling
    if(dbo != SQLITE_OK) {
        //Error message thrown by vprepare, signal the error to main
        return dbo;
    }

    sqlite3_bind_int(stmt, 1, album_id);
    int max_cell_number;
    
    dbo = sqlite3_step(stmt);
    //Execute the query to retrieve the highest cell number 
    if(dbo == SQLITE_ROW) {
        max_cell_number = sqlite3_column_int(stmt, 0);
    }
    else if (dbo == SQLITE_DONE) {
        max_cell_number = 0;
    }

    //The next cell has number equal to the found value +1
    max_cell_number++;

    sqlite3_finalize(stmt);

    //Prepare the statement to insert card name
    dbo = vprepare(db, sql2, &stmt);
    //Error handling
    if(dbo != SQLITE_OK) {
        //Error message thrown by vprepare, signal the error to main
        return dbo;
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

    //Prepare the statement 
    dbo = vprepare(db, sql3, &stmt);
    //Error handling
    if(dbo != SQLITE_OK) {
        //Error message thrown by vprepare, signal the error to main
        return dbo;
    }

    sqlite3_bind_int(stmt, 1, id);

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        std::cout << "New cell created successfully" << std::endl;
    }

    sqlite3_finalize(stmt);

    return dbo;
}

int make_card(sqlite3* db, std::string name, std::string set, int NCards = 1) {
    //Check if the card has already been inserted before
    const char* sql1 = "SELECT album_id FROM CData WHERE name = ?;";
    //Album id (if found) is saved as an int variable
    int album = -1;
    //Before searching, assume all cards have never been recorded
    //Obsolete: i run the functions directly in the if else cases
    //int found = 0;

    //Statement handle
    sqlite3_stmt* stmt = nullptr;

    //Prepare the statement to insert card name
    int dbo = vprepare(db, sql1, &stmt);
    //Error handling
    if(dbo != SQLITE_OK) {
        //Error message thrown by vprepare, signal the error to main
        return dbo;
    }

    //Insert the name to be searched
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, nullptr);

    //Check if the card already exists and, if not, call the "create_data" function
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        //Debug check
        std::cout << "Found correctly" << std::endl;

        album = sqlite3_column_int(stmt, 1);
        //found = 1;

        dbo = find_and_insert(db, name, set, NCards, album);
    }
    else {
        //Debug check
        std::cout << "Card " << name << " not found" << std::endl;

        int album_id = recorddata(db, name);
        //Call "insert_in_new", which makes a new cell and save up to 4 cards, calling itself recursively untill no cards are to add, passiing name, album_id, and number of cards (NCards)
    }

    //Close the statement
    sqlite3_finalize(stmt);

    //End function
    return dbo;
}

int vprepare(sqlite3* db, const char* sql, sqlite3_stmt** stmt) {
    int dbo = sqlite3_prepare_v2(db, sql, -1, stmt, nullptr);

    if(dbo != SQLITE_OK) {
        std::cerr << "Statement preparation error: " << sqlite3_errmsg(db) << std::endl;
        return dbo;
    }
    else {
        //Debug check
        std::cout << "Prepared correctly" << std::endl;
    }

    return dbo;
}

int find_and_insert(sqlite3* db, std::string card_name, std::string card_set,  int card_number, int album_id) {
    //Statement to locate the free cells
    const char* sql1 = "SELECT id, filled FROM Cells WHERE name = ? AND filled != 4 ORDER BY id ASC LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;

    //Cell id to use
    int id = -1;

    //Preparing statement
    int dbo = vprepare(db, sql1, &stmt);
    //Error handling
    if(dbo != SQLITE_OK) {
        //Error message thrown by vprepare, signal the error to main
        return dbo;
    }

    sqlite3_bind_text(stmt, 1, sql1, -1, nullptr);

    //If there are available cells, record the first available cell id
    if(sqlite3_step(stmt) == SQLITE_ROW) {
        //Record cell id and free c_cards
        id = sqlite3_column_int(stmt, 1);
        int filled = sqlite3_column_int(stmt, 2);

        //Record the free spaces
        int upperbound = 4 - filled;

        //If there are more free spaces than cards, the function runs up to card_number, vice versa all free spots are filled and there may be residual card (recursive call)
        if(upperbound > card_number)
            upperbound = card_number;

        //Create the card and prepare the statement
        const char* sql2 = "INSERT INTO Cards (name, c_set, cell_id) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt2 = nullptr;

        //Prepare the statement to insert card name
        int dbo = vprepare(db, sql2, &stmt);
        //Error handling
        if(dbo != SQLITE_OK) {
            //Error message thrown by vprepare, signal the error to main
            return dbo;
        }

        sqlite3_bind_text(stmt2, 1, card_name.c_str(), -1, nullptr);
        sqlite3_bind_text(stmt2, 2, card_set.c_str(), -1, nullptr);
        sqlite3_bind_int(stmt2, 3, id);

        int card_id;

        //Insert the cards in the free spots
        for(int i = 0; i < upperbound; i++) {
            switch(filled) {
                //All spaces are free, record in c1
                case 0: 
                    //Insert the card, recording the id of the (newly created) card, then insert the id into the c1 element
                    if(sqlite3_step(stmt2) == SQLITE_DONE) {
                        //Debug check
                        std::cout << "Inserted correctly" << std::endl;

                        //Get inserted card id
                        card_id = sqlite3_last_insert_rowid(db);

                        //Insert value into the c1 cell
                        sqlite3_stmt* stmt3 = nullptr;
                        const char* sql3 = "UPDATE CellCards SET c1 = ? WHERE id = ?;";
                        dbo = vprepare(db, sql3, &stmt3); 
                        //Error handling
                        if(dbo != SQLITE_OK) {
                            //Error message thrown by vprepare, signal the error to main
                            return dbo;
                        }

                        sqlite3_bind_int(stmt3, 1, card_id);
                        sqlite3_bind_int(stmt3, 2, id);

                        if(sqlite3_step(stmt3) == SQLITE_DONE) {
                            //Debug check
                            std::cout << "Inserted correctly" << std::endl;
                        }
                        else {
                            std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
                        }

                        sqlite3_finalize(stmt3);

                        const char* sql4 = "UPDATE Cells SET filled = filled + 1 WHERE id = ? AND filled < 4;";
                        dbo = vprepare(db, sql4, &stmt3); 
                        //Error handling
                        if(dbo != SQLITE_OK) {
                            //Error message thrown by vprepare, signal the error to main
                            return dbo;
                        }

                        sqlite3_bind_int(stmt3, 1, id);

                        if(sqlite3_step(stmt3) == SQLITE_DONE) {
                            //Debug check
                            std::cout << "Updated correctly" << std::endl;
                        }
                        else {
                            std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
                        }

                    }
                    else {
                        //Debug check
                        std::cout << "Card " << card_name << " not found" << std::endl;

                        //Call "create_data", which returns an int "album_id", passing name
                        //Call "insert_in_new", which makes a new cell and save up to 4 cards, calling itself recursively untill no cards are to add, passiing name, album_id, and number of cards (NCards)
                    }
                    
            }
        }


    }

    return dbo;
}

int recorddata(sqlite3* db, std::string name) {
    //Non-GUI version of record new card data
    std::string colors;
    std::string cost;
    std::string sutype = "NULL";
    std::string type;
    std::string wording;
    int id;
    std::string stats = "0/0";

    std::string empty;

    std::cout << "Insert colors (w, U, B, R, G, C for White, Blue, Black, Red, Green and Colorless): ";
    std::cin >> colors;
    std::cout << "Insert cost (for chromatic mana use format above): ";
    std::cin >> colors;
    std::cout << "Insert supertype (if not present type \"/\"): ";
    std::cin >> empty;

    if(empty != "\n")
        sutype = empty;

    std::cout << "Insert type and subtypes (divided by a comma \",\"): ";
    std::cin >> type;

    if(colors == "W") {
        id = 1;
    }
    else if(colors == "U") {
        id = 2;
    }
    else if(colors == "B") {
        id = 3;
    }
    else if(colors == "R") {
        id = 4;
    }
    else if(colors == "G") {
        id = 5;
    }
    else if (colors == "C") {
        id = 6;
    }
    else {
        id = 7;
    }

    const char* sql = "INSERT INTO CData (name, color, cost, sutype, typing, stats, wording, album_id) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    int dbo = vprepare(db, sql, &stmt);
    if(dbo != SQLITE_OK) {
        return -1;
    }

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        //Debug check
        std::cout << "Inserted correctly" << std::endl;
    }
    else {
        std::cerr << "SQL Error: " << sqlite3_errmsg(db) << std::endl;
        return -2;
        
    }

    sqlite3_finalize(stmt);

    return id;
}