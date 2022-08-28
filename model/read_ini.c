#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "en_typing.h"
#include "read_ini.h"
#include "read_wl.h"

#define LINE_SIZE   512

char    select_str[] =
            "mode\t\t.\t.\n"
                "normal\t\tmode\t.\n"
                "hard\t\tmode\t.\n"
            "questions\t\t.\t.\n"
                "Unlimited\t\tquestions\t.\n"
                "5\t\tquestions\t.\n"
                "10\t\tquestions\t.\n"
                "20\t\tquestions\t.\n"
                "30\t\tquestions\t.\n"
                "40\t\tquestions\t.\n"
                "50\t\tquestions\t.\n"
                "60\t\tquestions\t.\n"
                "70\t\tquestions\t.\n"
                "80\t\tquestions\t.\n"
                "90\t\tquestions\t.\n"
                "100\t\tquestions\t.\n"
            "space\t\t.\t.\n"
                "normal\t\tspace\t.\n"
                "free\t\tspace\t.\n"
            "sort\t\t.\t.\n"
                "shuffle\t\tsort\t.\n"
                "normal\t\tsort\t.\n"
                ;

WL_LNODE*
    create_sl()
{
    WL_LNODE    *set_list = create_lnode();

    int line_status = 0, column_status = 0;

    char    line[LINE_SIZE];
    int j = 0;
    for( int i = 0; select_str[i] != '\0'; i++ )
    {
        line[j] = select_str[i];
        j++;

        if( select_str[i] == '\n' )
        {
            line[j] = '\0';
            read_wl_from_line( set_list, line, &line_status, &column_status );
            j = 0;
        }
    }

    return  set_list;
}

void
    set_parameter( char *param_str )
{
    if( strcmp( param_str, "mode=normal" ) == 0 )
    {
        global_set.mode = INI_MODE_NORMAL;
    }
    else if( strcmp( param_str, "mode=hard" ) == 0 )
    {
        global_set.mode = INI_MODE_HARD;
    }
    else if( strncmp( param_str, "questions=", 10 ) == 0 )
    {
        global_set.questions = strtol( param_str + 10, NULL, 10 );
    }
    else if( strcmp( param_str, "space=normal" ) == 0 )
    {
        global_set.space = INI_SPACE_NORMAL;
    }
    else if( strcmp( param_str, "space=free" ) == 0 )
    {
        global_set.space = INI_SPACE_FREE;
    }
    else if( strcmp( param_str, "sort=normal" ) == 0 )
    {
        global_set.sort = INI_SORT_NORMAL;
    }
    else if( strcmp( param_str, "sort=shuffle" ) == 0 )
    {
        global_set.sort = INI_SORT_SHUFFLE;
    }
}

void
    apply_settings( WL_LNODE *set_list )
{
    char    line[LINE_SIZE];
    for( WL_LNODE *node = set_list->child; node != NULL; node = node->next )
    {
        if( node->word->en_word != NULL && node->child != NULL && node->child->word->en_word != NULL )
        {
            snprintf( line, LINE_SIZE, "%s=%s", node->word->en_word, node->child->word->en_word );
            set_parameter( line );
        }
    }
}

int
    save_settings( WL_LNODE *set_list )
{
    FILE    *fp = fopen( "en_typing.ini", "w" );
    if( fp == NULL )
    {
        return  -1;
    }

    for( WL_LNODE *node = set_list->child; node != NULL; node = node->next )
    {
        if( node->word->en_word != NULL && node->child != NULL && node->child->word->en_word != NULL )
        {
            fprintf( fp, "%s=%s\n", node->word->en_word, node->child->word->en_word );
        }
    }

    fclose( fp );

    return  0;
}
