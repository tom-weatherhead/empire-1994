// empire.cpp
// (C) Copyright 1992-1994 by Tom Weatherhead

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream.h>
#include <ctype.h>
#include <assert.h>
#include "q1.h"
#include "empire.h"


int e_sign( int val )
{
    return( (val > 0) ? 1 : ( (val < 0) ? -1 : 0 ) );
} // e_sign()


unsigned int owner_num( char ch )
{
    /* ie. if bad parm, should ACCVIO or seg. fault */
    unsigned int rtn = 32767;

    if( islower( ch ) ) {
	rtn = ch - 'a';
    } else if( isupper( ch ) ) {
	rtn = ch - 'A';
    } else if( isdigit( ch ) ) {
	rtn = ch - '0';
/*    } else {
	cout << "owner_num() : bad parameter '" << ch << "'\n"; */
    } else {
    	assert( 0 );
    } // if

    return( rtn );
} // owner_num()


void get_adjacent_area( unsigned int row, unsigned int col, unsigned int * top,
  unsigned int * bottom, unsigned int * left, unsigned int * right )
{
    *top = (row) ? row - 1 : 0;
    *bottom = (row == MAP_ROWS - 1) ? MAP_ROWS - 1 : row + 1;
    *left = (col) ? col - 1 : 0;
    *right = (col == MAP_COLS - 1) ? MAP_COLS - 1 : col + 1;
} // get_adjacent_area()


World_Class::World_Class( void )
{
    char cmd_str[10];

    num_cities = num_empires = 0;
    day = 1;
    cur_map_num = NUM_EMPIRES;
    map = new Map_Class;

    cout << "Start (n)ew game or (l)oad old game: ";
    cin >> cmd_str;

    if( tolower(cmd_str[0]) == 'l' ) {
        load_world();
    } else {

        do {
            cout << "Number of empires (max. " << NUM_EMPIRES << "): ";
            cin >> num_empires;
        } while( num_empires == 0  ||  num_empires > NUM_EMPIRES );

        do {
            cout << "Number of cities (max. " << NUM_CITIES << "): ";
            cin >> num_cities;
        } while( num_cities < num_empires  ||  num_cities > NUM_CITIES );

        init_all();
    } // if

    init_display();
} // World_Class::World_Class()


void World_Class::load_world( void )
{
    char filename[40];
    unsigned int i, row, col;
    fstream file;
    Army_Class * army_ptr;
    City_Class * city_ptr;
    Empire_Class * owner;

    cout << "Warning! this will kill class member pointers\n";
    cout << "Load: Filename (or quit): ";
    cin >> filename;

    if( !strcmp( filename, "quit" ) ) exit( 0 );

    file.open( filename, ios::in );
    file.read( (char *)&day, sizeof( day ) );
    file.read( (char *)&map, sizeof( map ) );
    file.read( (char *)&num_empires, sizeof( num_empires ) );

    for( i = 0; i < num_empires; i++ ) {
// Read live/dead status; if dead then don't read this empire; continue;

	owner = empire[i] = new Empire_Class( i, this );
	file.read( (char *)owner, sizeof( Empire_Class ) );
	owner->map.src_map = map;
	// This stuff isn't just done in constructors
        // because old values must be overwritten.
	owner->army_q.head = NULL;
	owner->army_q.size = 0;
//	owner->idle_army_q = &idle_army_q;
	owner->city_head = NULL;
    	owner->WorldObj = this;
    } // for

    file.read( (char *)&num_cities, sizeof( num_cities ) );

    for( i = 0; i < num_cities; i++ ) {
	city_ptr = city[i] = new City_Class( i );
	file.read( (char *)city_ptr, sizeof( City_Class ) );

	if( city_ptr->ch == NEUTRAL_CITY_CH ) continue;

	city_ptr->owner = empire[owner_num( city_ptr->ch )];
	owner = city_ptr->owner;
	city_ptr->next = owner->city_head; // Add city to owner's list
	owner->city_head = city_ptr;

	if( city_ptr->guest_army ) {
	    army_ptr = idle_army_q.dequeue();
	    army_ptr->init( city_ptr );
	    army_ptr->host_city = city_ptr;
	    city_ptr->owner->army_q.enqueue( army_ptr );
	    city_ptr->guest_army = army_ptr;
	} // if
    } // for

    file.close();

    // Detect armies on map and create objects for them

    for( row = 0; row < MAP_ROWS; row++ ) {

	for( col = 0; col < MAP_COLS; col++ ) {

	    if( !islower( map->ch[row][col] ) ) continue;

	    army_ptr = idle_army_q.dequeue();
	    owner = empire[owner_num( map->ch[row][col] )];
	    army_ptr->init( owner, row, col );
	    owner->army_q.enqueue( army_ptr );
	} // if
    } // if
} // World_Class::load_world()


void World_Class::save_world( void )
{
    char filename[40];
    unsigned int i;
    fstream file;

    show_prompt( "Save: Filename: " );
    get_string( filename, sizeof(filename) );
    file.open( filename, ios::out );
    file.write( (char *)&day, sizeof( day ) );
    file.write( (char *)&map, sizeof( map ) );
    file.write( (char *)&num_empires, sizeof( num_empires ) );

    for( i = 0; i < num_empires; i++ ) {
// Write empire's alive/dead status; if dead then continue;

	file.write( (char *)&empire[i], sizeof( Empire_Class ) );
    } // for

    file.write( (char *)&num_cities, sizeof( num_cities ) );

    for( i = 0; i < num_cities; i++ ) {
	file.write( (char *)&city[i], sizeof( City_Class ) );
    } // for

    file.close();
} // World_Class::save_world()


