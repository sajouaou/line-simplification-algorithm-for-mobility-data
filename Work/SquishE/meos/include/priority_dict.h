#pragma once

typedef struct PriorityDict
{
  void * key;
  double priority;
} PriorityDict;


typedef void* PDict;

int comparp(const void *l, const void *r);
void p_free(void *l);
double
get_priority_dict(void *p_i,PDict *dict);
void
set_priority_dict(void * p_i,double priority,PDict *dict);
void destroy_PriorityDict(PDict dict);
void destroy_elem_PriorityDict(void * p_i,PDict *dict);