#include "List.h"
#include <stdlib.h>

void freeList(TList list, FreeFn freeFn)
{
    if (list != NULL)
    {
        freeList(list->tail, freeFn);
        if (freeFn != NULL)
        {
            freeFn(list->elem);
        }
        free(list);
    }
}