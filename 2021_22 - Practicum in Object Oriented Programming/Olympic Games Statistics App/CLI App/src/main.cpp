// For Debugging:
 #define _CRT_SECURE_NO_WARNINGS
 #define _CRTDBG_MAP_ALLOC
 #include <stdlib.h>
 #include <crtdbg.h>


#include "App.hpp"


// ============================
// Program Functions Declarations:


void runOlympicGamesApp();


// ============================
// RUNNER:



int main()
{
    // freopen("testing_user_input.txt", "r", stdin);


    runOlympicGamesApp();
    //tmpTest();


    // _CrtDumpMemoryLeaks();

    return 0;
}



// ============================
// Program Functions:


void runOlympicGamesApp() {
    sale::App olympic_games_app;
    olympic_games_app.forEverLoop();  // never exits it
}
