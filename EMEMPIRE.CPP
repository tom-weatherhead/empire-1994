// emempire.cpp
// (C) Copyright 1992-1994 by Tom Weatherhead

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
// define NDEBUG to turn off assertions
#include <assert.h>
#include "q1.h"
#include "empire.h"

static int v8[8][2] = { { -1, -1 }, { -1,  0 }, { -1,  1 }, { 0,  1 },
			{  1,  1 }, {  1,  0 }, {  1, -1 }, { 0, -1 } };


Empire_Class::Empire_Class( unsigned int num_parm, World_Class * WorldObjParm )
{
    automove = (num_parm != 0) ? TRUE : FALSE; // Empire 0 is interactive
    num = num_parm;
    num_cities = 0;
    city_ch = 'A' + num_parm;
    army_ch = 'a' + num_parm;
    city_head = NULL;
    WorldObj = WorldObjParm;
//    idle_army_q = iaq;
} // Empire_Class::Empire_Class()


// Update square at coords, and all adjacent squares

void Empire_Class::update_local_map_area( unsigned int row,
  unsigned int col )
{
    map.copy_local_area( row, col );

    if( num == WorldObj->cur_map_num ) {
	map.refresh_area( row, col );
    }

    if( automove ) {	// Target any new bogeys
	local_bogey_search( row, col );
    } // if
} // Empire_Class::update_local_map_area()


void Empire_Class::update_local_map( void )
{
    Army_Class * army_ptr;
    City_Class * city_ptr;

    for( army_ptr = (Army_Class *)(army_q.head); army_ptr != NULL;
      army_ptr = (Army_Class *)(army_ptr->next) ) {
        update_local_map_area( army_ptr->row, army_ptr->col );
    }

    for( city_ptr = city_head; city_ptr != NULL; city_ptr = city_ptr->next ) {

        if( city_ptr->guest_army != NULL ) continue; // Already updated above

        update_local_map_area( city_ptr->row, city_ptr->col );
    }
} // Empire_Class::update_local_map()


bool Empire_Class::adjacent_search( unsigned int row,
  unsigned int col, srch_code_type srch_code, unsigned int * rtn_row,
  unsigned int * rtn_col )
{
    char ch;
    bool found;
    unsigned int top, bottom, left, right, i, j, posn_list[8][2],
      num_posns = 0;

    get_adjacent_area( row, col, &top, &bottom, &left, &right );

    for( i = top; i <= bottom; i++ ) {

        for( j = left; j <= right; j++ ) {

            if( i == row  &&  j == col ) continue;

            found = FALSE;
            ch = map.ch[i][j];

            switch( srch_code ) {
                case SRCH_LAND: if( ch == LAND_CH ) found = TRUE; break;
                case SRCH_WATER: if( ch == WATER_CH ) found = TRUE; break;
                case SRCH_OPPONENT:

                    if( ch == NEUTRAL_CITY_CH  ||  ( isalnum( ch )
                      && owner_num( ch ) != num ) ) {
                        found = TRUE;
                    }

                    break;
            }

            if( found ) {
                posn_list[num_posns][0] = i;
                posn_list[num_posns++][1] = j;
            }
        }
    }

    if( num_posns == 0 ) return( FALSE );

    i = rand() % num_posns;
    *rtn_row = posn_list[i][0];
    *rtn_col = posn_list[i][1];
    return( TRUE );
} // Empire_Class::adjacent_search()


void Empire_Class::move_all_armies( void )
{
    char cmd_str[10], msg_str[40];
    Army_Class * ptr, * next;

    for( ptr = (Army_Class *)(army_q.head); ptr != NULL; ptr = next ) {
	next = (Army_Class *)(ptr->next);	// Because ptr may get deleted

	if( ptr->is_automated ) {
	    ptr->Automove();
	    continue;
	} else if( ptr->is_targeted  &&  ptr->MoveToTarget() ) {
	    continue;
	} // if

	mark_army( ptr->row, ptr->col );

	do {
	    sprintf( msg_str, "Army at row %d, col %d: ", ptr->row,
	      ptr->col );
	    show_prompt( msg_str );
	    get_string( cmd_str, sizeof(cmd_str) );
	} while( !ptr->manual_move( tolower(cmd_str[0]) ) );
    } // for
} // Empire_Class::move_all_armies()


void Empire_Class::show_msg_if_interactive( char * msg )
{
    if( automove ) return;

    show_msg( msg );
} // Empire_Class::show_msg_if_interactive()


bool Empire_Class::target_location( unsigned int row, unsigned int col )
{
    Army_Class * army_ptr;

    if( (army_ptr = army_q.nearest_free_army( row, col ) ) == NULL )
	return( FALSE );

    army_ptr->is_targeted = TRUE;
    army_ptr->target_row = row;
    army_ptr->target_col = col;
    return( TRUE );
} // Empire_Class::target_location()


// Target any newly-encountered bogeys

void Empire_Class::local_bogey_search( unsigned int p_row,
  unsigned int p_col )
{
    bool is_targeted;
    unsigned int row, col;
    Army_Class * army_ptr;

    if( !adjacent_search( p_row, p_col, SRCH_OPPONENT, &row, &col ) )
      return;

    // NOTE: if >1 bogeys adjacent, one randomly chosen; not rigorous

    // See if bogey is already targeted
    is_targeted = FALSE;

    for( army_ptr = (Army_Class *)(army_q.head); army_ptr != NULL;
      army_ptr = (Army_Class *)(army_ptr->next) ){

        if( army_ptr->is_targeted  &&  army_ptr->target_row == row
          &&  army_ptr->target_col == col ) {
            is_targeted = TRUE;
            break;      // ie. 1 army allocated per bogey
        }
    }

    if( is_targeted ) return;

    target_location( row, col );
} // Empire_Class::local_bogey_search()


// Most of this should go into Army_Class::automove()

void Empire_Class::automove_all_armies( void )
{
    Army_Class * army_ptr, * next = NULL;

    for( army_ptr = (Army_Class *)(army_q.head); army_ptr != NULL; army_ptr = next ) {
	next = (Army_Class *)(army_ptr->next);
    	army_ptr->Automove();
    } // for

#ifdef _DEBUG
    show_line( "automove_all_armies done", DATELINE );
#endif
} // Empire_Class::automove_all_armies()


// **** End of emempire.cpp ****