void World_Class::init_all( void )
{
    char cmd_str[10];
    unsigned int i;
    Empire_Class * empirePtr;

    srand( time( NULL ) );
    cout << "Land-filled map? (y/n): ";
    cin >> cmd_str;

    if( cmd_str[0] == 'y' ) {
	map->landfill();
    } else {
	map->generate();
    } // if

    for( i = 0; i < num_cities; i++ ) {
	city[i] = new City_Class( i );
	city[i]->PlaceRandomly( map );
    } // for

    for( i = 0; i < num_empires; i++ ) {
	empirePtr = empire[i] = new Empire_Class( i, this );
	empirePtr->map.src_map = map;
	city[i]->change_owner( empirePtr );
    } // for
} // World_Class::init_all()


City_Class * World_Class::find_city( unsigned int row, unsigned int col )
{
    unsigned int i;

    for( i = 0; i < num_cities; i++ ) {

        if( city[i]->row == row  &&  city[i]->col == col ) return( city[i] );
    } // for

    return( NULL );
} // World_Class::find_city()


void World_Class::neutralize_city( void )
{
    City_Class * ptr;
    char in_str[10];
    unsigned int row, col;

    show_prompt( "Neutralize city: Row: " );
    get_string( in_str, sizeof(in_str) );
    row = atoi( in_str );
    show_prompt( "Neutralize city: Col: " );
    get_string( in_str, sizeof(in_str) );
    col = atoi( in_str );
    ptr = find_city( row, col );

    if( ptr == NULL  ||  ptr->owner == NULL ) return;

    // Make into City_Class member
    ptr->ch = map->ch[row][col] = NEUTRAL_CITY_CH;
    ptr->owner->update_local_map_area( row, col );
    ptr->owner = NULL;
} // World_Class::neutralize_city()


void World_Class::create_land( void )
{
    char in_str[10], * ptr;
    unsigned int row, col;

    show_prompt( "Create land: Row: " );
    get_string( in_str, sizeof( in_str ) );
    row = atoi( in_str );
    show_prompt( "Create land: Col: " );
    get_string( in_str, sizeof( in_str ) );
    col = atoi( in_str );

    if( row >= MAP_ROWS  ||  col >= MAP_COLS ) {
	show_msg( "Coords not on map" );
	return;
    } // if

    ptr = &map->ch[row][col];

    if( *ptr == WATER_CH ) {
	*ptr = LAND_CH;
    } // if
} // World_Class::create_land()


void World_Class::one_days_move( void )
{
    static char msg_str[80];
    unsigned int i;
    Empire_Class * owner;

    for( i = 0; i < num_empires; i++ ) {
	// Put into Empire_Class
	owner = empire[i];

#ifndef _DEBUG
	if( !owner->automove ) {
#endif
            sprintf( msg_str, "Day %d; Empire %d: %d cities, %d armies",
              day, i, owner->num_cities, owner->army_q.size );
            show_line( msg_str, DATELINE );
#ifndef _DEBUG
	} // if
#endif

        if( owner->army_q.head == NULL ) continue;

        cur_map_num = NUM_EMPIRES;  /* ie. no on-screen updating */
        owner->update_local_map();

	if( owner->automove ) {
#ifdef _DEBUG
	    cur_map_num = i;
	    owner->map.display();
#endif
	    owner->automove_all_armies();
        } else {
            cur_map_num = i;
            owner->map.display();
	    owner->move_all_armies();
        } // if
    } // for

    for( i = 0; i < num_cities; i++ ) {
        city[i]->update();
    } // for

    day++;
} // World_Class::one_days_move()


void World_Class::Animate( void )
{
    char cmd_str[10];

    for( ; ; ) {
        show_prompt( "Command (almnqrsuw): " );
        get_string( cmd_str, sizeof(cmd_str) );

        switch( tolower(cmd_str[0]) ) {
            case 'a':
                show_prompt( "Automate which empire? " );
                get_string( cmd_str, sizeof(cmd_str) );
                empire[atoi(cmd_str)]->automove = TRUE;
                break;
            case 'l':
                create_land();          break;
            case 'm':
                one_days_move();        break;
            case 'n':
                neutralize_city();      break;
            case 'q':
                return;
            case 'r':
                refresh_screen();       break;
            case 's':
                save_world();           break;
	    case 'u':
                show_prompt( "Unautomate which empire? " );
                get_string( cmd_str, sizeof(cmd_str) );
		empire[atoi(cmd_str)]->automove = FALSE;
                break;
            case 'w':
                map->display();
                show_msg( "Objective world view" );
                break;
        } // switch
    } // for
} // World_Class::Animate()


World_Class::~World_Class( void )
{
    unsigned int i;

    delete map;

    for( i = 0; i < num_cities; i++ ) {
	delete city[i];
    } // for

    for( i = 0; i < num_empires; i++ ) {
	delete empire[i];
    } // for

    kill_display();
} // World_Class::~World_Class()


void main( void )
{
    World_Class WorldObj;	// The world object

    WorldObj.Animate();
} // main()


// **** End of empire.cpp ****
