//database manager

//header
#include "Libraries/db_manager.h"

//Libraries
#include <vector>

//Changing version: the finalize is to be executed by the caller, the close is to be executed by main or a direct delegate (like error handling)

int card_rec(sqlite3* db, std::string name, std::string set, int cell_id) {
    //Insert statement and handle
    const char* sql = "INSERT INTO Cards (name, c_set, cell_id) VALUES (?, ?, ?);";
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

    sqlite3_finalize(stmt);

    //Operation successfull, record the inserted card id
    int c_id = sqlite3_last_insert_rowid(db);


    std::cout << "ID inserted: " << c_id << std::endl;

    //Return the id
    return c_id;

}

int int_bind(sqlite3* db, sqlite3_stmt** stmt, int position, int value) {
    //If binding for the first column of a statement, reset the statement as a safety measure
    if(position == 1) {
        sqlite3_reset(*stmt);
    }

    //Execute the integer binding routine
    if(sqlite3_bind_int(*stmt, position, value) != SQLITE_OK) {
        //Signal the binding has not succeeded and throw the error message
        std::cerr << "Binding error: " << sqlite3_errmsg(db) << std::endl;
        return -114;
    }

    //If binding is successfull, signal the success
    return SQLITE_OK;
}

int text_bind(sqlite3* db, sqlite3_stmt** stmt, int position, std::string value) {
    //If binding for the first column of a statement, reset the statement as a safety measure
    if(position == 1) {
        sqlite3_reset(*stmt);
    }

    //Execute the string binding rutine
    if(sqlite3_bind_text(*stmt, position, value.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
        //Signal the binding has not succeeded and throw the error message
        std::cerr << "Binding error: " << sqlite3_errmsg(db) << std::endl;
        return -114;
    }

    return SQLITE_OK;
}

int vprepare(sqlite3* db, const char* sql, sqlite3_stmt** stmt) {
    //Prepare the statement
    if(sqlite3_prepare_v2(db, sql, -1, stmt, nullptr) != SQLITE_OK) {
        //Signal preparation has failed and throw error message
        std::cerr << "Binding error: " << sqlite3_errmsg(db) << std::endl;
        return -112;
    }

    //If preparation is successfull, signal the success
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

int new_cell(sqlite3* db, int album_id) {
    //Get the highest cell number in the needed album
    const char* sql1 = "SELECT number FROM Cells WHERE album_id = ? ORDER BY number DESC LIMIT 1;";
    
    sqlite3_stmt* stmt = nullptr;

    //Prepare the stetement
    if(vprepare(db, sql1, &stmt) != SQLITE_OK) {
        //Prepare Error
        return -112;
    }

    if(int_bind(db, &stmt, 1, album_id) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    int cell_n;
    int dbo = sqlite3_step(stmt);
    //Execute the step
    if(dbo == SQLITE_ROW) {
        //Cell row found
        //The cell to add has number equal to highest existing cell + 1
        cell_n = sqlite3_column_int(stmt, 0) + 1;
    }
    else if(dbo == SQLITE_DONE) {
        //The album is still empty
        char empty_album;
        std::cout << "The album appears empty. Please, confirm the cell thats about to be created is the first one, then confirm. Y/N. . .\t";
        std::cin >> empty_album;

        if(empty_album == 'Y' || empty_album == 'y') {
            cell_n = 1;
        }
        else {
            sqlite3_finalize(stmt);
            std::cerr << "\tAlbum is confirmed to be not empy, however the query reads it as empty.\n\tPlease check the query in the source code.\n\tThe app will shut down to prevent errors and data loss.";
            return -113;
        }
    }
    else {
        //Step Error
        sqlite3_finalize(stmt);
        return -113;
    }

    sqlite3_finalize(stmt);

    //Create a new free cell in memory
    const char* sql2 = "INSERT INTO Cells (album_id, number) VALUES (?, ?);";

    //Prepare the stetement
    if(vprepare(db, sql2, &stmt) != SQLITE_OK) {
        //Prepare Error
        return -112;
    }

    if(int_bind(db, &stmt, 1, album_id) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 2, cell_n) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    //Newly created cell's assigned id
    int id;

    if(sqlite3_step(stmt) == SQLITE_DONE) {
        id = sqlite3_last_insert_rowid(db);

        //Debug check
        std::cout << "Cella di prova; id: " << id << std::endl;
    }
    else{
        //Step Error
        sqlite3_finalize(stmt);
        return -113;
    }

    sqlite3_finalize(stmt);

    //Create the cell-card bridge table entry for the newly created cell
    const char* sql3 = "INSERT INTO CellCards (id) VALUES (?);";

    //Prepare the stetement
    if(vprepare(db, sql3, &stmt) != SQLITE_OK) {
        //Prepare Error
        return -112;
    }

    if(int_bind(db, &stmt, 1, id) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(sqlite3_step(stmt) != SQLITE_DONE) {
        //Step Error
        sqlite3_finalize(stmt);
        return -113;
    }

    sqlite3_finalize(stmt);
    
    return id;    
}

int rec_data_c(sqlite3* db, std::string name) {
    //Record the data needed to save a new card
    std::string colors;
    std::string cost;
    char stype_check;
    int c_check = 0;
    std::string sutype;
    std::string type;
    std::string wording;
    int album_id;
    std::string stats;
    int constitution;

    //NOTE: ALL THE SECTION BELOW IS FOR MANUAL TESTING ONLY, IN THE GUI RELEASE EVERYTHING ABOVE LINE "//GUI-VIABLE CODE" IS TO BE REPLACED WITH THE CORRECT GUI CODE
    //For non GUI version data insertion is done manually
    std::cout << "Insert colors in the format W, U, B, R, G; Use C for colorless (and devoid): ";
    std::cin >> colors;
    std::cout << "Has the card supertypes? Y/N. . .\t";
    std::cin >> stype_check;
    
    //If there is a supertype, ask for supertype and flag for supertype existance
    if(stype_check == 'Y' || stype_check == 'y') {
        std::cout << "List all the supertypes: ";
        std::getline(std::cin, sutype);
    }

    std::cout << "List all types, followed by all subtypes: ";
    std::getline(std::cin, type);

    if(colors == "W") {
        album_id = 1;
    }
    else if(colors == "U") {
        album_id = 2;
    }
    else if(colors == "B") {
        album_id = 3;
    }
    else if(colors == "R") {
        album_id = 4;
    }
    else if(colors == "G") {
        album_id = 5;
    }
    else if(colors == "C") {
        album_id = 6;
    }
    else {
        album_id = 7;
    }

    if(type.find("Creature") != std::string::npos || type.find("creature") != std::string::npos) {
        std::cout << "Card inserted is a Creature, provide strenght: ";
        std::cin >> stats;
        std::cout << "\nconstitution: ";
        std::cin >> constitution;
        stats.append(std::to_string(constitution));
        c_check == 1;
    }
    else if(type.find("Vehicle") != std::string::npos || type.find("vehicle") != std::string::npos) {
        std::cout << "Card inserted is a Vehicle, provide strenght: ";
        std::cin >> stats;
        std::cout << "\nconstitution: ";
        std::cin >> constitution;
        stats.append(std::to_string(constitution));
        c_check == 1;
    }

    std::cout << "Insert the card wording. If there is none, type \"blank card\": ";
    std::getline(std::cin, wording);

    //GUI-VIABLE CODE
    if((stype_check == 'y' || stype_check == 'Y') && c_check) {
        int dbo = record_stats(db, name, colors, cost, sutype, type, stats, wording, album_id);
        if(dbo != SQLITE_OK) {<< dbo;
            return dbo;
        }
    }
    else if((stype_check == 'y' || stype_check == 'Y') && !c_check) {
        int dbo = record_nostats(db, name, colors, cost, sutype, type, wording, album_id);
        if(dbo != SQLITE_OK) {
            std::cout << "Error " << dbo;
            return dbo;
        }
    }
    else if((stype_check != 'y' && stype_check != 'Y') && c_check) {
        int dbo = record_nosup(db, name, colors, cost, sutype, type, wording, album_id);
        if(dbo != SQLITE_OK) {
            std::cout << "Error " << dbo;
            return dbo;
        }
    }
    else if((stype_check == 'y' || stype_check == 'Y') && !c_check) {
        int dbo = record_nostats_nosup(db, name, colors, cost, type, wording, album_id);
        if(dbo != SQLITE_OK) {
            std::cout << "Error " << dbo;
            return dbo;
        }
    }
    else {
        std::cout << "Error of unknown type in input of data, returning generic -110 error." << std::endl;
        return -110;
    }

    return album_id;
}

int record_stats(sqlite3* db, std::string name, std::string color, std::string cost, std::string sutype, std::string type, std::string stats, std::string wording, int album_id) {
    //Record the card data, insert into the database, then return the succes or error code
    const char* sql = "INSERT INTO CData (name, color, cost, sutype, typing, stats, wording, album_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

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

    if(text_bind(db, &stmt, 1, color) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, cost) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, sutype) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, type) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, stats) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, wording) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 3, album_id) != SQLITE_OK) {
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

    sqlite3_finalize(stmt);

    return SQLITE_OK;
}

