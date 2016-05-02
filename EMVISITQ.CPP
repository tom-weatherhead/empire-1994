// emvisitq.cpp
// (C) Copyright 1994 by Tom Weatherhead

// #include <iostream.h>
#include <stdlib.h>
//#include <stdio.h>
//#include <ctype.h>
#include "empire.h"


VisitQClass::VisitQClass( void )
{
    head = NULL;
    tail = &head;
} // VisitQClass::VisitQClass()


bool VisitQClass::IsEmpty( void )
{
    return( ( head == NULL ) ? TRUE : FALSE );
} // VisitQClass::IsEmpty()


CoordType * VisitQClass::FindCoord( unsigned int row, unsigned int col )
{
    CoordType * ptr;

    for( ptr = head; ptr != NULL  &&  (row != ptr->row  ||  col != ptr->col);
      ptr = ptr->next );

    return( ptr );
} // VisitQClass::FindCoord()


void VisitQClass::EnqueueCoord( unsigned int row, unsigned int col )
{
    CoordType * ptr;

    if( FindCoord( row, col ) != NULL ) return;

    ptr = new CoordType;
    ptr->row = row;
    ptr->col = col;
    ptr->targeted = FALSE;
    ptr->prev = &head;
    ptr->next = head;

    if( IsEmpty() ) {
	assert( tail == &head );
	tail = &ptr->next;
    } else {
	assert( tail != &head );
	head->prev = &ptr->next;
    } // if

    head = ptr;
} // VisitQClass::EnqueueCoord()


bool VisitQClass::DequeueCoord( unsigned int row, unsigned int col )
{
    CoordType * ptr = FindCoord( row, col );

    if( FindCoord == NULL ) return( FALSE );

    *(ptr->prev) = ptr->next;

    if( ptr->next != NULL ) {
	ptr->next->prev = ptr->prev;
    } else {
	tail = ptr->prev;
    } // if

    delete ptr;
    return( TRUE );
} // VisitQClass::DequeueCoord()


void VisitQClass::EnqueueArea( unsigned int row, unsigned int col,
  Map_Class * map )
{
    unsigned int top, bottom, left, right;

    GetAdjacent2Area( row, col, &top, &bottom, &left, &right );
} // VisitQClass::EnqueueArea()


void VisitQClass::DequeueArea( unsigned int row, unsigned int col )
{
    unsigned int top, bottom, left, right, r, c;

    GetAdjacentArea( row, col, &top, &bottom, &left, &right );

    for( r = top; r <= col; r++ ) {

	for( c = left; c <= right; c++ ) {
	    DequeueCoord( r, c );
	} // for
    } // for
} // VisitQClass::DequeueArea()


CoordType * VisitQClass::GetNextCoord( void )
{
    CoordType * ptr;

    for( ptr = head; ptr != NULL  &&  ptr->targeted; ptr = ptr->next );

    return( ptr );
} // VisitQClass::GetNextCoord()


VisitQClass::~VisitQClass( void )
{
    CoordType * ptr;

    while( head != NULL ) {
	ptr = head->next;
	delete head;
        head = ptr;
    } // while
} // VisitQClass::~VisitQClass()


// **** End of emvisitq.cpp ****
