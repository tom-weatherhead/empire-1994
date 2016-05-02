// q1.h - Queue Base Class
// (C) Copyright 1994 by Tom Weatherhead

#ifndef FALSE
#undef TRUE
typedef enum { FALSE, TRUE } bool;
#endif


// Base element class

class Q1EltClass {
  public:
    Q1EltClass * next, ** prev;

    Q1EltClass( void );
}; // Q1EltClass


class Q1Class {
    protected:
	void Enqueue( Q1EltClass * );
	void Dequeue( Q1EltClass * );

    public:
	Q1EltClass * head, ** tail;

	Q1Class( void );
        bool IsEmpty( void );
	~Q1Class( void );
}; // Q1Class


// **** End of q1.h ****
