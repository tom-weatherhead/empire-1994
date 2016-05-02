// emarmy.cpp
// (C) Copyright 1992-1994 by Tom Weatherhead

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "q1.h"
#include "empire.h"

static int v8[8][2] = { { -1, -1 }, { -1,  0 }, { -1,  1 }, { 0,  1 },
                        {  1,  1 }, {  1,  0 }, {  1, -1 }, { 0, -1 } };


Army_Class::Army_Class( void )
{
    init( NULL, 0, 0 );
} // Army_Class::Army_Class()


void Army_Class::init( City_Class * home )
{
    if( home == NULL ) {
    	init( NULL, 0, 0 );
    } else {
	init( home->owner, home->row, home->col );
        host_city = home;
    } // if
} // Army_Class::init()


void Army_Class::init( Empire_Class * owner_parm, unsigned int row_parm,
  unsigned int col_parm )
{
    row = row_parm;
    col = col_parm;
    owner = owner_parm;
    next = NULL;
    prev = NULL;
    target_row = target_col = 0;
    vect_to_prev = 0;
    ch = (owner != NULL) ? owner->army_ch : '?';
    host_city = NULL;
    is_targeted = FALSE;
    is_automated = (owner != NULL) ? owner->automove : FALSE;
    fc_dir = DIR_LINEAR;
    wander_vector = v8[((unsigned int)rand()) % 8];
} // Army_Class::init()


void Army_Class::kill( void )
{

    if( !( owner->army_q.remove( this ) ) ) return;

    owner->update_local_map_area( row, col );

    if( is_targeted ) {
        owner->target_location( target_row, target_col );
    }

    owner->WorldObj->idle_army_q.enqueue( this );
} // Army_Class::kill()


// Kill the enemy army occupying the same position.

bool Army_Class::kill_other_army( void )
{
    unsigned int i;
    Army_Class * ptr;
    World_Class * WorldObj = owner->WorldObj;

    for( i = 0; i < WorldObj->num_empires; i++ ) {

        if( i == owner->num ) continue;

	ptr = WorldObj->empire[i]->army_q.find( row, col );

        if( ptr == NULL ) continue;

        ptr->kill();
        return( TRUE );
    } // for

    return( FALSE );
} // Army_Class::kill_other_army()


void Army_Class::army_takes_city( void )
{
    char msg_str[80];
    unsigned int occ_dur;
    City_Class * city_ptr, **cityPP;
    Empire_Class * old_owner;
    World_Class * WorldObj = owner->WorldObj;

    if( ( city_ptr = WorldObj->find_city( row, col ) ) == NULL ) {
        show_msg( "Error: ghost city" );
        return;
    } // if

    occ_dur = WorldObj->day - city_ptr->held_since;
    old_owner = city_ptr->owner;
//    map.ch[ptr->row][ptr->col] = ptr->owner->city_ch;

    if( city_ptr->guest_army != NULL ) {
        city_ptr->guest_army->kill();
    } // if

    // Move this into change_owner() ?

    if( old_owner != NULL ) {
        old_owner->num_cities--;
	// Delete city from old_owner city queue

	for( cityPP = &old_owner->city_head; *cityPP != NULL;
	  cityPP = &((*cityPP)->next) ) {

	    if( *cityPP == city_ptr ) {
		*cityPP = city_ptr->next;
		break;
	    } // if
	} // for
    } // if

    city_ptr->change_owner( owner );
    city_ptr->held_since = WorldObj->day; // Move into change_owner()

    if( old_owner != NULL ) {
        old_owner->update_local_map_area( row, col );
    } // if

    is_targeted = FALSE; // So city is no longer attacked
    kill();

    if( !city_ptr->owner->automove ) {
	sprintf( msg_str,
	  "City %d at row %d, col %d subjugated after %d days",
	  city_ptr->num, city_ptr->row, city_ptr->col, occ_dur );
	show_msg( msg_str );
    } // if
} // Army_Class::army_takes_city()


bool Army_Class::move_to_coords( unsigned int new_row,
  unsigned int new_col )
{
    char new_ch, * new_ptr;
    bool kill_other = FALSE;

    if( /* new_row < 0  || */ new_row >= MAP_ROWS  /* ||  new_col < 0 */
      ||  new_col >= MAP_COLS ) {
	owner->show_msg_if_interactive( "Can't move off map" );
	return( FALSE );
    } // if

//    new_ptr = &owner->map.src_map->ch[new_row][new_col];
    new_ptr = &owner->WorldObj->map->ch[new_row][new_col];
    new_ch = *new_ptr;

    if( new_ch == WATER_CH ) {
	owner->show_msg_if_interactive( "Can't move onto water" );
	return( FALSE );
    } else if( isalnum(new_ch)  &&  owner_num(new_ch) == owner->num ) {
	owner->show_msg_if_interactive( "Can't attack self" );
	return( FALSE );
    } // if
          
    owner->WorldObj->map->ch[row][col] = (host_city != NULL) ? owner->city_ch
      : LAND_CH;

    if( abs((int)(row - new_row)) > 1
      ||  abs((int)(col - new_col)) > 1 ) { // 'Goto' cmd used
	owner->update_local_map_area( row, col );
    } // if

    if( host_city != NULL ) {
	host_city->guest_army = NULL;
	host_city = NULL;
    } // if

    row = new_row;
    col = new_col;

    if( new_ch != LAND_CH ) {	// Fight

	if( rand() & 16384 ) {	// Kill this army
	    owner->show_msg_if_interactive( "Army is defeated" );
	    kill();
	    return( TRUE );
	} // if

	if( new_ch == NEUTRAL_CITY_CH  ||  isupper( new_ch ) ) {
	    army_takes_city();
	    return( TRUE );	// City captured
	} // if

	// Opponent's army destroyed
	kill_other = TRUE;
    } // if

    *new_ptr = ch;
    owner->update_local_map_area( new_row, new_col );

    if( kill_other ) {
	owner->show_msg_if_interactive( "Army is victorious" );
	kill_other_army();
    } // if

    return( TRUE );
} // Army_Class::move_to_coords()


