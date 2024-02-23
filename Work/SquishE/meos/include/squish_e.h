#pragma once
#include "point_dict.h"
#include "priority_dict.h"
#include "priority_queue.h"
double
SED(void *p_a,void *p_b,void *p_c,interpType interp ,bool hasz );

void
adjust_priority(void *p_i,struct PriorityQueue *Q, Dict *pred,Dict *succ,PDict  *p,
bool syncdist,interpType interp ,bool hasz );

void
reduce(struct PriorityQueue *Q,Dict *pred,Dict *succ,PDict  *p,
bool syncdist,interpType interp ,bool hasz );

void
iteration_simplification_sqe(void *p_i , void *p_j ,
                            size_t *beta,const double lambda,int i,
                            Dict *succ,Dict *pred,
                            PDict  *p,struct PriorityQueue *Q,
                             bool syncdist,interpType interp ,bool hasz ,uint32_t minpts);

typedef struct
{
  uint32_t i;
  long int MMSI;

  size_t beta;
  Dict succ;
  Dict pred;
  PDict  *p;
  PriorityQueue *Q;

  TInstant * p_i;
  TInstant * p_j;
  TInstant * point;

} squish_variables;

void
init_squish_variables(squish_variables *sq);
void
free_squish_variables(squish_variables *sq);
void
sq_iteration(TInstant * point,squish_variables *sq ,const double lambda, bool syncdist,
                                                      uint32_t minpts);


TSequence *
construct_simplify_path(squish_variables *sq,
bool lower_inc,bool upper_inc, interpType interp, bool normalize);

static TSequence *
tsequence_simplify_sqe(const TSequence *seq, double dist, bool syncdist,
  uint32_t minpts);

