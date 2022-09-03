#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <ncurses.h>

#include "en_typing.h"
#include "view.h"
#include "core.h"
#include "read_ini.h"
#include "timespec_util.h"

#define KEY_ESC 0x1b

enum
{
    INVALID_KEY_RIGHT = 1 << 0,
    INVALID_KEY_LEFT = 1 << 1,
};

typedef
    struct
{
    int x;
    int y;
    int x_org;
    int y_org;
}
CUR_POS;


void
    init_display()
{
    initscr();
    noecho();
    cbreak();
    curs_set( 0 );
    keypad( stdscr, TRUE );
    start_color();
    use_default_colors();
    init_pair( 1, COLOR_GREEN, -1 );
    init_pair( 2, COLOR_RED, -1 );
}

void
    put_menu_guide( int scr_width, int scr_height )
{
    char    *str = "Esc: Exit    "
                "S: Settings    ";
    int y, x;
    y = scr_height - 1;
    x = scr_width - strlen( str );
    move( y, x );
    attrset( 0 );
    addstr( str );
}

void
    put_settings_guide( int invalid_key_flag, int scr_width, int scr_height )
{
    char    *str_key_left = "Left: Previous Parameter    ",
            *str_key_right = "Right: Next Parameter    ",
            *str = "Enter: Apply and Exit    "
                "S: Save to ini file    ";
    int y, x;
    y = scr_height - 1;
    x = scr_width - strlen( str_key_left ) - strlen( str_key_right ) - strlen( str );
    attrset( 0 );

    if( x >= 0 && y >= 0 )
    {
        move( y, x );
        if( ! ( invalid_key_flag & INVALID_KEY_LEFT ) )
        {
            addstr( str_key_left );
        }
    }
    x += strlen( str_key_left );

    if( x >= 0 && y >= 0 )
    {
        move( y, x );
        if( ! ( invalid_key_flag & INVALID_KEY_RIGHT ) )
        {
            addstr( str_key_right );
        }
    }
    x += strlen( str_key_right );
    move( y, x );

    addstr( str );
}

int
    change_parameters( WL_LNODE *set_list )
{
    int c;
    int saved = 0, invalid_key_flag;
    
    int scr_width, scr_height;
    int x, y;
    WL_LNODE    *selected_node = set_list->child;
    do
    {
        getmaxyx( stdscr, scr_height, scr_width );
        clear();

        y = ( scr_height - 3 ) / 2;
        x = ( scr_width - 15 ) / 2;

        for( WL_LNODE *node = set_list->child; node != NULL; node = node->next )
        {
            move( y, x );
            attrset( 0 );
            printw( "%s\t : ", node->word->en_word  );
            if( node == selected_node )
            {
                attrset( A_REVERSE );
            }
            else
            {
                attrset( 0 );
            }
            printw( "%s", node->child->word->en_word );
            y++;
        }

        if( saved )
        {
            y = scr_height - 2;
            x = scr_width - 9;
            move( y, x );
            addstr( "saved" );
            saved = 0;
        }
        
        invalid_key_flag = 0;
        if( selected_node->child->prev == NULL )
        {
            invalid_key_flag |= INVALID_KEY_LEFT;
        }
        if( selected_node->child->next == NULL )
        {
            invalid_key_flag |= INVALID_KEY_RIGHT;
        }

        put_settings_guide( invalid_key_flag, scr_width, scr_height );

        refresh();

        c = getch();
        switch( c )
        {
        case    KEY_UP:
            if( selected_node->prev != NULL )
            {
                selected_node = selected_node->prev;
            }

            break;
        case    KEY_DOWN:
            if( selected_node->next != NULL )
            {
                selected_node = selected_node->next;
            }

            break;
        case    KEY_LEFT:
            if( selected_node->child->prev != NULL )
            {
                selected_node->child = selected_node->child->prev;
            }

            break;
        case    KEY_RIGHT:
            if( selected_node->child->next != NULL )
            {
                selected_node->child = selected_node->child->next;
            }

            break;
        case    '\n':
            apply_settings( set_list );
            return  0;

            break;
        case    'S':
            if( save_settings( set_list ) == 0 )
            {
                saved = 1;
            }

            break;
        default:
            break;
        }
    }
    while( 1 );
}

