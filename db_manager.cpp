//database manager

//header
#include "Libraries/db_manager.h"

//Libraries
#include <vector>

//text_bind function
int text_bind(sqlite3* db, sqlite3_stmt** stmt, int position, std::string value) {
    //While binding to first position, reset the statement.
    if (position == 1) {
        sqlite3_reset(*stmt);
    }

    //Bind the values to position
    if(sqlite3_bind_text(*stmt, position, value.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind value: "<< sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(*stmt);
        sqlite3_close(db);
        return -114;
    }

    return SQLITE_OK;
}

//Existence callback
int existence(void* data, int argc, char** argv, char** colNames) {
    //Cast the data to int (from void*)
    int* exists = static_cast<int*>(data);
    //If there is at least 1 row the table exists
    if(argc > 0 && argv[0]) {
        *exists = std::stoi(argv[0]);
    }
    else {
        *exists = 0;
    }

    return 0;
}

//Create tables at first launch
int tables(sqlite3* db) {
    //Album(s)
    const char* Albums =    "CREATE TABLE IF NOT EXISTS Album (" //IF NOT EXISTS clause prevent duplication error. It's an additional layer of protection in case the later checks were to fail
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //See README.md, section "- 2:Album(s) organization"
                            "name TEXT NOT NULL, " //The names will match the colors of the cards saved in each album, W (for white), U (for blue), B (black), R (red), G (green), C (colorless), and M (multi)
                            "UNIQUE (name));";
    const char* Cells =     "CREATE TABLE IF NOT EXISTS Cells ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //Cell ID is NOT Cell Number, which is unique in each album but repeatable globally (and thus can't be Prim. Key)
                            "name TEXT DEFAULT NULL, " //Cell Name, matches the name of the (up to 4) cards that will be held in the cell. By default is null, as cells are created empty
                            "album_id INTEGER, " //Album of reference, is FOREIGN KEY (see below)
                            "number INTEGER NOT NULL, " //Cell Number, is unique within each number but not globally
                            "filled INTEGER DEFAULT 0, " //Marks how many cards are already recorded in this cell, goes up to 4, where 4 is "Cell Completely Filled"
                            "FOREIGN KEY(album_id) REFERENCES Album(id), "
                            "UNIQUE(number, album_id));";
    const char* CellsC =    "CREATE TABLE IF NOT EXISTS CellCards (" //Bridge table that records the cards saved in each cell
                            "id INTEGER PRIMARY KEY, " //Cell ID, is FOREIGN KEY (see below)
                            "c1 INTEGER DEFAULT NULL, " //First card of this cell, is FOREIGN KEY (see below)
                            "c2 INTEGER DEFAULT NULL, " //Second card of this cell, is FOREIGN KEY (see below)
                            "c3 INTEGER DEFAULT NULL, " //Third card of this cell, is FOREIGN KEY (see below)
                            "c4 INTEGER DEFAULT NULL, " //Fourth card of this cell, is FOREIGN KEY (see below)
                            "FOREIGN KEY(id) REFERENCES Cells(id), "
                            "FOREIGN KEY(c1) REFERENCES Cards(id), "
                            "FOREIGN KEY(c2) REFERENCES Cards(id), "
                            "FOREIGN KEY(c3) REFERENCES Cards(id), "
                            "FOREIGN KEY(c4) REFERENCES Cards(id));";
    const char* Cards =     "CREATE TABLE IF NOT EXISTS Cards ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //Card id, primary key as Name is NOT UNIQUE
                            "name TEXT NOT NULL, " //Name can be repeated as multiple cards can have the same nameù
                            "c_set TEXT NOT NULL, " //Cards of same Name have been printed in multiple Set(s)
                            "price DECIMAL DEFAULT NULL, " //Cards have prices characteristic of both Name and Set. For first release this field is set to NULL
                            "cell_id INT NOT NULL, " //Cards must be saven in album cell or in deck (feature for future version)
                            "FOREIGN KEY(cell_id) REFERENCES Cells(id));";
    const char* CData =     "CREATE TABLE IF NOT EXISTS CData ("
                            "name TEXT PRIMARY KEY, " //Each card name has all the same data except the price, which depends on set. Since Version 1 ignores prices, Name can be the primary key.
                            "color TEXT NOT NULL, " //The card color(s), expressed by 1 or more letters, where multiple letters will be inserted in album M, and single-letters will be inserted in the matching album name.
                            "cost TEXT NOT NULL, " //Card cost, usually in format number + chromatic mana symbols
                            "sutype TEXT DEFAULT NULL, " //Supertype, NULL for most cards (can be Basic, Legendary, Snow, World)
                            "typing TEXT NOT NULL, " //Type (Land, Creature, Artifact, Enchantment, Planeswalker, Battle, Instant, Sorcery) and subtype, will be recorded divided by a comma ","
                            "stats TEXT DEFAULT NULL, " //Statistics, needed only if Creature, Vehicle ecc, in format S/T (Strenght/Toughness).
                            "wording TEXT DEFAULT NULL, " //Text on the card, NULL by default as many creatures only have a S/T field. IMPORTANT: during preliminary testing is ignored to quicken execution
                            "album_id INTEGER NOT NULL, " //Album where card is supposed to be saved, is FOREIGN KEY (see below) and matches the card's color identity
                            "FOREIGN KEY(album_id) REFERENCES Album(id));";
    const char* SData =     "CREATE TABLE IF NOT EXIST SData ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, " //Is primary key
                            "name TEXT NOT NULL, " //Card name, is foreign key
                            "c_set TEXT NOT NULL, " //Card set
                            "value DECIMAL DEFAULT NULL, " //Price, set to null in the first iteration
                            "UNIQUE(name, c_set), " //Each couple card name - card set is unique
                            "FOREIGN KEY(name) REFERENCES CData(name));";  

    //Error message for SQLite operations
    char* errormessage;

    //Check if the table Album already exists
    int exists;
    const char* FirstRun =  "SELECT EXISTS (SELECT 1 FROM sqlite_master WHERE type='table' AND name='Album');";
    int dbo = sqlite3_exec(db, FirstRun, existence, &exists, &errormessage);
    if(dbo != SQLITE_OK) {
        std::cerr << "SQL error: " << errormessage << std::endl;
        sqlite3_free(errormessage);
        return -111;
    }
    else {card_id = sqlite3_last_insert_rowid(db);
        //Debug check
        std::cout << "First Run control executed correctly" << std::endl;
    }

    //If the album table does not exist, create all tables, then populate Album table
    if(exists == 0) {

        //Execute all instructions in succession, building the various tables
        int dbo = sqlite3_exec(db, Albums, nullptr, nullptr, &errormessage); 
        if(dbo != SQLITE_OK) {
            //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
            std::cerr << "SQL error: " << errormessage << std::endl;
            sqlite3_free(errormessage);
            return -111;
        }
        else {
            //Debug check
            std::cout << "Album created successfully" << std::endl;
        }

        dbo = sqlite3_exec(db, Cells, nullptr, nullptr, &errormessage);
        if(dbo != SQLITE_OK) {
            //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
            std::cerr << "SQL error: " << errormessage << std::endl;
            sqlite3_free(errormessage);
            return -111;
        }
        else {
            //Debug check
            std::cout << "Cells created/found successfully" << std::endl;
        }

        dbo = sqlite3_exec(db, CellsC, nullptr, nullptr, &errormessage);
        if(dbo != SQLITE_OK) {
            //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
            std::cerr << "SQL error: " << errormessage << std::endl;
            sqlite3_free(errormessage);
            return -111;
        }
        else {
            //Debug check
            std::cout << "CellCard created/found successfully" << std::endl;
        }

        dbo = sqlite3_exec(db, Cards, nullptr, nullptr, &errormessage);
        if(dbo != SQLITE_OK) {
            //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
            std::cerr << "SQL error: " << errormessage << std::endl;
            sqlite3_free(errormessage);
            return -111;
        }
        else {
            //Debug check
            std::cout << "Cards created/found successfully" << std::endl;
        }

        dbo = sqlite3_exec(db, CData, nullptr, nullptr, &errormessage);
        if(dbo != SQLITE_OK) {
            //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
            std::cerr << "SQL error: " << errormessage << std::endl;
            sqlite3_free(errormessage);
            return -111;
        }
        else {
            //Debug check
            std::cout << "Card data created/found successfully" << std::endl;
        }


        dbo = sqlite3_exec(db, SData, nullptr, nullptr, &errormessage);
        if(dbo != SQLITE_OK) {
            //In case of errors, throws an error code and abort the function. Main app will also close to prevent data loss and errors.
            std::cerr << "SQL error: " << errormessage << std::endl;
            sqlite3_free(errormessage);
            return -111;
        }
        else {
            //Debug check
            std::cout << "Card-set data created/found successfully" << std::endl;
        }

        //Populate the Album table
        const char* Populate = "INSERT INTO Album (name) VALUES (?);";
        sqlite3_stmt* stmt;

        dbo = vprepare(db, Populate, &stmt);
        if(dbo != SQLITE_OK) {
            //Error handled by vprepare
            return -112;
        }

        std::string names[7] = {"w", "U", "B", "R", "G", "C", "M"};

        for(int i = 0; i < 7; i++) {
            dbo = text_bind(db, &stmt, 1, names[i]);
            if(dbo == -114) {
                //Signal the bind statement failed
                return -114;
            }

            if(sqlite3_step(stmt) != SQLITE_DONE) {
                //Signal step statement failed
                std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
                return -113;
            }
            else {
                //Debug Check
                std::cout << "Album " << names[i] << " inserted correctly" << std::endl;
            }
        }

    }
    else {
        //Debug check
        std::cout << "Tables already existing" << std::endl;
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

int make_card(sqlite3* db, std::string name, std::string set, int number) {
    //Check if the card exists in the system
    const char* sql1 = "SELECT album_id FROM CData WHERE name = ?;";
    //Album id (if found) is recorded
    int album = -1; //Default value -1, if not changed, signals the application to call record data (see #)

    //Statement handle
    sqlite3_stmt* stmt = nullptr;

    //Prepare statement
    int dbo = vprepare(db, sql1, &stmt);
    if(dbo != SQLITE_OK) {
        //Prepare error signal
        return -112;
    }

    //If preparation is successfull bind name for the searched
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    //Check if card exists
    dbo = sqlite3_step(stmt);
    if(dbo == SQLITE_ROW) {
        //Upon correctly finding the card the data are recorded 
        album = sqlite3_column_int(stmt, 1);

        //Debug check
        std::cout << "Card " << name << " found correctly" << std::endl;

        //Run find and insert
        dbo = find_and_insert(db, name, set, number);

    }
    else if(dbo == SQLITE_DONE) {
        //Operation executed correctly but card not found
        //Debug check 
        std::cout << "Card " << name << "not found" << std::endl;

        album = recorddata(db, name);
        //Handle results compatible with error -110 to return to main
    }
    else {
        //STEP error, returning error code
        //Debug check
        std::cout << "Error: CData search failed" << std::endl;
        return -113;
    }

    //Finalize statement
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

int find_and_insert(sqlite3* db, std::string name, std::string set, int number) {
    //Statement looking for free cells
    const char* sql1 = "SELECT id, filled FROM Cells WHERE name = ? and filled != 4 ORDER BY id ASC LIMIT 1;"; //May consider removing the LIMIT 1 if useful for improving efficiency
    sqlite3_stmt* stmt;

    //Cell id to use (if any available)
    int dbo = vprepare(db, sql1, &stmt);
    if(dbo != SQLITE_OK) {
        //Prepare error signal
        return -112;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    int cells = -1;
    int filling = -1;

    //Check if free cells exists
    dbo = sqlite3_step(stmt);
    if(dbo == SQLITE_ROW) {
        //Found free cells are recorded
        cells = sqlite3_column_int(stmt, 1);
        filling = sqlite3_column_int(stmt, 2);

        //Insert the cards
        number = insert(db, name, set, number, cells, filling);
        if(number < 0)
            return number;

    }
    else if(dbo == SQLITE_DONE) {
        //Operation executed correctly but card not found
        
        //* * *Create new cell, then insert
        //TESTING ONLY ALL CARDS ARE PLACED IN ALBUM 1
        dbo = make_cell(db, 1);

        //Insert the cards
        number = insert(db, name, set, number, cells, filling);
        if(number < 0)
            return number;

    }
    else {
        //STEP error, returning error code
        //Debug check
        std::cout << "Error: Cells search failed" << std::endl;
        return -113;
    }

    if(number > 0)
        dbo = find_and_insert(db, name, set, number);

    return dbo;
}

int insert(sqlite3* db, std::string name, std::string set, int number, int cell, int fillings) {
    //Create card using data passed
    const char* sql1 = "INSERT INTO Cards (name, c_set, cell_id) VALUES (?, ?, ?);";

    //Statement handle
    sqlite3_stmt* stmt;

    //prepare statement
    int dbo = vprepare(db, sql1, &stmt);
    if(dbo != SQLITE_OK) {
        return -112;
    }

    //Bind the name
    dbo = text_bind(db, &stmt, 1, name);
    if(dbo == -114) {
        //Signal the bind statement failed
        return -114;
    }

    //Bind the set
    dbo = text_bind(db, &stmt, 2, set);
    if(dbo == -114) {
        //Signal the bind statement failed
        return -114;
    }

    //Bind the cell id
    sqlite3_bind_int(stmt, 3, cell);

    //Determine if there are more cards to insert or free spaces in the current cell
    int upperbound = 4- fillings;

    //Smallest number determines the cycle (can't place more cards than there are, also can't fill more spaces than available)
    if (upperbound > number)
        upperbound = number;

    //Insert the cards to filling or untill all cards are located
    for(int i = 0; i < upperbound; i++) {
        //Insert the card and record the id of inserted card
        int card_id;
        if(sqlite3_step(stmt) == SQLITE_DONE) {
            //Card correctly inserted
            card_id = sqlite3_last_insert_rowid(db);

            //Debug check
            std::cout << "Inserted id " << card_id << std::endl;
        }
        else {
            //Debug check
            std::cout << "STEP error" << std::endl;
            return -113;
        }

        const char* sql2;

        //Depending on the filling value, a different statement is used to fill the cell
        switch (fillings) {
            case 0:
                //If cell is completely empty, fill the first space
                sql2 = "UPDATE CellCards SET c1 = ? WHERE id = ?;";
                break;
            case 1:
                sql2 = "UPDATE CellCards SET c2 = ? WHERE id = ?;";
                break;
            case 2:
                sql2 = "UPDATE CellCards SET c3 = ? WHERE id = ?;";
                break;
            case 3:
                sql2 = "UPDATE CellCards SET c4 = ? WHERE id = ?;";
                break;
            default:
                return -115;
        }

        //Index is updated to use the correct cell spot
        fillings++;

        //Extra control for data safety, I will probably remove it in release
        if (fillings > 4) {
            std::cerr << "Error: out of bound" << std::endl;
            return -115;
        }

        //* * *Add option that removes last inserted card for memory integrity if the data safety control fails

        //Cell statement handle
        sqlite3_stmt* stmt2;

        //prepare statement
        dbo = vprepare(db, sql2, &stmt2);
        if(dbo != SQLITE_OK) {
            return -112;
        }

        //Bind card id to be inserted in the appropriate cell position
        sqlite3_bind_int(stmt2, 1, card_id);
        sqlite3_bind_int(stmt2, 2, cell);

        if(sqlite3_step(stmt2) == SQLITE_DONE) {
            //Card correctly inserted
            //Debug check
            std::cout << "Inserted id " << card_id << std::endl;
        }
        else {
            //Debug check
            std::cout << "STEP error" << std::endl;
            return -113;
        }

        //* * *Update the cells table to mark that one more card is recorded there
        const char* sql3 = "UPDATE Cells SET filled = filled + 1 WHERE id = ? AND filled < 4;";
        sqlite3_stmt* stmt3;

        dbo = vprepare(db, sql3, &stmt3); 
        //Error handling
        if(dbo != SQLITE_OK) {
            //Error message thrown by vprepare, signal the error to main
            return -112;
        }

        sqlite3_bind_int(stmt3, 1, cell);

        //Check for cell update
        if(sqlite3_step(stmt3) == SQLITE_DONE) {
            //Card correctly inserted
            //Debug check
            std::cout << "Inserted id " << card_id << std::endl;
        }
        else {
            //Debug check
            std::cout << "STEP error" << std::endl;
            return -113;
        }

        number--;

        sqlite3_finalize(stmt2);
        sqlite3_finalize(stmt3);
    }

    sqlite3_finalize(stmt);

    return number;
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

    if(empty != "/")
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

    const char* sql = "INSERT INTO CData (name, color, cost, sutype, typing, stats, wording, album_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    int dbo = vprepare(db, sql, &stmt);
    if(dbo != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, colors.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, cost.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, sutype.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, stats.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, wording.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 8, id);

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

