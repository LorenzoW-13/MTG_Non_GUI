//Database Manager with queries
#ifndef DB_MANAGER_H
#define DB_MANAGER_H

//At first launch creates the tables for the cards database
int tables(sqlite3* db) {
    const char* Albums =    "CREATE TABLE IF NOT EXISTS Album ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "name TEXT NOT NULL, "
                            "UNIQUE(name));";
    const char* Cells =     "CREATE TABLE IF NOT EXISTS Cells ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "name TEXT, number INTEGER NOT NULL, "
                            "album_id INTEGER, "
                            "FOREIGN KEY(album_id) REFERENCES Album(id), "
                            "UNIQUE(number, album_id));";
    const char* CellsC =    "CREATE TABLE IF NOT EXISTS CellCards ("
                            "id INTEGER PRIMARY KEY, "
                            "c1 INTEGER DEFAULTS NULL, "
                            "c2 INTEGER DEFAULTS NULL, "
                            "c3 INTEGER DEFAULTS NULL, "
                            "c4 INTEGER DEFAULTS NULL, "
                            "FOREIGN KEY (id) REFERENCES Cells(id), "
                            "FOREIGN KEY c1 REFERENCES Cards(id), "
                            "FOREIGN KEY c2 REFERENCES Cards(id), "
                            "FOREIGN KEY c3 REFERENCES Cards(id), "
                            "FOREIGN KEY c4 REFERENCES Cards(id);)";
    const char* Cards =     "CREATE TABLE IF NOT EXISTS Cards ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "name TEXT NOT NULL, "
                            "color TEXT, "
                            "set TEXT NOT NULL, "
                            "cost TEXT NOT NULL, "
                            "price DECIMAL DEFAULT NULL, "
                            "cell_id INT NOT NULL, "
                            "FOREIGN KEY (cell_id) REFERENCES CellCards(id));";
}

#endif