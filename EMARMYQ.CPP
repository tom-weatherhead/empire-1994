// emarmyq.cpp
// (C) Copyright 1992-1994 by Tom Weatherhead

// #include <iostream.h>
#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
//#include <ctype.h>
#include <assert.h>
#include "q1.h"
#include "empire.h"


Army_Q_Class::Army_Q_Class( void )
{
//    head = NULL;
    size = 0;
    next_new_army_num = 1;
} // Army_Q_Class::Army_Q_Class()


void Army_Q_Class::enqueue( Army_Class * ptr )
{
    assert( ptr != NULL );
//    ptr->next = head;
//    head = ptr;
    Q1Class::Enqueue( ptr );
    size++;
} // Army_Q_Class::enqueue()


Army_Class * Army_Q_Class::dequeue( void ) /* Use with idle army queue */
{
    Army_Class * rtn;

    if( head != NULL ) {
        rtn = (Army_Class *)head;
//        head = head->next;
	size--;
        Q1Class::Dequeue( head );
//	rtn->next = NULL;
    } else {
	rtn = new Army_Class;
        rtn->num = next_new_army_num++;
    } // if

    return( rtn );
} // Army_Q_Class::dequeue()


Army_Class * Army_Q_Class::find( unsigned int row, unsigned int col )
{
    Army_Class * ptr;

    for( ptr = (Army_Class *)head; ptr != NULL;
      ptr = (Army_Class *)(ptr->next) ) {

        if( ptr->row == row  &&  ptr->col == col ) break;
    }

    return( ptr );
} // Army_Q_Class::find()


// Dequeues, but doesn't delete.
// Assumes that key is in the queue.

bool Army_Q_Class::remove( Army_Class * key )
{
#if 1
    Q1Class::Dequeue( key );
    size--;
    return( TRUE );     /* Nailed it */
#else
    Army_Class * ptr, ** listPP;

    for( listPP = &head; *listPP != NULL;
      listPP = &((*listPP)->next) ) {
        ptr = *listPP;

        if( ptr == key ) {
            *listPP = ptr->next;
            size--;
            return( TRUE );     /* Nailed it */
	}
    }

    return( FALSE );    /* No such army in this queue */
#endif
} // Army_Q_Class::remove()


Army_Class * Army_Q_Class::nearest_free_army( unsigned int row,
  unsigned int col )
{
    unsigned int dist, best_dist = MAP_ROWS*MAP_ROWS+MAP_COLS*MAP_COLS;
    Army_Class * best = NULL, * ptr;

    for( ptr = (Army_Class *)head; ptr != NULL;
      ptr = (Army_Class *)(ptr->next) ) {

        if( ptr->is_targeted ) continue;

        dist = (row - ptr->row) ^ 2 + (col - ptr->col) ^ 2;

        if( dist < best_dist ) {
            best_dist = dist;
            best = ptr;
        }
    }

    return( best );
} // Army_Q_Class::nearest_free_army()


Army_Q_Class::~Army_Q_Class( void )
{
#if 0
    Army_Class * ptr;

    while( head != NULL ) {
        ptr = head;
        head = head->next;
        delete ptr;
    }
#endif
} // Army_Q_Class::~Army_Q_Class()


// **** End of emarmyq.cpp ****
