#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "en_typing.h"
#include "read_wl.h"
#include "read_ini.h"

#define LINE_SIZE   1024
#define CONTENT_SIZE    10240

int read_wl_from_file( char*, WL_LNODE* );


WL_LNODE*
    create_lnode()
{
    WL_LNODE    *node = (WL_LNODE*)malloc( sizeof( WL_LNODE ) );
    node->word = NULL;
    node->prev = NULL;
    node->child = NULL;
    node->next = NULL;
    node->option = NULL;
}

WL_LNODE*
    add_empty_cnode( WL_LNODE *node )
{
    if( node->child == NULL )
    {
        node->child = create_lnode();
        return  node->child;
    }

    WL_LNODE    *cnode;
    for( cnode = node->child; cnode->next != NULL; cnode = cnode->next );
    cnode->next = create_lnode();
    cnode->next->prev = cnode;

    return  cnode->next;
}

WL_LNODE*
    add_cnode( WL_LNODE *node, WL_LNODE *new_node )
{
    if( node->child == NULL )
    {
        node->child = new_node;
        new_node->prev = NULL;
        new_node->next = NULL;
        return  node->child;
    }

    WL_LNODE    *cnode;
    for( cnode = node->child; cnode->next != NULL; cnode = cnode->next );
    cnode->next = new_node;
    new_node->prev = cnode;
    new_node->next = NULL;

    return  cnode->next;
}


WL_LNODE*
    dup_node( WL_LNODE *node )
{
    WL_LNODE    *new_node = create_lnode();
    new_node->word = node->word;
    new_node->option = node->option;

    return  new_node;
}


WL_LNODE*
    dup_list( WL_LNODE *node )
{
    WL_LNODE    *new_lnode = create_lnode(), *dnode;
    for( WL_LNODE *cnode = node->child; cnode != NULL; cnode = cnode->next )
    {
        dnode = dup_node( cnode );
        add_cnode( new_lnode, dnode );
    }

    return  new_lnode;
}


WL_LNODE*
    strip_node( WL_LNODE *lnode, WL_LNODE *node )
{
    if( node->word->categories != NULL )
    {
        for( WL_LNODE *cnode = node->word->categories->child; cnode != NULL; cnode = cnode->next )
        {
            if( cnode == node )
            {
                strip_node( node->word->categories, node );
            }
        }
    }

    if( node->next != NULL )
    {
        if( lnode->child == node )
        {
            lnode->child = node->next;
        }
        node->next->prev = node->prev;
    }

    if( node->prev != NULL )
    {
        if( lnode->child == node )
        {
            lnode->child = node->prev;
        }
        node->prev->next = node->next;
    }

    if( lnode->child == node )
    {
        lnode->child = NULL;
    }

    return  node;
}


void
    del_list( WL_LNODE *lnode )
{
    WL_LNODE    *cnode;
    for( cnode = lnode->child; cnode != NULL; cnode = cnode->prev );
    lnode->child = cnode;

    for( cnode = lnode->child; cnode != NULL; cnode = cnode->next )
    {
        if( cnode->child != NULL )
        {
            del_list( cnode );
        }

        strip_node( lnode, cnode );
        free( cnode );
    }

    free( lnode );
}


void
    del_word_list( WL_LNODE *lnode )
{
    WL_LNODE    *cnode;
    for( cnode = lnode->child; cnode != NULL; cnode = cnode->prev );
    lnode->child = cnode;

    for( cnode = lnode->child; cnode != NULL; cnode = cnode->next )
    {
        if( cnode->child != NULL )
        {
            del_list( cnode );
        }

        if( cnode->word != NULL )
        {
            free( cnode->word );
        }
        strip_node( lnode, cnode );
        free( cnode );
    }

    if( lnode->word != NULL )
    {
        free( lnode->word );
    }
    free( lnode );
}


