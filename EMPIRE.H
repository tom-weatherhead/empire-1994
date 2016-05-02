// empire.h
// (C) Copyright 1992-1994 by Tom Weatherhead

#define MAP_ROWS                20
#define MAP_COLS                70
#define CITY_DENSITY            60
#define NUM_CITIES              (MAP_ROWS*MAP_COLS)/CITY_DENSITY
#define NUM_EMPIRES             10
#define LAND_CH                 '+'
#define WATER_CH                '.'
#define NEUTRAL_CITY_CH         '*'
#define GENERATE_MAP_REPS       10
#define DATELINE		MAP_ROWS+3	// For Unix
#define INIT_POP		1000
#define MAX_ARMIES_PER_CITY	4

#define DIR_LINEAR		0
#define DIR_CW			1
#define DIR_CCW			-1

#ifndef NULL
#define NULL                    0
#endif

#if 0
//#ifndef FALSE
#undef TRUE
typedef enum { FALSE, TRUE } bool;
#endif

typedef enum {
    SRCH_LAND,
    SRCH_WATER,
    SRCH_OPPONENT
} srch_code_type;

#if 0
typedef struct {
    char * name;
    unsigned int init_hit_pts;
    unsigned int give_hits;
} army_def_type;
#endif


// **** Class Definitions ****

class Map_Class {
    public:
        char ch[MAP_ROWS][MAP_COLS+1];
	Map_Class * src_map;		// Objective world map

        Map_Class( void );
        void generate( void );
        void landfill( void );
        void display( void );
	void copy_local_area( unsigned int, unsigned int );
	void refresh_area( unsigned int, unsigned int );
}; // Map_Class


class City_Class;
class Empire_Class;


class Army_Class : public Q1EltClass
{
    public:
//        Army_Class * next;
        unsigned int row, col, num, target_row, target_col;
	int * wander_vector;
//        int hit_pts;
	int fc_dir;			// direction used in follow_coast()
	unsigned int vect_to_prev;	// index of vector to prev. position
        char ch;
        bool is_targeted, is_automated;
	City_Class * host_city;
        Empire_Class * owner;

	Army_Class( void );
	void init( Empire_Class *, unsigned int, unsigned int );
        void init( City_Class * );
	void kill( void );
	bool kill_other_army( void );
	bool manual_move( char );
	bool move_to_coords( unsigned int, unsigned int );
	bool MoveToTarget( void );
        void Automove( void );
	void init_vect_to_prev( void );
	bool follow_coast( void );
	void army_takes_city( void );
}; // Army_Class


class Army_Q_Class : public Q1Class
{
    public:
//        Army_Class * head;
        unsigned int size, next_new_army_num;

        Army_Q_Class( void );
        void enqueue( Army_Class * );
        Army_Class * dequeue( void );
        Army_Class * find( unsigned int, unsigned int );
        bool remove( Army_Class * );
	Army_Class * nearest_free_army( unsigned int, unsigned int );
        ~Army_Q_Class( void );
}; // Army_Q_Class


typedef struct CoordStruct {
    struct CoordStruct * next, ** prev;
    unsigned int row, col;
    bool targeted;
} CoordType;


#if 0
class VisitQClass : public Q1Class {
    private:
	CoordType * FindCoord( unsigned int row, unsigned int col );
	void EnqueueCoord( unsigned int row, unsigned int col );
	bool DequeueCoord( unsigned int row, unsigned int col );

    public:
	CoordClass * head, ** tail;

	VisitQClass( void );
	void EnqueueArea( unsigned int row, unsigned int col,
	  Map_Class * map );
	CoordClass * GetNextCoord( void );
	void DequeueArea( unsigned int row, unsigned int col );
	~VisitQClass( void );
}; // VisitQClass
#endif


class City_Class {
    public:
	City_Class * next;
        unsigned int row, col, num, next_production, population, held_since;
        char ch;
//        army_def_type * product;        ie. what the city is making
        Army_Class * guest_army;
        Empire_Class * owner;

	City_Class( unsigned int );
	void PlaceRandomly( Map_Class * );
	void change_owner( Empire_Class * );
	void update( void );
}; // City_Class


class World_Class;

class Empire_Class {
    public:
        bool automove;
        unsigned int num, num_cities;
        Map_Class map;
        Army_Q_Class army_q;
//	Army_Q_Class * idle_army_q;
	City_Class * city_head;
        char city_ch, army_ch;
	World_Class * WorldObj;

	Empire_Class( unsigned int, World_Class * );
	void update_local_map_area( unsigned int row,
	  unsigned int col );
	void update_local_map( void );
	bool target_location( unsigned int, unsigned int );
	void local_bogey_search( unsigned int, unsigned int );
	void automove_all_armies( void );
	void move_all_armies( void );
	bool adjacent_search( unsigned int, unsigned int, srch_code_type,
	  unsigned int *, unsigned int * );
	void show_msg_if_interactive( char * );
}; // Empire_Class


class World_Class {
    private:
	void load_world( void );
	void save_world( void );
	void init_all( void );
	void neutralize_city( void );
	void create_land( void );
	void one_days_move( void );

    public:
	unsigned int num_empires, num_cities, day, cur_map_num;
	City_Class * city[NUM_CITIES];
	Empire_Class * empire[NUM_EMPIRES];
	Army_Q_Class idle_army_q;
	Map_Class * map;

        World_Class( void );
	City_Class * find_city( unsigned int row, unsigned int col );
	void Animate( void );
	~World_Class( void );
}; // World_Class


// **** Prototypes ****

// In empire.cpp
int e_sign( int );
void get_adjacent_area( unsigned int, unsigned int, unsigned int *,
  unsigned int *, unsigned int *, unsigned int * );
unsigned int owner_num( char );

// In e_io*.cpp
unsigned int init_display( void );
unsigned int kill_display( void );
void show_line( char *, unsigned int );
void show_msg( char * );
void show_prompt( char * );
void get_string( char *, unsigned int );
void mark_army( unsigned int, unsigned int );
void refresh_screen( void );
//unsigned int get_rand_seed( void );


// **** End of empire.h ****
