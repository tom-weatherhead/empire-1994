// empire_io_vms.cxx

#include <string.h>
#include <iostream.h>
#include <curses.h>
#include "empire.h"

#define ROW_OFFSET 1    // ie. start in row 2
#define COL_OFFSET 1
#define MSG_ROW    (MAP_ROWS+ROW_OFFSET+1)
#define PROMPT_ROW (MAP_ROWS+ROW_OFFSET)


void Map_Class::display( void )
{
    unsigned int i;

    for( i = 0; i < MAP_ROWS; i++ ) {
        mvaddstr( i + ROW_OFFSET, COL_OFFSET, ch[i] );
    }
}


unsigned int init_display( void )
{
    int i;

    initscr();	// or newterm();

#if 0
    cbreak();
    noecho();	// for character-at-a-time input without echoing

    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);

// use tset command in .profile or .login to set tab stops?
#endif

    for( i = 0; i <= (MAP_ROWS-1)/10; i++ ) {
        mvaddch( 10 * i + ROW_OFFSET, 0, (char)(i + 48) );
    }

    for( i = 0; i <= (MAP_COLS-1)/10; i++ ) {
        mvaddch( 0, 10 * i + COL_OFFSET, (char)(i + 48) );
    }

    return( OK );
}


unsigned int kill_display( void )
{
    endwin();
    return( OK );
}


void show_line( char * str, unsigned int row )
{
    unsigned int len = strlen( str );
    char out_str[79];

    memset( out_str, ' ', 78 - len );
    out_str[78 - len] = '\0';
    mvaddstr( row, len, out_str );
    mvaddstr( row, 0, str );
}


void show_msg( char * str ) {
    show_line( str, MSG_ROW );
}


void show_prompt( char * str ) {
    show_line( str, PROMPT_ROW );
}


void get_string( char * str, unsigned int len )
{
    refresh();		// Attempt to minimize refreshing by calling it here
    getstr( str );
    str[len - 1] = '\0';
}


void mark_army( unsigned int row, unsigned int col )
{
    mvaddch( row + ROW_OFFSET, col + COL_OFFSET, '#' );
}


// Refresh area of map on screen

void Map_Class::refresh_area( unsigned int row_p, unsigned int col_p )
{
    unsigned int row, top, bottom, left, right, width;
    char str[4];	// Max. area width is 4 - 1

    get_adjacent_area( row_p, col_p, &top, &bottom, &left, &right );
    width = right - left + 1;
    str[width] = '\0';

    for( row = top; row <= bottom; row++ ) {
	memcpy( str, &ch[row][left], width );
	mvaddstr( row + ROW_OFFSET, left + COL_OFFSET, str );
    }
}


void refresh_screen( void )
{
    refresh();
}


unsigned int get_rand_seed( void )
{
#if 0   // VMS
#include <time.h>
    unsigned int timebuf[2];

    time( (time_t *)timebuf );
    return( timebuf[0] );
#else   // Unix
    unsigned int seed;

    cout << "Srand seed: ";
    cin >> seed;
    return( seed );
#endif
}

