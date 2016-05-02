// emmap.cpp
// Map class member functions
// (C) Copyright 1992-1994 by Tom Weatherhead

// #include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "q1.h"
#include "empire.h"


Map_Class::Map_Class( void )
{
    unsigned int i;

    src_map = NULL;

    for( i = 0; i < MAP_ROWS; i++ ) {
        memset( ch[i], ' ', MAP_COLS );
        ch[i][MAP_COLS] = '\0';
    }
} // Map_Class::Map_Class()


void Map_Class::landfill( void )
{
    unsigned int i;

    for( i = 0; i < MAP_ROWS; i++ ) {
        memset( ch[i], LAND_CH, MAP_COLS );
    }
} // Map_Class::landfill()


void Map_Class::generate( void )
{
    char temp_ch[MAP_ROWS][MAP_COLS+1];
    unsigned int i, row, col, top, bottom, left, right, water_cnt, row2, col2,
      change_pt, total_neigh;

    for( row = 0; row < MAP_ROWS; row++ ) {

        for( col = 0; col < MAP_COLS; col++ ) {
	    ch[row][col] = (rand()&8192) ? LAND_CH : WATER_CH;
        }

        temp_ch[row][MAP_COLS] = '\0';
    }

    for( i = 0; i < 3 /* GENERATE_MAP_REPS */; i++ ) {

        for( row = 0; row < MAP_ROWS; row++ ) {
	    top = (row) ? row - 1 : 0;
            bottom = (row == MAP_ROWS - 1) ? MAP_ROWS - 1 : row + 1;

            for( col = 0; col < MAP_COLS; col++ ) {
                left = (col) ? col - 1 : 0;
                right = (col == MAP_COLS - 1) ? MAP_COLS - 1 : col + 1;
                total_neigh = water_cnt = 0;

                for( row2 = top; row2 <= bottom; row2++ ) {

                    for( col2 = left; col2 <= right; col2++ ) {

                        if( row == row2  &&  col == col2 ) continue;

                        total_neigh++;

                        if( ch[row2][col2] == WATER_CH ) {
                            water_cnt++;
                        }
		    } // for
		} // for

                change_pt = ( ch[row][col] == LAND_CH ) ? 5 : 3 /* 2 */;
                temp_ch[row][col] = (8*water_cnt <= change_pt*total_neigh )
                  ? LAND_CH : WATER_CH;
            } // for
	} // for
	   
	for( row = 0; row < MAP_ROWS; row++ ) {
	    memcpy( ch[row], temp_ch[row], MAP_COLS+1 );
        }
    } // for
} // Map_Class::generate()


void Map_Class::copy_local_area( unsigned int row, unsigned int col )
{
    unsigned int top, bottom, left, right, i, width;

    if( src_map == NULL ) return;

    get_adjacent_area( row, col, &top, &bottom, &left, &right );
    width = right - left + 1;

    for( i = top; i <= bottom; i++ ) {
	memcpy( ch[i] + left, src_map->ch[i] + left, width );
    } // for
} // Map_Class::copy_local_area()


// **** End of emmap.cpp ****
