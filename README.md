# MTG
MTG Collection Manager APP

- INDEX:  NOTA: CORREGGI ERRORI DATI DALLA LUNGHEZZA DELL?INDICE
     1) - Compiler Instructions (rows # - #)
     2) - Album(s) organization (rows # - #)
     3) - Internal error handling (rows # - #)
     4) - Unused tables (rows # - #)

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
  - If an `execute` instruction fails the running function returns `-111`, which is returned upstream to the `main` to stop the application. This way, if any I/O operation fails no data is lost. Same happens for `prepare`, returning `-112`, `step` returning `-113`, `bind` (either `TEXT` or `INT`) returning `-114`. If the cell "filling" is out of bound, the code gives off an error `-115`. This should never happen after the testing and debug phase, however the error is not removed for possible future diagnostics prposes. If there is an error which is non identifiable, program returns error `-110`, `unknown`.

  Unused tables:
  - There is a `SData` table in the database structure, that is currently not in use. Like some other parts of the application, these unused functions and portions of code are the foundations of future features. Specifically:
     - The `SData` will be used toghether with an API taking card value data on a monthly basis, updating the value of the collection. This is currently EXTREMELY impractical as the update would need to be manual, reason  why the table, while present, is never used.

Future additions:
 - This application is primarily a tool to organize my own relatively small collection of cards, and also a way for me to showcase my coding abilities since it's aimed at being part of my curriculum vitae. For this reason, some optimization steps that are not essential are, for now, left behind. For example, indexing the various table would significantly improve performances of I/O operations, and would be essential for collections of hundreds of thousands of cards. If this application were to be ever released for public use, I'll make sure to include this efficiency improvements. Also, one of the features I'm planning for a possible future release of the app would be the ability to personalize the structure and shape of the collection (number and definition of binders, possibility to add subsections to the binders ecc). This current version of the app is intended for personal use with my small collection and to showcase my knowledge of c++ and sqlie3, and for this reason offers limited features, as described above.