bool Army_Class::manual_move( char control_ch )
{
    char in_str[10];
    int new_row = row, new_col = col;

    switch( control_ch ) {

	case 's':	// Stay where you are
	case ' ':
	    owner->update_local_map_area( new_row, new_col );
	    return( TRUE );

	case 'r':
	    refresh_screen();
	    return( FALSE );

	case 'i':	// Independence
	    is_automated = TRUE;
	    Automove();
	    return( TRUE );

#if 0
	case 't':	// Aim army at a target

	    for( ; ; ) {
		cout << "Target: row? ";
		cin >> target_row;
		cout << "Target: col? ";
		cin >> target_col;
		is_targeted = TRUE;

		if( MoveToTarget() ) break;

		show_msg( "Target lock didn't hold; try again" );
	    }

	    return( TRUE );

	case 'g':	// GOTO hack; ie. teleport army
	    show_prompt( "Row: " );
	    get_string( in_str, sizeof( in_str ) );
	    new_row = atoi( in_str );
	    show_prompt( "Col: " );
	    get_string( in_str, sizeof( in_str ) );
	    new_col = atoi( in_str );
	    break;
#endif

	case 'q':
	case 'w':
	case 'e':
	    new_row--; break;

	case 'z':
	case 'x':
	case 'c':
	    new_row++; break;

	case 'a':
	case 'd':
	    break;

	default:
	    return( FALSE );	// Unrecognized key
    } // switch

    switch( control_ch ) {
	case 'q':
	case 'a':
	case 'z':
	    new_col--;
	    break;

	case 'e':
	case 'd':
	case 'c':
	    new_col++;
	    break;

	default:
            break;
    } // switch

    return( move_to_coords( new_row, new_col ) );
} // Army_Class::manual_move()


void Army_Class::Automove( void )
{
#ifdef _DEBUG
    static char buf[60];
#endif
    unsigned int new_row, new_col;

#ifdef _DEBUG
    sprintf( buf, "Army %c at %d,%d", owner->army_ch, row, col );
    show_line( buf, DATELINE );
#endif

    // First, kill any adjacent target

    if( owner->adjacent_search( row, col, SRCH_OPPONENT, &new_row, &new_col ) ) {
        is_targeted = TRUE;
        target_row = new_row;
        target_col = new_col;
    } // if

    // Continue towards programmed target, if any

    if( is_targeted  &&  MoveToTarget() )
      return;

    // Follow wander vector, if possible

    if( !owner->adjacent_search( row, col, SRCH_LAND,
      &new_row, &new_col ) ) return; // If you can't move, don't try

    for( ; ; ) {

	if( fc_dir == DIR_LINEAR ) {
//          assert( wander_vector != NULL );

	    if( wander_vector!=NULL && move_to_coords(
		row + wander_vector[0],
		col + wander_vector[1] ) ) break;

	} else if( follow_coast() ) break;

	// Search for new way to move

	if( rand() & 16384 ) {		// Try to follow coast
	    fc_dir = (rand() & 16384) ? DIR_CW : DIR_CCW;
	    init_vect_to_prev();
	} else {
	    fc_dir = DIR_LINEAR;
            wander_vector = v8[((unsigned int)rand()) % 8];
        } // if
    } // for
} // Army_Class::Automove()


void Army_Class::init_vect_to_prev( void )
{
    unsigned int i, old_row, old_col;

    if( owner->adjacent_search( row, col, SRCH_WATER, &old_row, &old_col ) ) {

	for( i = 0; i < 8; i++ ) {

	    if( v8[i][0] == old_row - row && v8[i][1] == old_col - col ) break;
	}
 
	vect_to_prev = i;
	return;
    }

    if( row == 0 ) {
	vect_to_prev = 1;
    } else if( row == MAP_ROWS - 1 ) {
	vect_to_prev = 5;
    } else if( col == 0 ) {
	vect_to_prev = 7;
    } else if( row == MAP_ROWS - 1 ) {
	vect_to_prev = 3;
    }
} // Army_Class::init_vect_to_prev()


bool Army_Class::follow_coast( void )
{
    unsigned int i, cnt, new_row, new_col; // last 2 should be redundant

    if( !owner->adjacent_search( row, col, SRCH_WATER, &new_row, &new_col )
      && row > 0 && row < MAP_ROWS - 1 && col > 0 && col < MAP_COLS - 1 ) {
	// Army has run off track; switch to linear movement
	fc_dir = DIR_LINEAR;
	return( FALSE );
    }

    for( i = (vect_to_prev + fc_dir) % 8, cnt = 0; cnt < 8;
      i = (i + fc_dir) % 8, cnt++ ) {

	if( !move_to_coords( row + v8[i][0], col + v8[i][1] ) ) continue;

	vect_to_prev = (i + 4) % 8;
	break;
    }

    return( TRUE );
} // Army_Class::follow_coast()


bool Army_Class::MoveToTarget( void )
{
    unsigned int new_row, new_col;

    new_row = row + e_sign( target_row - row );
    new_col = col + e_sign( target_col - col );

    if( ( new_row != row  ||  new_col != col )
      &&  move_to_coords( new_row, new_col ) ) return( TRUE );

    // Reached target location or ran into obstacle; untarget
    is_targeted = FALSE;
    return( FALSE );
} // Army_Class::MoveToTarget()


// **** End of emarmy.cpp ****
