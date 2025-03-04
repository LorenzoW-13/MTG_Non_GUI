# MTG
MTG Collection Manager APP

- INDEX:  NOTA: CORREGGI ERRORI DATI DALLA LUNGHEZZA DELL?INDICE
     1) - Compiler Instructions (rows # - #)
     2) - Album(s) organization (rows # - #)
     3) - Internal error handling (rows # - #)

Compiler instruction:
1) - Check for the sqlite3 library files (sqlite3.h; sqlite3.c). If missing download them from https://sqlite.org/download.html under Source Code (file sqlite-src-3490100.zip)
2) - Extract the content of .zip inside the "Libraries" folder
3) - Run
     - gcc -c Libraries/sqlite3.c -o sqlite3.o
     - g++ main.cpp db_manager.cpp sqlite3.o -Wall -o MTGCollector
     - ./MTGCollector

Album(s) organization
 - The first release is designed on a library of 7 albums (one fo each color + 1 for colorless and 1 for multi-color) which are loaded in advance at the first start of the program. I might add a menu allowing the user to define its own personal criteria to add different binders, but since this is primarily a personal project, and nobody will ever read this document I'm typing (which, by the way, i'm writing out just as an exercise), this is unlikely. Also, I think the seven binders disposition is the best disposition, and the only plausible modification I'm going to make will be the division in sections by converted mana cost.

 Internal errors handling
  - If an `execute` instruction fails the running function returns `-111`, which is returned upstream to the `main` to stop the application. This way, if any I/O operation fails no data is lost. Same happens for `prepare`, returning `-112`, `step` returning `-113` and `bind` (either `TEXT` or `INT`) returning `-114` 