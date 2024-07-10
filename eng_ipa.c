#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
┌────┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ {[│ }]│ |\│
├────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴───┤
│Caps │ A │ S │ D │ F.│ G │ H │.J │ K │ L │ :;│ "'│ Enter│
├─────┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴──────┤
│Shift  │ Z │ X │ C │ V │ B │ N │ M │ <,│ >.│ ?/│  Shift │
└───────┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴────────┘
*/

typedef struct vowel {
	wchar_t* utf8;
	char*    ascii;
	char*    sound;
	char     key;
} vowel;

#define VOWEL_COUNT 15
vowel ipa_vowels[VOWEL_COUNT] = {
    { L"ə", "of", "of.mp3" },
    { L"ɹ", "work", "work.mp3" },
    { L"ɪ", "with", "with.mp3" },
    { L"i", "be", "be.mp3" },
    { L"ɛ", "get", "get.mp3" },
    { L"æ", "map", "map.mp3" },
    { L"u", "who", "who.mp3" },
    { L"ɑ", "on", "on.mp3" },
    { L"ɔ", "more", "more.mp3" },
    { L"ʊ", "good", "good.mp3" },
    { L"eɪ", "they", "they.mp3" },
    { L"aɪ", "by", "by.mp3" },
    { L"oʊ", "most", "most.mp3" },
    { L"aʊ", "how", "how.mp3" },
    { L"ɔɪ", "point", "point.mp3" },
};

// @vowel_exists
int vowel_exists(char* user_string)
{
    for (int i = 0; i < 15; i++)
    {
	    if (strcmp(ipa_vowels[i].ascii, user_string) == 0)
                     	    return 1;
    }
    return 0;
}

// @print
void print(int random_int, int quiz_flags)
{
    mvaddwstr(0, 0, L"type 'h' to hide/show ipa, 'q' to quit");

    for (int i = 0; i < VOWEL_COUNT; i++)
    {
            mvaddwstr(2 + i, 0, quiz_flags ? ipa_vowels[i].utf8 : L"?");
            mvaddstr(2 + i, 3, ipa_vowels[i].ascii);
    }

    mvaddwstr(17, 0, ipa_vowels[random_int].utf8);
    printw("\n\n");
}

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

    srand(time(0));

    char user_string[128];
    int random_int = rand() % VOWEL_COUNT;
    int new_random_int = random_int;
    int quiz_flags = 0;
    while (strcmp(user_string, "q") != 0)
    {

	    clear();
            print(random_int, quiz_flags);

            getnstr(user_string, 127);
            if (strcmp(user_string, "h") == 0)
	    {
		    quiz_flags = quiz_flags ? 0 : 1;
	    }
	    if (vowel_exists(user_string) == 0)
            {
	            result = ma_engine_play_sound(&engine, "Abstract2.mp3", NULL);
		    if (result != MA_SUCCESS)
			    return -1;
	    }
	    else
	    {
		    if (strcmp(user_string, ipa_vowels[random_int].ascii) == 0)
		    {
	                    new_random_int = random_int;
	                    while (new_random_int == random_int)
                                new_random_int = rand() % VOWEL_COUNT;
	                    random_int = new_random_int;

	                    result = ma_engine_play_sound(&engine, ipa_vowels[random_int].sound, NULL);
	                    if (result != MA_SUCCESS)
	                            return -1;
		    }
		    else
		    {
		            result = ma_engine_play_sound(&engine, "African1.mp3", NULL);
		            if (result != MA_SUCCESS)
		                    return -1;
		    }
	    }

            refresh();
    }


    endwin();
    ma_engine_uninit(&engine);

    return 0;
}