int record_nostats(sqlite3* db, std::string name, std::string color, std::string cost, std::string sutype, std::string type, std::string wording, int album_id) {
    //Record the card data, insert into the database, then return the succes or error code
    const char* sql = "INSERT INTO CData (name, color, cost, sutype, typing, wording, album_id) VALUES (?, ?, ?, ?, ?, ?, ?);";

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

    if(text_bind(db, &stmt, 1, color) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, cost) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, sutype) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, type) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, wording) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 3, album_id) != SQLITE_OK) {
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

    sqlite3_finalize(stmt);

    return SQLITE_OK;
}

int record_nostats_nosup(sqlite3* db, std::string name, std::string color, std::string cost, std::string type, std::string wording, int album_id) {
    //Record the card data, insert into the database, then return the succes or error code
    const char* sql = "INSERT INTO CData (name, color, cost, typing, wording, album_id) VALUES (?, ?, ?, ?, ?, ?);";

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

    if(text_bind(db, &stmt, 1, color) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, cost) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, type) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, wording) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 3, album_id) != SQLITE_OK) {
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

    sqlite3_finalize(stmt);

    return SQLITE_OK;
}

int record_nosup(sqlite3* db, std::string name, std::string color, std::string cost, std::string type, std::string stats, std::string wording, int album_id) {
    //Record the card data, insert into the database, then return the succes or error code
    const char* sql = "INSERT INTO CData (name, color, cost, typing, stats, wording, album_id) VALUES (?, ?, ?, ?, ?, ?, ?);";

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

    if(text_bind(db, &stmt, 1, color) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, cost) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, type) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, stats) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(text_bind(db, &stmt, 1, wording) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 3, album_id) != SQLITE_OK) {
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

    sqlite3_finalize(stmt);

    return SQLITE_OK;
}

