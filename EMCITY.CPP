// emcity.cpp
// (C) Copyright 1992-1994 by Tom Weatherhead

#include <stdlib.h>
#include "q1.h"
#include "empire.h"


// ******** City_Class member functions ********


City_Class::City_Class( unsigned int num_parm )
{
    num = num_parm;
    row = col = 0;
    next_production = 1000; // Something big 
    owner = NULL;
    guest_army = NULL;
    ch = NEUTRAL_CITY_CH;
    next = NULL;
    population = INIT_POP;
    held_since = 1;
//    PlaceRandomly( map );
} // City_Class::City_Class()


void City_Class::PlaceRandomly( Map_Class * map )
{
    do {
	row = rand() % MAP_ROWS;
	col = rand() % MAP_COLS;
    } while( map->ch[row][col] != LAND_CH );

    map->ch[row][col] = ch;
} // City_Class::PlaceRandomly()


void City_Class::change_owner( Empire_Class * owner_parm )
{
    next_production = 6;
    guest_army = NULL;
    owner = owner_parm;
    ch = owner->city_ch;
    next = owner->city_head;
    owner->city_head = this;
    owner->num_cities++;
    owner->WorldObj->map->ch[row][col] = ch;
    owner->update_local_map_area( row, col );
} // City_Class::change_owner()


void City_Class::update( void )
{
    Army_Class * ptr;

    // Update the city's population

    if( owner == NULL  ||  guest_army != NULL ) return;

    if( owner->army_q.size >= MAX_ARMIES_PER_CITY * owner->num_cities ) return;

    if( --next_production > 0 ) return;

    ptr = owner->WorldObj->idle_army_q.dequeue();
    ptr->init( owner, row, col );
    owner->army_q.enqueue( ptr ); 
    ptr->host_city = this;
    next_production = 5;
    guest_army = ptr;
} // City_Class::update()


// **** End of emcity.cpp ****
