// e_iodos.cpp
// (C) Copyright 1992-1994 by Tom Weatherhead

#include <string.h>
#include <iostream.h>
#include <conio.h>
#include "q1.h"
#include "empire.h"

#define ROW_OFFSET 1    // ie. start in row 2
#define COL_OFFSET 1
#define MSG_ROW    (MAP_ROWS+ROW_OFFSET+1)
#define PROMPT_ROW (MAP_ROWS+ROW_OFFSET)


void mvaddch( unsigned int row, unsigned int col,
  char ch )
{
    gotoxy( col, row );
    cout << ch;
}


void mvaddstr( unsigned int row, unsigned int col,
  char * str )
{
    gotoxy( col, row );
    cout << str;
}


unsigned int init_display( void )
{
    int i;

    for( i = 0; i <= (MAP_ROWS-1)/10; i++ ) {
        mvaddch( 10 * i + ROW_OFFSET, 0, (char)(i + 48) );
    }

    for( i = 0; i <= (MAP_COLS-1)/10; i++ ) {
        mvaddch( 0, 10 * i + COL_OFFSET, (char)(i + 48) );
    }

    return( 0 );
}


unsigned int kill_display( void )
{
    return( 0 );
}


void show_line( char * str, unsigned int row )
{
    unsigned int len = strlen( str );
    static char out_str[79];

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
    // refresh();		// Attempt to minimize refreshing by calling it here
    cin >> str;			// getstr( str );
    str[len - 1] = '\0';
}


void mark_army( unsigned int row, unsigned int col )
{
    mvaddch( row + ROW_OFFSET, col + COL_OFFSET, '#' );
}


void refresh_screen( void )
{
    // refresh();
}


void Map_Class::display( void )
{
    unsigned int i;

    for( i = 0; i < MAP_ROWS; i++ ) {
        mvaddstr( i + ROW_OFFSET, COL_OFFSET, ch[i] );
    }
} // Map_Class::display()


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
} // Map_Class::refresh_area()


// ******** End of e_iodos.cpp ********
