#include "control.h"
#include <stdio.h>

struct control *add_interface_to_cntl_list(struct control *phead, struct control *pnew)
{
    if (phead == NULL)
    {
        phead = pnew;
    }
    else
    {
        pnew->next = phead;
        phead = pnew;
    }
    return phead;   
}
