#pragma once

typedef struct PointDict
{
    void *key;
    void *value;
} PointDict;


typedef void* Dict;


int compar(const void *l, const void *r);
void pd_free(void *l);
void *
get_point_dict(void *p_i,Dict *dict);
void
set_point_dict(void * p_i,void * p_j,Dict *dict);
void destroy_PointDict(void *dict);
void destroy_elem_PointDict(void * p_i,Dict *dict);