WL_LNODE*
    display_menu( WL_LNODE *list, WL_LNODE *set_list )
{
    int scr_width, scr_height;
    int y, x;

    int i, lnum = 0;
    int c;

    for( WL_LNODE *node = list->child; node != NULL; node = node->next )
    {
        if( node->child == NULL )
        {
            return  list;
        }

        lnum++;
    }

    WL_LNODE    *selected_node = list->child;
    do
    {
        getmaxyx( stdscr, scr_height, scr_width );
        clear();

        char    msg[] = "[Select a list.]";
        y = ( scr_height - lnum ) / 2 - 1;
        x = ( scr_width - strlen( msg ) ) / 2;
        move( y, x );
        attrset( 0 );
        addstr( msg );

        i = 0;
        for( WL_LNODE *node = list->child; node != NULL; node = node->next, i++ )
        {
            if( node->word->en_word != NULL )
            {
                y = ( scr_height - lnum ) / 2 + i;
                x = ( scr_width - strlen( node->word->en_word ) ) / 2;
                move( y, x );
                if( node == selected_node )
                {
                    attrset( A_REVERSE );
                }
                else
                {
                    attrset( 0 );
                }
                addstr( node->word->en_word );
            }
        }
        put_menu_guide( scr_width, scr_height );
        refresh();

        switch( c = getch() )
        {
        case    '\n':
            return  display_menu( selected_node, set_list );

            break;
        case    KEY_UP:
            if( selected_node->prev != NULL )
            {
                selected_node = selected_node->prev;
            }

            break;
        case    KEY_DOWN:
            if( selected_node->next != NULL )
            {
                selected_node = selected_node->next;
            }

            break;
        case    'S':
            change_parameters( set_list );

            break;
        case    KEY_ESC:
            return  NULL;

            break;
        default:
            break;
        }
    }
    while( 1 );
}

void
    put_count( int scr_width, int scr_height )
{
    char    *str = "Fail Count: ";
    int y, x;
    y = 0;
    x = scr_width - strlen( str ) - 8;
    move( y, x );
    attrset( 0 );
    printw( "%s%d", str, global_set.fail_count );

    str = "Questions: ";
    y = 1;
    x = scr_width - strlen( str ) - 8;
    move( y, x );
    attrset( 0 );
    printw( "%s%d / %d", str, global_set.questions_count, global_set.questions_max );
}


void
    go_ahead_space( char c, CUR_POS *cur_pos_p )
{
    if( c == '\n' )
    {
        chgat( 1, A_NORMAL, 0, NULL );
        cur_pos_p->x = cur_pos_p->x_org;
        cur_pos_p->y++;
    }
    else if( c == '\t' )
    {
        chgat( 4, A_NORMAL, 1, NULL );
        cur_pos_p->x += 4;
    }
    else if( isspace( c ) )
    {
        chgat( 1, A_NORMAL, 1, NULL );
        cur_pos_p->x++;
    }

    move( cur_pos_p->y, cur_pos_p->x );
}


void
    put_game_guide( int scr_width, int scr_height )
{
    char    *str = "Esc: Exit without Scoring    ";
    int y, x;

    y = scr_height - 1;
    x = scr_width - strlen( str );
    move( y, x );
    attrset( 0 );
    addstr( str );
}


