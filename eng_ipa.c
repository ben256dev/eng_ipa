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
┌────────────────── XW ┌─────┐
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
        ┌───┐           ┌───┬───┐
        │eɪᵉ│           │ᵤaɪ│ᵢɔɪ│
 ┌───┬──┴┬──┴┬───┬───┬──┴┬──┴┬──┴┬───┬───┐
 │iᵃ │ɪˢ │ɛᵈ │ᶠə.│ɹᵍ │ʰæ │ʲɑ.│ᵏɔ │ˡu │ ʊ │
 └───┴───┴─┬─┴─┬─┴───┴───┴─┬─┴─┬─┴───┴───┘
           │oʊᶜ│           │ᵐaʊ│
           └───┘           └───┘
*/

// @quiz flags
#define QUIZ_FLAGS_SHOW_IPA_KEY_BIT    1
#define QUIZ_FLAGS_SHOW_OPTIONS_BIT    2
#define QUIZ_FLAGS_SINGLE_KEY_MODE_BIT 4
int quiz_flags = QUIZ_FLAGS_SHOW_IPA_KEY_BIT | QUIZ_FLAGS_SHOW_OPTIONS_BIT | QUIZ_FLAGS_SINGLE_KEY_MODE_BIT;

typedef struct allophone {
	wchar_t* ipa_str;
	char*    ascii_str;
	char*    sound_file_path_str;
	int      flags;
	char     key;
} allophone;

// @allophones
#define ALLOPHONE_COUNT 15
const allophone allophones[ALLOPHONE_COUNT] = {
    { L"ə", "of", "of.mp3", 0, 'f' },
    { L"ɹ", "work", "work.mp3", 0, 'g' },
    { L"ɪ", "with", "with.mp3", 0, 's' },
    { L"i", "be", "be.mp3", 0, 'a' },
    { L"ɛ", "get", "get.mp3", 0, 'd' },
    { L"æ", "map", "map.mp3", 0, 'h' },
    { L"u", "who", "who.mp3", 0, 'l' },
    { L"ɑ", "on", "on.mp3", 0, 'j' },
    { L"ɔ", "more", "more.mp3", 0, 'k' },
    { L"ʊ", "good", "good.mp3", 0, ';' },
    { L"eɪ", "they", "they.mp3", 0, 'e' },
    { L"aɪ", "by", "by.mp3", 0, 'u' },
    { L"oʊ", "most", "most.mp3", 0, 'c' },
    { L"aʊ", "how", "how.mp3", 0, 'm' },
    { L"ɔɪ", "point", "point.mp3", 0, 'i' },
};

// @vowel exists
int vowel_exists(char* user_string)
{
    for (int i = 0; i < 15; i++)
    {
	    if (strcmp(allophones[i].ascii_str, user_string) == 0)
                     	    return 1;
    }
    return 0;
}

// @print original
void print_original(WINDOW* win, int quiz_flags)
{
    box(win, 0, 0);

    int rows = getmaxy(win);
    for (int i = 0; i < ALLOPHONE_COUNT ; i++)
    {
	    int y_offset = rows / 2 - ALLOPHONE_COUNT / 2 + i;
            mvwaddwstr(win, y_offset, 2, (quiz_flags & QUIZ_FLAGS_SHOW_IPA_KEY_BIT) ? allophones[i].ipa_str : L"? ");
            mvwaddstr(win, y_offset, 5, allophones[i].ascii_str);
    }

    wrefresh(win);
}

// @print keyboard
void print_keyboard(WINDOW* win)
{
    box(win, 0, 0);

    int cols = getmaxx(win);
    int x_offset = cols / 2 - 20;
    mvwaddwstr(win, 1, x_offset, L"       ┌───┐           ┌───┬───┐         ");
    mvwaddwstr(win, 2, x_offset, L"       │eɪᵉ│           │ᵤaɪ│ᵢɔɪ│         ");
    mvwaddwstr(win, 3, x_offset, L"┌───┬──┴┬──┴┬───┬───┬──┴┬──┴┬──┴┬───┬───┐");
    mvwaddwstr(win, 4, x_offset, L"│iᵃ │ɪˢ │ɛᵈ │ᶠə.│ɹᵍ │ʰæ │ʲɑ.│ᵏɔ │ˡu │ ʊ │");
    mvwaddwstr(win, 5, x_offset, L"└───┴───┴─┬─┴─┬─┴───┴───┴─┬─┴─┬─┴───┴───┘");
    mvwaddwstr(win, 6, x_offset, L"          │oʊᶜ│           │ᵐaʊ│          ");
    mvwaddwstr(win, 7, x_offset, L"          └───┘           └───┘          ");

    wrefresh(win);
}

