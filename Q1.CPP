// q1.cpp
// (C) Copyright 1994 by Tom Weatherhead

#include <stdlib.h>
#include <assert.h>
#include "q1.h"


// **** Q1EltClass Member Functions ****


Q1EltClass::Q1EltClass( void )
{
    next = NULL;
    prev = NULL;
} // Q1EltClass::Q1EltClass()


// **** Q1Class Member Functions ****


Q1Class::Q1Class( void )
{
    head = NULL;
    tail = &head;
} // Q1Class::Q1Class()


bool Q1Class::IsEmpty( void )
{
    return( ( head == NULL ) ? TRUE : FALSE );
} // Q1Class::IsEmpty()


void Q1Class::Enqueue( Q1EltClass * ptr )
{
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
} // Q1Class::Enqueue()


void Q1Class::Dequeue( Q1EltClass * ptr )
{
    *(ptr->prev) = ptr->next;

    if( ptr->next != NULL ) {
	ptr->next->prev = ptr->prev;
        ptr->next = NULL;
    } else {
	tail = ptr->prev;
    } // if

    ptr->prev = NULL;
//    delete ptr;
//    return( TRUE );
} // Q1Class::Dequeue()


Q1Class::~Q1Class( void )
{
    Q1EltClass * ptr;

    while( head != NULL ) {
	ptr = head->next;
	delete head;
        head = ptr;
    } // while
} // Q1Class::~Q1Class()


// **** End of q1.cpp ****
