#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

/*
          ╭───╮      
    Row   │ R │      🡨   🡫   🡩   🡪
          ╰┬──┴╮   ╭───┬───┬───┬───╮
   Free    │ F.│   │ H │.J │ K │ L │
         ╭─┴─┬─┴─╮ ╰───┴───┴───┴───╯
 Column  │ C │ V │  View
         ╰───┴───╯
┌─────────────────────┐┌──── XW 
│ A moose crossed the ││     │
│ road and he was the NQ     │
│ biggest I ever saw. ││     │
└─────────────────────┘ MW ──┘
┌──┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬─────┐
│`~│!1 │@2 │#3 │$4 │%5 │ 6^│ 7&│ 8*│ 9(│ 0)│ -_│ +=│ Bksp│
├──┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬───┤
│Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ {[│ }]│ |\│
├────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴───┤                           
│Caps │ A │ S │ D │ F.│ G │ H │.J │ K │ L │ :;│ "'│ Enter│
├─────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴──────┤
│Shift  │ Z │ X │ C │ V │ B │ N │ M │ <,│ >.│ ?/│  Shift │
├────┬──┴┬──┴──┬┴───┴───┴───┴───┴───┴──┬┴───┴┬──┴──┬─────┤
│Ctrl│Mod│ Alt │                       │ Alt │     │ Ctrl│
└────┴───┴─────┴───────────────────────┴─────┴─────┴─────┘
*/
// @main
int main(int argc, char** argv)
{
    argv[2] = "list.txt";

    ma_engine engine;
    ma_result result;
    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS)
    	    return -1;

    setlocale(LC_ALL, "");

    initscr();
    refresh();


    printw("%s\n", argv[2]);
    FILE* file_ptr;
    file_ptr = fopen(argv[2], "r");
    if (file_ptr == NULL)
    {
	    printw("could not open %s!\n", argv[2]);
	    exit(0);
    }

    char line[256];
    fgets(line, sizeof(line), file_ptr);
    fgets(line, sizeof(line), file_ptr);

    fgets(line, sizeof(line), file_ptr);
    printw("%s\n", strtok(line, "\t"));
    printw("%s\n", strtok(NULL, "\t"));
    
    refresh();
	
    fgets(line, sizeof(line), file_ptr);
    printw("%s\n", strtok(line, "\t"));
    printw("%s\n", strtok(NULL, "\t"));

    fclose(file_ptr);
    
    result = ma_engine_play_sound(&engine, "bruh.mp3", NULL);
    if (result != MA_SUCCESS)
	    return -1;

    getch(); 
    endwin();

    ma_engine_uninit(&engine);

    return 0;
}
