#ifndef READ_WL_H_
#define READ_WL_H_

struct  _WL_LNODE;

typedef
    struct
{
    char    *en_word;
    char    *ja_word;
    struct  _WL_LNODE    *categories;
    char    *img_name;
}
WL_WORD;

typedef
    struct _WL_LNODE
{
    WL_WORD *word;
    struct  _WL_LNODE   *prev;
    struct  _WL_LNODE   *child;
    struct  _WL_LNODE   *next;
    struct  _WL_LNODE   *option;
}
WL_LNODE;

int apply_opt( WL_LNODE* );

WL_LNODE*   create_lnode();
WL_LNODE*   dup_rand_list( WL_LNODE* );
void    del_list( WL_LNODE* );
void    del_word_list( WL_LNODE* );
WL_LNODE*   find_node_from_en_word( WL_LNODE*, char* );
int read_wl_from_line( WL_LNODE*, char*, int*, int* );
WL_LNODE* get_wl( char* );

#endif //READ_WL_H_