WL_LNODE*
    dup_rand_list( WL_LNODE *node )
{
    WL_LNODE    *lnode = dup_list( node ), *dlnode = create_lnode(), *cnode;
    size_t  i, r, b, lnum = 0;
    for( cnode = lnode->child; cnode != NULL; cnode = cnode->next )
    {
        lnum++;
    }

    if( global_set.questions == 0 || global_set.questions > lnum )
    {
        b = 0;
    }
    else
    {
        b = lnum - global_set.questions;
    }

    for( ; lnum > b; lnum-- )
    {
        r = rand() % lnum;
        i = 0;
        for( cnode = lnode->child; i < r; cnode = cnode->next )
        {
            i++;
        }

        strip_node( lnode, cnode );
        add_cnode( dlnode, cnode );
    }

    del_list( lnode );
    return  dlnode;
}


WL_LNODE*
    add_optnode( WL_LNODE *node, WL_LNODE *new_node )
{
    if( node->option == NULL )
    {
        node->option = new_node;
        return  node->option;
    }

    WL_LNODE    *cnode;
    for( cnode = node->option; cnode->next != NULL; cnode = cnode->next );
    cnode->next = new_node;
    new_node->prev = cnode;

    return  cnode->next;
}


WL_WORD*
    create_word()
{
    WL_WORD *word = (WL_WORD*)malloc( sizeof( WL_WORD ) );
    word->en_word = NULL;
    word->ja_word = NULL;
    word->categories = NULL;
    word->img_name = NULL;

    return  word;
}

WL_LNODE*
    find_node_from_en_word( WL_LNODE *node, char *en_word )
{
    if( strcmp( en_word, "." ) == 0 )
    {
        return  node;
    }

    if( node->child == NULL )
    {
        return  NULL;
    }

    WL_LNODE    *cnode, *ccnode;
    for( cnode = node->child; cnode != NULL; cnode = cnode->next )
    {
        if( cnode->word->en_word != NULL )
        {
            if( strcmp( cnode->word->en_word, en_word ) == 0 )
            {
                return  cnode;
            }
        }
    }

    for( cnode = node->child; cnode != NULL; cnode = cnode->next )
    {
        if( cnode->child != NULL )
        {
            ccnode = find_node_from_en_word( cnode, en_word );
            if( ccnode == cnode )
            {
                return  node;
            }
            else if( ccnode != NULL )
            {
                return  ccnode;
            }
        }
    }

    return  NULL;
}


WL_LNODE*
    set_cnode_column( WL_LNODE *node, WL_LNODE *list, int column_status, char *str )
{
    if( str == NULL )
    {
        return  NULL;
    }

    WL_LNODE    *cate;
    switch( column_status )
    {
    case    0:
        node = create_lnode();
        node->word = create_word();

        node->word->en_word = (char*)malloc( strlen( str ) + 1 );
        strcpy( node->word->en_word, str );

        break;
    case    1:
        if( node == NULL )
        {
            return  NULL;
        }
        node->word->ja_word = (char*)malloc( strlen( str ) + 1 );
        strcpy( node->word->ja_word, str );

        break;
    case    2:
    case    -2:
        if( node == NULL || list == NULL )
        {
            return  NULL;
        }
        cate = find_node_from_en_word( list, str );
        if( cate == NULL )
        {
            return  NULL;
        }
        node->word->categories = create_lnode();
        add_empty_cnode( node->word->categories )->child = cate;
        if( column_status == -2 )
        {
            add_optnode( cate, node );
        }
        else
        {
            add_cnode( cate, node );
        }

        break;
    case    3:
        if( node == NULL )
        {
            return  NULL;
        }
        node->word->img_name = (char*)malloc( strlen( str ) + 1 );
        strcpy( node->word->img_name, str );

        break;
    default:
    }

    return  node;
}

WL_LNODE*
    set_cnode_column_from_file( WL_LNODE *node, WL_LNODE *list, int column_status, char *str )
{
    if( str == NULL )
    {
        return  NULL;
    }

    FILE    *fp;
    WL_LNODE    *cate;
    switch( column_status )
    {
    case    0:
        node = create_lnode();
        node->word = create_word();

        fp = fopen( str, "r" );
        if( fp == NULL )
        {
            return  NULL;
        }

        node->word->en_word = (char*)malloc( CONTENT_SIZE + 1 );
        fread( node->word->en_word, 1, CONTENT_SIZE, fp );
        node->word->en_word[CONTENT_SIZE] = '\0';

        fclose( fp );

        break;
    case    1:
    case    2:
    case    3:
        node = set_cnode_column( node, list, column_status, str );

        break;
    default:
    }

    return  node;
}

