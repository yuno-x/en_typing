#include "view.h"
    
int
    view( WL_LNODE *list, WL_LNODE *set_list )
{
    if( list == NULL || set_list == NULL )
    {
        return  -1;
    }

    display( list, set_list );

    return  0;
}