int not_present_card(sqlite3* db, std::string name, std::string set, int number) {
    //The premise of this function is that the card has never been inserted before
    //Call the record data routine
    int album_id = rec_data_C(db, name);

    //Create the new cell to pass to the card record routine
    int cell_id = new_cell(db, album_id);

    //Record the new card in the database
    int card_id = card_rec(db, name, set, cell_id);

    //* * *Here goes the function to set the cell destination.
}

int insert_into(sqlite3* db, int cell_id, int card_id, int position) {
    //Evaluate position, deciding the correct cell slot to insert the card into
    const char* sql;

    switch(position) {
        case 0:
            sql = "UPDATE CellCards SET c1 = ? WHERE id = ?;";
            break;
        case 1:
            sql = "UPDATE CellCards SET c2 = ? WHERE id = ?;";
            break;
        case 2:
            sql = "UPDATE CellCards SET c3 = ? WHERE id = ?;";
            break;
        case 3:
            sql = "UPDATE CellCards SET c4 = ? WHERE id = ?;";
            break;
        default:
            return -115;
    }

    sqlite3_stmt* stmt;

    if(vprepare(db, sql, &stmt) != SQLITE_OK) {
        //Prepare Error
        return -112;
    }

    if(int_bind(db, &stmt, 1, card_id) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 2, cell_id) != SQLITE_OK) {
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

    sqlite3_finalize(stmt);

    return SQLITE_DONE;
}