void
    display_word( WL_WORD *word, int ci, CUR_POS *cur_pos_p )
{
    int scr_width, scr_height;
    int y, x, y_org, x_org, y_cur, x_cur;

    int space_continue = 0;

    getmaxyx( stdscr, scr_height, scr_width );
    clear();

    attrset( 0 );

    put_game_guide( scr_width, scr_height );

    if( word->ja_word != NULL )
    {
        y = scr_height / 2 - 1;
        x = ( scr_width - mbswidth( word->ja_word ) ) / 2;
        move( y, x );
        addstr( word->ja_word );
    }

    if( word->en_word != NULL )
    {
        size_t  i, start = 0, max_width = 0;
        for( i = 0; word->en_word[i] != '\0'; i++ )
        {
            if( word->en_word[i] == '\n' )
            {
                if( max_width < i - start )
                {
                    max_width = i - start;
                }

                start = i + 1;
            }
        }
        if( max_width < i - start )
        {
            max_width = i - start;
        }

        y = y_cur = y_org = scr_height / 2;
        x = x_cur = x_org = ( scr_width - max_width ) / 2;

        attrset( COLOR_PAIR( 1 ) );

        space_continue = 0;
        start = 0;
        for( i = 0; word->en_word[i] != '\0'; i++ )
        {
            if( i == ci )
            {
                move( y, x );

                addnstr( &word->en_word[start], i - start );
                attrset( COLOR_PAIR( 1 ) | A_REVERSE );
                x += i - start;
                start = i;

                y_cur = y;
                x_cur = x;

                if( global_set.mode == INI_MODE_HARD )
                {
                    addch( word->en_word[i] );
                    break;
                }
            }

            if( word->en_word[i] == '\n' )
            {
                move( y, x );
                addnstr( &word->en_word[start], i - start );
                start = i + 1;

                x = x_org;
                y++;
            }
            else if( isspace( word->en_word[i] ) )
            {
                move( y, x );
                addnstr( &word->en_word[start], i - start );

                if( !space_continue && global_set.space == INI_SPACE_FREE )
                {
                    space_continue = 1;
                    attrset( COLOR_PAIR( 1 ) );
                }
                
                if( word->en_word[i] == '\t' )
                {
                    addstr( "    " );
                    x += i - start + 4;
                }
                else
                {
                    addstr( " " );
                    x += i - start + 1;
                }
                start = i + 1;
            }
            else
            {
                if( space_continue && global_set.space == INI_SPACE_FREE )
                {
                    space_continue = 0;
                    
                    if( i > ci )
                    {
                        move( y, x - 1 );
                        attrset( COLOR_PAIR( 1 ) | A_REVERSE );
                        addch( ' ' );
                    }
                }
            }
        }
        move( y, x );
        addnstr( &word->en_word[start], i - start );
        x = x_cur;
        y = y_cur;
    }
    attrset( COLOR_PAIR( 1 ) );

    cur_pos_p->x = x;
    cur_pos_p->y = y;
    cur_pos_p->x_org = x_org;
    cur_pos_p->y_org = y_org;

    put_count( scr_width, scr_height );
    refresh();
}


void
    wrong_mark( char c, int y, int x )
{
    if( c == '\t' )
    {
        if( global_set.space == INI_SPACE_FREE )
        {
            move( y, x + 3 );
            chgat( 1, A_REVERSE, 2, NULL );
            move( y, x );
        }
        else
        {
            chgat( 4, A_REVERSE, 2, NULL );
        }
    }
    else
    {
        chgat( 1, A_REVERSE, 2, NULL );
    }
}

