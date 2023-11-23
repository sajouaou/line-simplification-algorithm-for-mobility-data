#define MAX(x, y) (((x) > (y)) ? (x) : (y))



double
SED(void *p_a,void *p_b,void *p_c)
{
  const TInstant *start =  (const TInstant *) p_a;
  const TInstant *end = (const TInstant *) p_c;
  const TInstant *inst = (const TInstant *) p_b;
  double startval = DatumGetFloat8(tinstant_value(start));
  double endval = DatumGetFloat8(tinstant_value(end));
  double value = DatumGetFloat8(tinstant_value(inst));

  double duration2 = (double) (end->t - start->t);
  double duration1 = (double) (inst->t - start->t);

  double ratio = duration1 / duration2;
  double value_interp = startval + (endval - startval) * ratio;
  double d = fabs(value - value_interp);

  return d;
}




void
adjust_priority(void *p_i,struct PriorityQueue *Q, struct PointDict *pred,struct PointDict *succ,struct PriorityDict *p)
{
  void * p_h = get_point_dict(p_i,pred);
  void * p_k = get_point_dict(p_i,succ);
  if( p_h != NULL &&  p_k != NULL )
  {
    double priority = get_priority_dict(p_i,p) + SED(p_h,p_i,p_k);
    set_priority_queue(p_i,priority,Q);
  }
}

void
reduce(struct PriorityQueue *Q,struct PointDict *pred,struct PointDict *succ,struct PriorityDict *p)
{
  struct PriorityQueue *entry = remove_min(Q);
  void * p_j = entry->point;
  double priority = entry->priority;
  void * p_i = get_point_dict(p_j,pred);
  void * p_k = get_point_dict(p_j,succ);

  set_priority_dict(p_k, MAX(priority, get_priority_dict(p_k,p) ) ,p);
  set_priority_dict(p_i, MAX(priority, get_priority_dict(p_i,p) ) ,p);

  set_point_dict(p_i,p_k,succ);
  set_point_dict(p_k,p_i,pred);

  adjust_priority(p_k ,Q,pred,succ,p);
  adjust_priority(p_i ,Q,pred,succ,p);

  //Delete pointer
  free(entry);  destroy_elem_PriorityDict(p_j,p);
  destroy_elem_PointDict(p_j,succ);
  destroy_elem_PointDict(p_j,pred);
}




void
iteration_simplification_sqe(void *p_i , void *p_j ,
                            int *beta,double lambda,int i,
                            struct PointDict *succ,struct PointDict *pred,
                            struct PriorityDict *p,struct PriorityQueue *Q)
{
  if( double(i)/lambda >= *beta)
  {
      *beta += 1;
  }
  set_priority_queue(p_i,INFINITY,Q);
  set_priority_dict(p_i,0,p);
  if(i >= 1)
  {
     set_point_dict(p_i,p_j,pred);
     set_point_dict(p_j,p_i,succ);
     adjust_priority(p_j,Q,pred,succ,p);
  }

  if(size_queue(Q) == *beta ){
    reduce(Q,pred,succ,p);
  }
}




/**
 * @brief Simplify the temporal sequence set float/point using the
 * Douglas-Peucker line simplification algorithm.
 */
static TSequence *
tsequence_simplify_sqe(const TSequence *seq, double dist, bool syncdist,
  uint32_t minpts)
{
  uint32_t i,outn;

  int beta = 4;
  struct PointDict *succ = malloc(sizeof(struct PointDict));
  struct PointDict *pred = malloc(sizeof(struct PointDict));
  struct PriorityDict *p = malloc(sizeof(struct PriorityDict));
  struct PriorityQueue *Q = malloc(sizeof(struct PriorityQueue));

  TInstant * p_i = NULL;
  TInstant * p_j = NULL;
  TInstant * point = NULL;


  assert(MEOS_FLAGS_LINEAR_INTERP(seq->flags));
  assert(seq->temptype == T_TFLOAT || tgeo_type(seq->temptype));
  /* Do not try to simplify really short things */
  if (seq->count < 3)
    return tsequence_copy(seq);


  i = 0;
  do
  {
    p_i = TSEQUENCE_INST_N(seq,i);
    if(i == 0){
      point = p_i;
    }
    iteration_simplification_sqe(p_i , p_j ,&beta,dist,i,
                                 succ,pred,
                                 p,Q);
    p_j = p_i;
    i++;
  }
  while(i < seq->count);




  /* Create a new temporal sequence */
  outn = size_queue(Q);
  i = 0;
  const TInstant **instants = palloc(sizeof(TInstant *) * outn);
  while(point != NULL && i < outn)
  {
    instants[i] = point;
    point = (TInstant *) get_point_dict(point,succ);
    i++;
  }
  TSequence *result = tsequence_make(instants, outn, seq->period.lower_inc,
       seq->period.upper_inc, LINEAR, NORMALIZE);
  pfree(instants);

  destroy_PointDict(pred);
  destroy_PointDict(succ);
  destroyQueue(Q);
  destroy_PriorityDict(p);

  return result;
}
















/**
 * @brief Simplify the temporal sequence set float/point using the
 * Douglas-Peucker line simplification algorithm.
 * @param[in] ss Temporal point
 * @param[in] dist Distance
 * @param[in] syncdist True when computing the Synchronized Euclidean
 * Distance (SED), false when computing the spatial only distance.
 * @param[in] minpts Minimum number of points
 */
static TSequenceSet *
tsequenceset_simplify_sqe(const TSequenceSet *ss, double dist, bool syncdist,
  uint32_t minpts)
{
  TSequence **sequences = palloc(sizeof(TSequence *) * ss->count);
  for (int i = 0; i < ss->count; i++)
  {
    const TSequence *seq = TSEQUENCESET_SEQ_N(ss, i);
    sequences[i] = tsequence_simplify_sqe(seq, dist, syncdist, minpts);
  }
  return tsequenceset_make_free(sequences, ss->count, NORMALIZE);
}

/**
 * @ingroup libmeos_temporal_analytics_simplify
 * @brief Simplify the temporal float/point using the Douglas-Peucker line
 * simplification algorithm.
 * @param[in] temp Temporal value
 * @param[in] dist Distance in the units of the values for temporal floats or
 * the units of the coordinate system for temporal points.
 * @param[in] syncdist True when the Synchronized Distance is used, false when
 * the spatial-only distance is used. Only used for temporal points.
 * @note The funcion applies only for temporal sequences or sequence sets with
 * linear interpolation. In all other cases, it returns a copy of the temporal
 * value.
 * @sqlfunc DouglasPeuckerSimplify()
 */
Temporal *
temporal_simplify_sqe(const Temporal *temp, double dist, bool syncdist)
{
  /* Ensure validity of the arguments */
  if (! ensure_not_null((void *) temp) ||
      ! ensure_tnumber_tgeo_type(temp->temptype) ||
      ! ensure_positive_datum(Float8GetDatum(dist), T_FLOAT8))
    return NULL;

  Temporal *result;
  assert(temptype_subtype(temp->subtype));
  if (temp->subtype == TINSTANT || ! MEOS_FLAGS_LINEAR_INTERP(temp->flags))
    result = temporal_copy(temp);
  else if (temp->subtype == TSEQUENCE)
    result = (Temporal *) tsequence_simplify_sqe((TSequence *) temp, dist,
      syncdist, 2);
  else /* temp->subtype == TSEQUENCESET */
    result = (Temporal *) tsequenceset_simplify_sqe((TSequenceSet *) temp, dist,
      syncdist, 2);
  return result;
}