int remove_card(sqlite3* db, int card_id) {
    //Remove statement
    const char* sql = "DELETE FROM Cards WHERE id = ?;";
    sqlite3_stmt* stmt;

    if(vprepare(db, sql, &stmt) != SQLITE_OK) {
        //Prepare Error
        return -112;
    }

    if(int_bind(db, &stmt, 1, card_id) != SQLITE_OK) {
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

    sqlite3_finalize(stmt);

    return SQLITE_DONE;
}

int fill_update(sqlite3* db, int cell_id, int fill) {
    //Statement to update the cell's fill parameter
    const char* sql = "UPDATE Cells SET filled = ? WHERE id = ?;";
    sqlite3_stmt* stmt;

    if(vprepare(db, sql, &stmt) != SQLITE_OK) {
        //Prepare Error
        return -112;
    }

    if(int_bind(db, &stmt, 1, fill) != SQLITE_OK) {
        //Bind Error
        sqlite3_finalize(stmt);
        return -114;
    }

    if(int_bind(db, &stmt, 2, cell_id) != SQLITE_OK) {
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

    sqlite3_finalize(stmt);

    return SQLITE_DONE;
}

//Function to record a newly created card into a cell
int insert_to_cell(sqlite3* db, std::string name, std::string set, int cell_id, int number, int counter) {
    //Create a new card with the given cell id
    int card_id = card_rec(db, name, set, cell_id);
    if(card_id < 0) {
        //If card_id is less than 0 an error occured during the recording process. The error code is transmitted upstream, shutting the processes to prevent data loss, 
        //up to the point where is evaluated and fixed
        std::cerr << "Error of type " << card_id << " has occurred during the recording of a card. Please check the database manually to confirm flawed data have correctly been removed." << std::endl;
        return card_id;
    }

    //Record the newly created card id into the cell-card bridge table
    int dbo = insert_into(db, cell_id, card_id, counter);
    if(dbo != SQLITE_DONE) {
        //Error while inserting a card into its spot
        std::cerr << "Error of type " << dbo << " has occured while inserting the card into its cell's slot. Please remove card " << card_id << " from the binder and verify it has correctly been removed from the card table.Also, please, verify that cell " << cell_id << " filling parameter is set to " << counter << std::endl;

        //update the filled parametr with the successfull insertions before closing the app
        fill_update(db, cell_id, counter);

        //Card is removed to avoid leaving unassigned and untracked cards
        remove_card(db, card_id);

        //Return the error code upstream for evaluation
        return dbo;
    }

    //Increase the result and decrease the number, as the operation has been successfull. This means there is one less card to allocate and one less free spot in the 
    counter++;
    number--;

    if(counter < 4 && number > 0)
        number = insert_to_cell(db, name, set, cell_id, number, counter);

    if(counter >= 4 || number <= 0) {
        int dbo = fill_update(db, cell_id, counter);
        if(dbo != SQLITE_DONE){
            //Error while fixing the filled value
            std::cerr << "Error of type " << dbo << " has occured while updating cell's filling value: " << counter << ", please update the value manually. " << number << " cards have not yet been recorded in the system. The application is shutting down to prevent further data loss." << std::endl;

            return dbo;
        }
        else {
            //Let the end user know where the cards need to be placed in the physical binder
            std::cout << counter << " cards recorded and inserted in cell " << cell_id << std::endl;
        }
    }

    return number;
}
//Insert the cards into cells
/*

    get the new cell id, the number of cards to load, and a counter defaulted to 0

    call make new card, get in return an id

    call record into cell, passing the counter as position

    increment the counter if everything checks out
    decrease the number as well

    if number is > 0 AND counter is < 4 do: 
        call this same function again. counter is counter.

    if number is <= 0 OR counter is >= 4
        NOTA: add a "flawed" flag that prompts for a manual check? or better add a vector holding all the newly inserted cards id, in case of error delete the last cell and all 
        the cards inserted and show an error message "CARDS NOT RECORDED; DATABASE ERROR" to user.

        update the value of the cell "filling" to counter's value

    return number of eleemnt left
*/



























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
    const char* SData =     "CREATE TABLE IF NOT EXISTS SData ("
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
        std::cout << "1" << std::endl;
        sqlite3_free(errormessage);
        return -111;
    }
    else {
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
            std::cout << "2" << std::endl;
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
            std::cout << "3" << std::endl;
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
            std::cout << "4" << std::endl;
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
            std::cout << "5" << std::endl;
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
            std::cout << "6" << std::endl;
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
            std::cout << "7" << std::endl;
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
            //Error handled by vprepare//Changing version: the finalize is to be executed by the caller, the close is to be executed by main or a direct delegate (like error handling)
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
                std::cout << "8" << std::endl;
                return -113;
            }
            else {
                //Debug Check
                std::cout << "Album " << names[i] << " inserted correctly" << std::endl;
            }
        }

        sqlite3_finalize(stmt);

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
        dbo = find_and_insert(db, name, set, number, album);

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