// @print input
#define CONTROLS_OFFSET_X -6
void print_input(WINDOW* win, int selected_vowel, int quiz_flags)
{
    box(win, 0, 0);

    selected_vowel %= ALLOPHONE_COUNT;

    int rows = getmaxy(win);
    int cols = getmaxx(win);
    int y_center = rows / 2;
    int x_center = cols / 2;

    mvwprintw(win, y_center, x_center + CONTROLS_OFFSET_X, "  ");
    mvwprintw(win, y_center, x_center + CONTROLS_OFFSET_X, "%-2ls", allophones[selected_vowel].ipa_str);

    if (quiz_flags & QUIZ_FLAGS_SHOW_OPTIONS_BIT) 
    {
	    wattron(win, A_REVERSE);
	    mvwaddwstr(win, rows - 6, cols - 18, L"                  ");
	    mvwaddwstr(win, rows - 5, cols - 18, L" q : quit         ");
	    mvwaddwstr(win, rows - 4, cols - 18, L" h : hide ipa key ");
	    mvwaddwstr(win, rows - 3, cols - 18, L" m : switch modes ");
	    mvwaddwstr(win, rows - 2, cols - 18, L" ? : controls     ");
	    mvwaddwstr(win, rows - 1, cols - 18, L"                  ");
	    wattroff(win, A_REVERSE);
    }
    else
    {
	    mvwaddwstr(win, rows - 1, cols - 1, L"?");

    }

    wrefresh(win);
}
// @process input
#define INPUT_MAX_LENGTH 128
char user_string[INPUT_MAX_LENGTH];
int process_input(WINDOW* win, ma_engine* sound_engine_ptr, int* quiz_flags, int random_int, int* random_int_out)
{
    int center_y = getmaxy(win) / 2;
    int center_x = getmaxx(win) / 2;

    int c;
    if (*quiz_flags & QUIZ_FLAGS_SINGLE_KEY_MODE_BIT)
    {
            mvwaddwstr(win, center_y + 2, center_x + CONTROLS_OFFSET_X - 1, L"  ");
            wmove(win, center_y + 2, center_x + CONTROLS_OFFSET_X);
	    c = wgetch(win);
            switch (c)
            {
		    case KEY_RESIZE:
			    return KEY_RESIZE;
                    case ':':
                            mvwaddwstr(win, center_y + 2, center_x + CONTROLS_OFFSET_X - 1, L": ");
                	    break;
		    case 'f':
		    case 'g':
		    case 's':
		    case 'a':
		    case 'd':
		    case 'h':
		    case 'l':
		    case 'j':
		    case 'k':
		    case ';':
		    case 'e':
		    case 'u':
		    case 'c':
		    case 'm':
		    case 'i':
	                    if (c == allophones[random_int].key)
	                    {
	                            int new_random_int = random_int;
                                    while (new_random_int == random_int)
	                        	    new_random_int = rand() % ALLOPHONE_COUNT;

	                            *random_int_out = random_int = new_random_int;

	                            ma_engine_play_sound(sound_engine_ptr, allophones[random_int].sound_file_path_str, NULL);
	                    }
	                    else
	                    {
	                            ma_engine_play_sound(sound_engine_ptr, "African1.mp3", NULL);
	                    }
			    return 0;
	            default:
                            ma_engine_play_sound(sound_engine_ptr, "Abstract2.mp3", NULL);
	            	    return 0;
            }
    }
    // @get user string
    mvwgetnstr(win, center_y + 2, center_x + CONTROLS_OFFSET_X, user_string, INPUT_MAX_LENGTH - 1);
    c = user_string[0];

    const int user_string_length = strlen(user_string);
    if (user_string_length == 1)
    {
            switch (c)
            {
                    case 'q':
                	    return 'q';
                    case 'h':
                	    *quiz_flags ^= QUIZ_FLAGS_SHOW_IPA_KEY_BIT;
                	    return 'h';
                    case '?':
                	    *quiz_flags ^= QUIZ_FLAGS_SHOW_OPTIONS_BIT;

                            int rows = getmaxy(win);
                            int cols = getmaxx(win);
	                    mvwaddwstr(win, rows - 6, cols - 18, L"                  ");
	                    mvwaddwstr(win, rows - 5, cols - 18, L"                  ");
	                    mvwaddwstr(win, rows - 4, cols - 18, L"                  ");
	                    mvwaddwstr(win, rows - 3, cols - 18, L"                  ");
	                    mvwaddwstr(win, rows - 2, cols - 18, L"                  ");
	                    mvwaddwstr(win, rows - 1, cols - 18, L"                  ");
                	    return '?';
                    case 'm':
                	    *quiz_flags ^= QUIZ_FLAGS_SINGLE_KEY_MODE_BIT;
                	    return 'm';
            }
    }

    // @clear user input from screen
    
    if (user_string_length > 10)
	    wclear(win);
    else
	    for (int i = 0; i < INPUT_MAX_LENGTH; i++)
                            mvwaddch(win, center_y + 2, center_x + CONTROLS_OFFSET_X - 1 + i, ' ');

    ma_result result;
    if (vowel_exists(user_string) == 0)
    {
            result = ma_engine_play_sound(sound_engine_ptr, "Abstract2.mp3", NULL);
	    if (result != MA_SUCCESS)
			    return -1;
    }
    else
    {
	    if (strcmp(user_string, allophones[random_int].ascii_str) == 0)
	    {
		    int new_random_int = random_int;
                    while (new_random_int == random_int)
			    new_random_int = rand() % ALLOPHONE_COUNT;

	            *random_int_out = random_int = new_random_int;

	            result = ma_engine_play_sound(sound_engine_ptr, allophones[random_int].sound_file_path_str, NULL);
	                    if (result != MA_SUCCESS)
	                            return 0;
	    }
	    else
	    {
	            result = ma_engine_play_sound(sound_engine_ptr, "African1.mp3", NULL);
	            if (result != MA_SUCCESS)
	                    return 0;
	    }
    }

    return 0;
}