int
    display_game( WL_LNODE *list )
{
    int scr_width, scr_height;
    CUR_POS cur_pos;

    int space_continue = 0;
    for( WL_LNODE *node = list->child; node != NULL; node = node->next )
    {
        global_set.questions_count++;
        if( node->word == NULL )
        {
            continue;
        }

        display_word( node->word, 0, &cur_pos );

        getmaxyx( stdscr, scr_height, scr_width );
        space_continue = 0;
        int i = 0, i_bak;
        int c;
        while( ( c = getch() ) != -1 )
        {
            if( isprint( c ) )
            {
                global_set.type_count++;
            }
            else if( c == KEY_RESIZE )
            {
                display_word( node->word, i, &cur_pos );
                getmaxyx( stdscr, scr_height, scr_width );
                continue;
            }

            move( cur_pos.y, cur_pos.x );
            if( global_set.space == INI_SPACE_FREE && isspace( c ) )
            {
                for( ; isspace( node->word->en_word[i] ); i++ )
                {
                    go_ahead_space( node->word->en_word[i], &cur_pos );
                    space_continue = 1;
                }

                if( global_set.mode == INI_MODE_HARD )
                {
                    attrset( COLOR_PAIR( 1 ) | A_REVERSE );
                    addch( node->word->en_word[i] );
                    move( cur_pos.y, cur_pos.x );
                }

                if( space_continue )
                {
                    global_set.type_count--;
                    continue;
                }
            }
            else
            {
                space_continue = 0;
            }

            if( c == node->word->en_word[i] )
            {
                if( isspace( c ) )
                {
                    go_ahead_space( c, &cur_pos );
                }
                else
                {
                    chgat( 1, A_NORMAL, 1, NULL );
                    cur_pos.x++;
                    move( cur_pos.y, cur_pos.x );
                }

                if( global_set.mode == INI_MODE_HARD )
                {
                    attrset( COLOR_PAIR( 1 ) | A_REVERSE );
                    if( isspace( node->word->en_word[i + 1] ) )
                    {
                        if( global_set.space == INI_SPACE_FREE )
                        {
                            i_bak = i;

                            for( ; isspace( node->word->en_word[i_bak + 1] ); i_bak++ );

                            for( i++; i < i_bak; i++ )
                            {
                                go_ahead_space( node->word->en_word[i], &cur_pos );
                            }

                            if( isspace( node->word->en_word[i] ) && node->word->en_word[i] != '\n' )
                            {
                                if( node->word->en_word[i] == '\t' )
                                {
                                    move( cur_pos.y, cur_pos.x + 3 );
                                }
                                else
                                {
                                    move( cur_pos.y, cur_pos.x );
                                }
                                attrset( COLOR_PAIR( 1 ) | A_REVERSE );
                                addch( ' ' );
                            }

                            i--;
                        }
                        else
                        {
                            if( node->word->en_word[i + 1] == '\t' )
                            {
                                addstr( "    " );
                            }
                            else
                            {
                                addch( node->word->en_word[i + 1] );
                            }
                        }                       
                    }
                    else
                    {
                        addch( node->word->en_word[i + 1] );
                    }
                }

                i++;
                if( node->word->en_word[i] == '\0' )
                {
                    break;
                }

                if( node->word->en_word[i] == '\n' )
                {
                    if( node->word->en_word[i + 1] == '\0' )
                    {
                        break;
                    }
                }

                move( cur_pos.y, cur_pos.x );
            }
            else if( c == 0x1b )
            {
                node = NULL;

                break;
            }
            else
            {
                global_set.fail_count++;

                wrong_mark( node->word->en_word[i], cur_pos.y, cur_pos.x );
            }

            put_count( scr_width, scr_height );
            refresh();
        }

        attrset( 0 );
        if( node == NULL )
        {
            return  -1;
        }
    }

    return  0;
}

void
    display_count_down( int count )
{
    int scr_width, scr_height;
    int x, y;
    for( ; count > 0; count-- )
    {
        getmaxyx( stdscr, scr_height, scr_width );
        clear();

        y = ( scr_height - 1 ) / 2;
        x = ( scr_width - 1 ) / 2;
        move( y, x );
        attrset( 0 );
        printw( "%d", count );

        refresh();
        sleep( 1 );
    }
}


