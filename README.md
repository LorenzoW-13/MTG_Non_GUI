# MTG
MTG Collection Manager APP

Compiler instruction:
1) - Check for the sqlite3 library files (sqlite3.h; sqlite3.c). If missing download them from https://sqlite.org/download.html under Source Code (file sqlite-src-3490100.zip)
2) - Extract the content of .zip inside the "Libraries" folder
3) - Run
     - gcc -c Libraries/sqlite3.c -o sqlite3.o
     - g++ main.cpp db_manager.cpp sqlite3.o -Wall -o MTGCollector
     - ./MTGCollector
