

#include <stdio.h>
#include <stdlib.h>
#include <search.h>


typedef struct PriorityDict
{
  void * key;
  double priority;
} PriorityDict;


typedef void* PDict;

int comparp(const void *l, const void *r)
{
    const PriorityDict *lm = l;
    const PriorityDict *lr = r;
    return lm->key - lr->key;
}

void p_free(void *l)
{
    free(l);
}


double
get_priority_dict(void *p_i,PDict *dict)
{
    double result = -1;
    PriorityDict find;
    find.key = p_i;
    void * res = tfind(&find, dict, comparp);
    if(res){
        result = (*(PriorityDict**)res)->priority;
    }
    return result;
}


void
set_priority_dict(void * p_i,double priority,PDict *dict)
{
   PriorityDict *find = malloc(sizeof(PriorityDict));
   find->key = p_i;
   find->priority = priority;
   void * result = tfind(find, dict, comparp);
   if(result){
        (*(PriorityDict**)result)->priority = priority;
   }
   else{
        tsearch(find, dict, comparp); /* insert */
   }
}


void destroy_PriorityDict(PDict dict)
{
    tdestroy(dict,p_free);
}


void destroy_elem_PriorityDict(void * p_i,PDict *dict)
{
    PriorityDict find;
    find.key = p_i;
    void * r = tfind(&find, dict, comparp);
    if(r)
    {
        PriorityDict * to_free = (*(PriorityDict**)r);
        tdelete(&find,dict,comparp);
        free(to_free);
    }
}