char*
    get_rank( double type_per_sec  )
{
    if( type_per_sec < 0.1 )
    {
        return  "F";
    }
    else if( type_per_sec < 0.2 )
    {
        return	"E-";
    }
    else if( type_per_sec < 0.4 )
    {
        return	"E+";
    }
    else if( type_per_sec < 0.7 )
    {
        return	"D-";
    }
    else if( type_per_sec < 1.0 )
    {
        return	"D+";
    }
    else if( type_per_sec < 1.4 )
    {
        return	"C-";
    }
    else if( type_per_sec < 1.9 )
    {
        return	"C+";
    }
    else if( type_per_sec < 2.5 )
    {
        return	"B-";
    }
    else if( type_per_sec < 3.1 )
    {
        return	"B+";
    }
    else if( type_per_sec < 3.6 )
    {
        return	"A-";
    }
    else if( type_per_sec < 4.1 )
    {
        return	"A+";
    }
    else if( type_per_sec < 4.6 )
    {
        return	"S-";
    }
    else if( type_per_sec < 5.1 )
    {
        return	"S+";
    }
    else if( type_per_sec < 5.5 )
    {
        return	"Professional";
    }
    else if( type_per_sec < 5.9 )
    {
        return	"Super Star";
    }
    else if( type_per_sec < 6.4 )
    {
        return	"Wizard";
    }
    else if( type_per_sec < 6.9 )
    {
        return	"God";
    }
    else
    {
        return	"Super God";
    }
}

void
    display_result()
{
    int scr_width, scr_height;
    int x, y;
    
    do
    {
        getmaxyx( stdscr, scr_height, scr_width );
        clear();

        char    *str = "Time: ";
        y = scr_height / 2;
        x = ( scr_width - strlen( str ) - 6 ) / 2;
        move( y, x );
        attrset( A_NORMAL );
        printw( "%s%ld.%02ld", str, global_set.diff_ts.tv_sec, global_set.diff_ts.tv_nsec / 10000000 );

        str = "Miss Count: ";
        y++;
        x = ( scr_width - strlen( str ) - 3 ) / 2;
        move( y, x );
        attrset( A_NORMAL );
        printw( "%s%d", str, global_set.fail_count );

        double  type_per_sec = (double)(global_set.type_count - global_set.fail_count ) / ( (double)global_set.diff_ts.tv_sec + (double)global_set.diff_ts.tv_nsec / 1000000000 );

        str = "Type per Second: ";
        y++;
        x = ( scr_width - strlen( str ) - 4 ) / 2;
        move( y, x );
        attrset( A_NORMAL );
        printw( "%s%.2lf", str, type_per_sec );


        str = get_rank( type_per_sec );

        y++;
        x = ( scr_width - strlen( str ) - 6 ) / 2;
        move( y, x );
        attrset( A_NORMAL );
        printw( "Rank: %s", str );

        refresh();
    }
    while( getch() == KEY_RESIZE );
}

void
    display( WL_LNODE *list, WL_LNODE *set_list )
{
    init_display();
    struct  timespec    start_ts, end_ts, diff_ts;

    WL_LNODE    *selected_node;
    while( ( selected_node = display_menu( list, set_list ) ) != NULL )
    {
        apply_opt( selected_node );
        if( global_set.sort == INI_SORT_SHUFFLE )
        {
            selected_node = dup_rand_list( selected_node );
        }

        for( WL_LNODE *cnode = selected_node->child; cnode != NULL; cnode = cnode->next )
        {
            global_set.questions_max++;
        }

        display_count_down( 3 );

        timespec_get( &start_ts, TIME_UTC );
        if( display_game( selected_node ) == 0 )
        {
            timespec_get( &end_ts, TIME_UTC );
            timespec_diff( &start_ts, &end_ts, &global_set.diff_ts );

            display_result();
        }

        if( global_set.sort == INI_SORT_SHUFFLE )
        {
            del_list( selected_node );
        }

        global_init( set_list, NULL );
    }
    endwin();
}
