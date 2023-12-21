
#include <stdio.h>
#include <stdlib.h>
#include <search.h>

struct PointDict
{
    void *key;
    void *value;
} typedef PointDict;


typedef void* Dict;

int compar(const void *l, const void *r)
{
    const PointDict *lm = l;
    const PointDict *lr = r;
    return lm->key - lr->key;
}

void pd_free(void *l)
{
    free(l);
}


void *
get_point_dict(void *p_i,Dict *dict)
{
    PointDict find;
    find.key = p_i;
    void * result = tfind(&find, dict, compar);
    if(result){
        result = (*(PointDict**)result)->value;
    }
    return result;
}


void
set_point_dict(void * p_i,void * p_j,Dict *dict)
{
   PointDict *find = malloc(sizeof(PointDict));
   find->key = p_i;
   find->value = p_j;
   void * result = tfind(find, dict, compar);
   if(result){
        (*(PointDict**)result)->value = p_j;
   }
   else{
        tsearch(find, dict, compar); /* insert */
   }
}


void destroy_PointDict(void *dict)
{
    tdestroy(dict,pd_free);
}


void destroy_elem_PointDict(void * p_i,Dict *dict)
{
    PointDict find;
    find.key = p_i;
    void * r = tfind(&find, dict, compar);
    if(r)
    {
        PointDict * to_free = (*(PointDict**)r);
        tdelete(&find,dict,compar);
        free(to_free);
    }
}