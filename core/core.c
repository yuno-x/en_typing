#include <stdio.h>

#include "core.h"
#include "read_wl.h"
    
#define LINE_SIZE   512

void
    global_init( WL_LNODE *set_list, char *fname )
{
    global_set.mode = INI_MODE_NORMAL;
    global_set.questions = 0;
    global_set.space = INI_SPACE_NORMAL;
    global_set.sort = INI_SORT_SHUFFLE;

    global_set.type_count = 0;
    global_set.questions_max = 0;
    global_set.questions_count = 0;
    global_set.fail_count = 0;

    char    line[LINE_SIZE];
    WL_LNODE    *node, *param_node;

    if( fname != NULL )
    {
        FILE    *fp = fopen( fname, "r" );
        if( fp != NULL )
        {
            while( fgets( line, LINE_SIZE, fp ) != NULL )
            {
                size_t  i, start = 0;
                for( i = 0; line[i] !='\0'; i++ )
                {
                    if( line[i] == '=' )
                    {
                        line[i] = '\0';
                        node = find_node_from_en_word( set_list, line );
                        start = i + 1;
                    }
                    else if( line[i] == '\n' )
                    {
                        if( node != NULL )
                        {
                            line[i] = '\0';

                            param_node = find_node_from_en_word( node, line + start );
                            if( param_node != NULL )
                            {
                                node->child = param_node;
                            }
                        }
                    }
                }
            }

            fclose( fp );
        }
    }

    apply_settings( set_list );
}
