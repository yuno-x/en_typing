#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include "en_typing.h"
#include "core.h"
#include "read_ini.h"
#include "read_wl.h"

GLOBAL_SET  global_set;

int
    main( int argc, char **argv )
{
    srand( (unsigned)time( NULL ) );
    setlocale( LC_ALL, "" );

    WL_LNODE    *set_list, *list;
    set_list = create_sl();
    global_init( set_list, "en_typing.ini" );
    list = get_wl( "word_lists" );

    view( list, set_list );

    del_word_list( list );
    del_word_list( set_list );

    return  0;
}