int
    apply_opt( WL_LNODE *list )
{
    if( list == NULL || list->option == NULL )
    {
        return  -1;
    }

    for( WL_LNODE *cnode = list->option; cnode != NULL; cnode = cnode->next )
    {
        set_parameter( cnode->word->en_word );
    }

    return  0;
}

int
    set_opt( WL_LNODE *list, char *node_name, char *param_str )
{
    if( node_name == NULL || param_str == NULL )
    {
        return  -1;
    }

    set_cnode_column( set_cnode_column( NULL, NULL, 0, param_str ), list, -2, node_name );

    return  0;
}

int
    read_wl_from_line( WL_LNODE *list, char *line, int *line_status_p, int *column_status_p )
{
    WL_LNODE    *cnode;
    size_t  i, start;
    if( *line_status_p == 0 )
    {
        if( line[0] == '#' )
        {
            if( line[strlen( line ) - 1] != '\n' )
            {
                *line_status_p = 2;
            }
        }
        else if( line[0] == '%' )
        {
            if( line[strlen( line ) - 1] != '\n' )
            {
                return  -2;
            }

            line[strlen( line ) - 1] = '\0';
            if( strncmp( &line[1], "include\t", 8 ) == 0 )
            {
                read_wl_from_file( &line[9], list );
            }
            else if( strncmp( &line[1], "force\t", 6 ) == 0 )
            {
                for( i = 7; line[i] != '\0'; i++ )
                {
                    if( line[i] == '\t' )
                    {
                        line[i] = '\0';
                        i++;

                        break;
                    }
                }
                set_opt( list, &line[7], &line[i] );
            }
        }
        else if( line[0] == '\n' )
        {
            // do nothing
        }
        else
        {
            start = 0;
            if( line[strlen( line ) - 1] != '\n' )
            {
                return  -2;
            }

            for( i = 0; line[i] != '\0'; i++ )
            {
                switch( line[i] )
                {
                case    '\t':
                    if( line[i + 1] == '\0' )
                    {
                        continue;
                    }

                    line[i] = '\0';
                    if( strncmp( &line[start], "./", 2 ) == 0 )
                    {
                        cnode = set_cnode_column_from_file( cnode, list, *column_status_p, &line[start] );
                    }
                    else
                    {
                        cnode = set_cnode_column( cnode, list, *column_status_p, &line[start] );
                    }
                    start = i + 1;

                    (*column_status_p)++;

                    break;
                case    '\n':
                    line[i] = '\0';
                    cnode = set_cnode_column( cnode, list, *column_status_p, &line[start] );
                    if( cnode == NULL )
                    {
                        return  1;
                    }

                    *column_status_p = 0;
                    *line_status_p = 0;
                    break;
                default:
                    break;
                }
            }
        }
    }
    else if( *line_status_p == 2 )
    {
        if( line[strlen( line ) - 1] == '\n' )
        {
            *line_status_p = 0;
        }
    }

    return  0;
}

int
    read_wl_from_file( char *fname, WL_LNODE *list )
{
    FILE    *fp = fopen( fname, "r" );
    if( fp == NULL )
    {
        return  -1;
    }

    int line_status = 0, column_status = 0;

    char    line[LINE_SIZE];
    while( fgets( line, LINE_SIZE, fp ) != NULL )
    {
        if( read_wl_from_line( list, line, &line_status, &column_status ) < 0 )
        {
            return  -2;
        }
    }

    return  0;
}

WL_LNODE*
    get_wl( char *dirname )
{
    if( chdir( dirname ) < 0 )
    {
        return  NULL;
    }

    WL_LNODE *list = create_lnode();
    if( read_wl_from_file( "main.list", list ) == 0 )
    {
        chdir( ".." );

        return  list;
    }

    return  NULL;
}
