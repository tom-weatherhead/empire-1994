
/* empire_io_vms.cxx */

#include <string.h>
#include <ssdef.h>
#include <descrip.h>
#include <smgdef.h>
#include <smg$routines.h>
#include "empire.h"

#define ROW_OFFSET 2    /* ie. start in row 2 */
#define COL_OFFSET 2
#define MSG_ROW    22
#define PROMPT_ROW 23

#define check_status(s)         {int st; st=(s); if((st&1)==0) return(st);}

static unsigned int pasteboard_id, display_id, keyboard_id;


void Map_Class::display( void )
{
    unsigned int i, displ_row, displ_col = COL_OFFSET, stat;
    struct dsc$descriptor line_desc = {MAP_COLS, 0, 0, 0 };

    for( i = 0; i < MAP_ROWS; i++ ) {
        displ_row = i + ROW_OFFSET;
        line_desc.dsc$a_pointer = ch[i];
        stat = smg$put_chars( &display_id, &line_desc,
          &displ_row, &displ_col, 0, 0, 0, 0 );
    }
}


unsigned int init_display( void )
{
    int rows, cols, one = 1, row, col, i;
    $DESCRIPTOR( digit_desc, " " );

    check_status( smg$create_pasteboard( &pasteboard_id, 0, &rows, &cols, 0,
      0 ) );
    check_status( smg$create_virtual_display( &rows, &cols, &display_id, 0, 0,
      0 ) );
    check_status( smg$paste_virtual_display( &display_id, &pasteboard_id,
      &one, &one, 0 ) );
    check_status( smg$create_virtual_keyboard( &keyboard_id, 0, 0, 0, 0 ) );

    col = 1;

    for( i = 0; i < 2; i++ ) {
        row = 10 * i + ROW_OFFSET;
        digit_desc.dsc$a_pointer[0] = (char)(i + 48);
        check_status( smg$put_chars( &display_id, &digit_desc, &row, &col,
          0, 0, 0, 0 ) );
    }

    row = 1;

    for( i = 0; i < 6; i++ ) {
        col = 10 * i + COL_OFFSET;
        digit_desc.dsc$a_pointer[0] = (char)(i + 48);
        check_status( smg$put_chars( &display_id, &digit_desc, &row, &col,
          0, 0, 0, 0 ) );
    }

    return( SS$_NORMAL );
}


unsigned int kill_display( void )
{
    check_status( smg$delete_virtual_keyboard( &keyboard_id ) );
    check_status( smg$delete_virtual_display( &display_id ) );
    check_status( smg$delete_pasteboard( &pasteboard_id, 0 ) );
    return( SS$_NORMAL );
}


unsigned int show_line( char * str, unsigned int row )
{
    unsigned int col = 1, flags = SMG$M_ERASE_LINE;
    struct dsc$descriptor line_desc = {strlen(str), 0, 0, str};
    static $DESCRIPTOR( spaces,
      "                                                              " );

    check_status( smg$put_chars( &display_id, &spaces, &row, &col,
      0, 0, 0, 0 ) );
    return( smg$put_chars( &display_id, &line_desc, &row, &col,
      &flags, 0, 0, 0 ) );
}


void show_msg( char * str ) {
    show_line( str, MSG_ROW );
}


void show_prompt( char * str ) {
    show_line( str, PROMPT_ROW );
}


void get_string( char * str, unsigned int len )
{
    struct dsc$descriptor line_desc = {len, 0, 0, str};

    smg$read_string( &keyboard_id, &line_desc, 0, &len, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0 );
    str[line_desc.dsc$w_length] = '\0';
}


void mark_army( unsigned int row, unsigned int col )
{
    $DESCRIPTOR( mark_desc, "#" );

    row += ROW_OFFSET;
    col += COL_OFFSET;
    smg$put_chars( &display_id, &mark_desc, &row, &col, 0, 0, 0, 0 );
}


void refresh_area( Map_Class * map, unsigned int row_p, unsigned int col_p )
{
    unsigned int row, top, bottom, left, right;
    struct dsc$descriptor line_desc;

    get_adjacent_area( row_p, col_p, &top, &bottom, &left, &right );
    top += ROW_OFFSET;
    bottom += ROW_OFFSET;
    left += COL_OFFSET;
    right += COL_OFFSET;
    line_desc.dsc$w_length = right - left + 1;

    for( row = top; row <= bottom; row++ ) {
        line_desc.dsc$a_pointer = &map->ch[row-ROW_OFFSET][left-COL_OFFSET];
        smg$put_chars( &display_id, &line_desc,
          &row, &left, 0, 0, 0, 0 );
    }
}


void refresh_screen( void )
{
    smg$repaint_screen( &pasteboard_id );
}