int find_and_insert(sqlite3* db, std::string name, std::string set, int number, int album) {
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
        dbo = find_and_insert(db, name, set, number, album);

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
            sqlite3_finalize(stmt);
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
                sqlite3_finalize(stmt);
                return -115;
        }

        //Index is updated to use the correct cell spot
        fillings++;

        //Extra control for data safety, I will probably remove it in release
        if (fillings > 4) {
            std::cerr << "Error: out of bound" << std::endl;
            sqlite3_finalize(stmt);
            return -115;
        }

        //* * *Add option that removes last inserted card for memory integrity if the data safety control fails

        //Cell statement handle
        sqlite3_stmt* stmt2;

        //prepare statement
        dbo = vprepare(db, sql2, &stmt2);
        if(dbo != SQLITE_OK) {
            sqlite3_finalize(stmt);
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
            sqlite3_finalize(stmt);
            sqlite3_finalize(stmt2);
            return -113;
        }

        //* * *Update the cells table to mark that one more card is recorded there
        const char* sql3 = "UPDATE Cells SET filled = filled + 1 WHERE id = ? AND filled < 4;";
        sqlite3_stmt* stmt3;

        dbo = vprepare(db, sql3, &stmt3); 
        //Error handling
        if(dbo != SQLITE_OK) {
            //Error message thrown by vprepare, signal the error to main
            sqlite3_finalize(stmt);
            sqlite3_finalize(stmt2);
            return -112;
        }

        sqlite3_bind_int(stmt3, 1, cell);

        //Check for cell update
        if(sqlite3_step(stmt3) == SQLITE_DONE) {
            //Card correctly inserted
            //Debug check
            std::cout << "Inserted id " << card_id << std::endl;
            number--;
        }
        else {
            //Debug check
            std::cout << "STEP error" << std::endl;
            sqlite3_finalize(stmt);
            sqlite3_finalize(stmt2);
            sqlite3_finalize(stmt3);
            return -113;
        }

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
        std::cout << "9" << std::endl;
        return -2;
        
    }

    sqlite3_finalize(stmt);

    return id;
}

