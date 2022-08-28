#ifndef READ_INI_H_
#define READ_INI_H_

enum
{
    INI_MODE_NORMAL,
    INI_MODE_HARD
};

enum
{
    INI_SPACE_NORMAL,
    INI_SPACE_FREE
};

enum
{
    INI_SORT_NORMAL,
    INI_SORT_SHUFFLE
};

WL_LNODE*   create_sl();
void    set_parameter( char* );
void    apply_settings( WL_LNODE* );
int save_settings( WL_LNODE* );

#endif //READ_INI_H_
