#ifndef EN_TYPING_H_
#define EN_TYPING_H_

#include <time.h>

#include "view.h"
#include "model.h"

typedef
    struct
{
    int mode;
    int questions;
    int space;
    int sort;

    int fail_count;
    int questions_max;
    int questions_count;
    int type_count;
    struct  timespec    diff_ts;
}
GLOBAL_SET;

extern  GLOBAL_SET  global_set;

#endif //EN_TYPING_H_
