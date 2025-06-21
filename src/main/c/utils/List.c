#include "List.h"
#include <stdlib.h>

void freeList(TList list)
{
    if (list != NULL)
    {
        freeList(list->tail);
        free(list);
    }
}