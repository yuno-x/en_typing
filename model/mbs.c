#define _XOPEN_SOURCE
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

size_t
    mbslen( char *str )
{
    size_t len, slen;
    slen = strlen( str );
    wchar_t *wstr = (wchar_t*)malloc( sizeof( wchar_t ) * slen );
    len = mbstowcs( wstr, str, slen );
    free( wstr );
    return  len;
}

size_t
    mbswidth( char *str )
{
    size_t width, slen;
    slen = strlen( str );
    wchar_t *wstr = (wchar_t*)malloc( sizeof( wchar_t ) * slen );
    mbstowcs( wstr, str, slen );
    width = wcswidth( wstr, slen );
    free( wstr );
    return  width;
}