// @print menu
void print_menu(WINDOW* win, int selected_mode)
{
    box(win, 0, 0);

    const int mode_count = 2;
    selected_mode %= mode_count;
    
    int rows = getmaxy(win);
    int y_offset = rows / 2 - mode_count / 2;
    mvwaddwstr(win, 0 + y_offset, 1, L"   original mode  ");
    mvwaddwstr(win, 1 + y_offset, 1, L"   button press   ");
    mvwaddwstr(win, selected_mode + y_offset, 2, L">");

    wrefresh(win);
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
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    const int orig_x = 12;
    const int keyboard_y = 9;
    const int menu_x = 20;
    WINDOW* win_orig = newwin(rows, orig_x, 0, 0);
    WINDOW* win_keyboard = newwin(keyboard_y, cols - orig_x, 0, orig_x);
    WINDOW* win_input = newwin(rows - keyboard_y, cols - orig_x - menu_x, keyboard_y, orig_x);
    WINDOW* win_menu = newwin(rows - keyboard_y, menu_x, keyboard_y, cols - menu_x);
    refresh();

    srand(time(0));

    int vowel_index_selected = rand() % ALLOPHONE_COUNT;
    int c = 0;
    while (c != 'q')
    {
	    if (c == KEY_RESIZE)
	    {
                    getmaxyx(stdscr, rows, cols);
	            wresize(win_orig, rows, orig_x );
	            wresize(win_keyboard, keyboard_y, cols - orig_x );
	            wresize(win_input, rows - keyboard_y, cols - orig_x - menu_x);
	            wresize(win_menu, rows - keyboard_y, menu_x);
	            mvwin(win_menu, keyboard_y, cols - menu_x);

	            wclear(win_orig);
	            wclear(win_keyboard);
	            wclear(win_menu);
	            wclear(win_input);
	    }

            print_original(win_orig, quiz_flags);
	    print_keyboard(win_keyboard);
	    print_menu(win_menu, (quiz_flags & QUIZ_FLAGS_SINGLE_KEY_MODE_BIT) >> 2);
	    print_input(win_input, vowel_index_selected, quiz_flags);

	    c = process_input(win_input, &engine, &quiz_flags, vowel_index_selected, &vowel_index_selected);

	    refresh();
    }


    endwin();
    ma_engine_uninit(&engine);

    return 0;
}
