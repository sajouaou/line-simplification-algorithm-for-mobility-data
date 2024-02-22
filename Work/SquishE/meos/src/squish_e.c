

/*

SQUISH-E.c

*/

#define MAX_SIZE_PQ 100



double
SED(void *p_a,void *p_b,void *p_c,interpType interp ,bool hasz )
{
  POINT2D *p2k, *p2_sync, *p2a, *p2b;
  POINT3DZ *p3k, *p3_sync, *p3a, *p3b;


  const TInstant *start =  (const TInstant *) p_a;
  const TInstant *end = (const TInstant *) p_c;
  const TInstant *inst = (const TInstant *) p_b;

    if (hasz)
    {
      p3a = (POINT3DZ *) DATUM_POINT3DZ_P(tinstant_value(start));
      p3k = (POINT3DZ *) DATUM_POINT3DZ_P(tinstant_value(inst));
      p3b = (POINT3DZ *) DATUM_POINT3DZ_P(tinstant_value(end));
    }
    else
    {
      p2a = (POINT2D *) DATUM_POINT2D_P(tinstant_value(start));
      p2k = (POINT2D *) DATUM_POINT2D_P(tinstant_value(inst));
      p2b = (POINT2D *) DATUM_POINT2D_P(tinstant_value(end));
    }

  double startval = DatumGetFloat8(tinstant_value(start));
  double endval = DatumGetFloat8(tinstant_value(end));
  double value = DatumGetFloat8(tinstant_value(inst));

  Datum tmp_value;

  double duration2 = (double) (end->t - start->t);
  double duration1 = (double) (inst->t - start->t);

  double ratio = duration1 / duration2;
  double value_interp = startval + (endval - startval) * ratio;
  double d = fabs(value - value_interp);
  double d_tmp;

   if (hasz)
      {
          tmp_value = tsegment_value_at_timestamptz(start, end, interp, inst->t);
          p3_sync = (POINT3DZ *) DATUM_POINT3DZ_P(tmp_value);
          d_tmp = dist3d_pt_pt(p3k, p3_sync);
          pfree(DatumGetPointer(tmp_value));
      }
      else
      {
          tmp_value = tsegment_value_at_timestamptz(start, end, interp, inst->t);
          p2_sync = (POINT2D *) DATUM_POINT2D_P(tmp_value);
          d_tmp = dist2d_pt_pt(p2k, p2_sync);
          pfree(DatumGetPointer(tmp_value));
      }

  //elog(NOTICE,"DISTANCE WITH Duration : %f \n DISTANCE WITH Point : %f",d,d_tmp);
  d = d_tmp;

  return d;
}






void
adjust_priority(void *p_i,struct PriorityQueue *Q, Dict *pred,Dict *succ,PDict  *p,
bool syncdist,interpType interp ,bool hasz )
{

  ////elog(NOTICE,"//////  ADJUST PRIORITY ////////");

  ////elog(NOTICE,"get TEST BEGIN");
  void * p_h = get_point_dict(p_i,pred);
  void * p_k = get_point_dict(p_i,succ);
  ////elog(NOTICE,"get TEST END");
  if( p_h != NULL &&  p_k != NULL )
  {
    if(syncdist)
    {
      double priority = get_priority_dict(p_i,p) + SED(p_h,p_i,p_k, interp , hasz );
  ////elog(NOTICE,"//////  BEFORE SET PRIORITY ////////");
      set_priority_queue(p_i,priority,Q);
  ////elog(NOTICE,"//////  AFTER SET PRIORITY ////////");

    }
  }
  ////elog(NOTICE,"//////  ADJUST PRIORITY ////////");
}

void
reduce(struct PriorityQueue *Q,Dict *pred,Dict *succ,PDict  *p,
bool syncdist,interpType interp ,bool hasz )
{
  struct PriorityQueueElem *entry = remove_min(Q);
  size_t size_before = size_queue(Q);



  void * p_j = entry->point;
  double priority = entry->priority;

  ////elog(NOTICE,"reduce TEST BEGIN POINT : %p PRIORITY : %d",p_j,priority);
  void * p_i = get_point_dict(p_j,pred);
  void * p_k = get_point_dict(p_j,succ);

  double pr_i = get_priority_dict(p_i,p); if(priority > pr_i){ pr_i = priority; }
  double pr_k = get_priority_dict(p_k,p); if(priority > pr_k){ pr_k = priority; }

  set_priority_dict(p_k, pr_k ,p);
  set_priority_dict(p_i, pr_i ,p);

  set_point_dict(p_i,p_k,succ);
  set_point_dict(p_k,p_i,pred);
  set_point_dict(p_k,p_i,pred);



  adjust_priority(p_k ,Q,pred,succ,p,syncdist, interp ,hasz );
  adjust_priority(p_i ,Q,pred,succ,p,syncdist, interp ,hasz );


  //Delete pointer
  free(entry);
  destroy_elem_PriorityDict(p_j,p);
  destroy_elem_PointDict(p_j,succ);
  destroy_elem_PointDict(p_j,pred);

}



void printPriorityQueue(PriorityQueue *pq)
{
    ////elog(NOTICE,"PRINT");
    for(int i=0;i < pq->size;i++)
    {

        struct PriorityQueueElem *current = pq->arr[i];
        //elog(NOTICE,"Point: %p, Priority: %lf\n", current->point, current->priority);
    }
}

void
iteration_simplification_sqe(void *p_i , void *p_j ,
                            size_t *beta,const double lambda,int i,
                            Dict *succ,Dict *pred,
                            PDict  *p,struct PriorityQueue *Q,
                             bool syncdist,interpType interp ,bool hasz ,uint32_t minpts)
{
////elog(NOTICE,"/////////////////");
  if( i * lambda >= *beta)
  {
      *beta += 1;
  }
  size_t size_before = size_queue(Q);
  set_priority_queue(p_i,9999,Q);
  set_priority_dict(p_i,0,p);
  if(i >= 1)
  {
     ////elog(NOTICE,"set_point_dict TEST BEGIN %p %p",p_i,p_j);
     set_point_dict(p_i,p_j,pred);
     set_point_dict(p_j,p_i,succ);
     ////elog(NOTICE,"set_point_dict TEST END");
     adjust_priority(p_j,Q,pred,succ,p, syncdist, interp , hasz );
     ////elog(NOTICE,"/////////////////");
  }
  size_t size = size_queue(Q);
  //elog(NOTICE,"------------TEST iteration %i -- beta %i -- limit %i -- lambda %f \n ---- size %zu",i,*beta,i/lambda,lambda,size);
    //elog(NOTICE,"BEFORE IF SIZE QUEUE  %zu  ->  %zu /// %i  ///%f",size_before,size,*beta,lambda);
  if(size - *beta == 0 ){
    reduce(Q,pred,succ,p,syncdist, interp , hasz );
    //elog(NOTICE,"ENTER IF SIZE QUEUE  %zu  ->  %zu /// %i  ///%f",size_before,size,*beta,lambda);
  }
  //elog(NOTICE,"/////////////////");
  printPriorityQueue(Q);
}




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
init_squish_variables(squish_variables *sq)
{
   sq->beta = 4;
   sq->i = 0;
   sq->succ = 0;
   sq->pred = 0;
   sq->p = 0;
   sq->Q = create_PriorityQueue(MAX_SIZE_PQ);
   sq->p_i = NULL;
   sq->p_j = NULL;
   sq->point = NULL;
}



void
free_squish_variables(squish_variables *sq)
{
  destroy_PointDict(sq->pred);
  destroy_PointDict(sq->succ);
  destroy_Queue(sq->Q);
  destroy_PriorityDict(sq->p);
}

void
sq_iteration(TInstant * point,squish_variables *sq ,const double lambda, bool syncdist,
                                                      uint32_t minpts)
{
  interpType interp = MEOS_FLAGS_GET_INTERP(point->flags);
  bool hasz = MEOS_FLAGS_GET_Z(point->flags);
  if(sq->i == 0)
  {
        sq->point = point;
        ////elog(NOTICE,"POINT %p \n",sq->point);
  }
  sq->p_j = sq->p_i;
  sq->p_i = point;
  iteration_simplification_sqe(sq->p_i , sq->p_j ,&(sq->beta),lambda,sq->i,
                                 &sq->succ,&sq->pred,
                                 &sq->p,sq->Q, syncdist,interp,hasz, minpts);
  sq->i++;
}

TSequence *
construct_simplify_path(squish_variables *sq,
bool lower_inc,bool upper_inc, interpType interp, bool normalize)
{
  /* Create a new temporal sequence */
  uint32_t i = 0;
  uint32_t outn = size_queue(sq->Q);
  const TInstant **instants = malloc(sizeof(TInstant *) * outn);
  //elog(NOTICE,"BEGIN CONSTRUCT SIZE Q : %i   \n",outn);
  //elog(NOTICE,"POINT %p \n",sq->point);
  TInstant * point = sq->point;
  while(point)
  {
    //elog(NOTICE,"POINT FROM SUCC : %p \n",sq->point);
    instants[i] = point;
    sq->p_j = sq->p_i;
    sq->p_i = point;

    instants[i] = point;
    point = (TInstant *) get_point_dict(point,&sq->succ);
    //elog(NOTICE,"POINT FROM SUCC : %p",point);
    i++;
  }
  //elog(NOTICE,"END CONSTRUCT SIZE succ : %i \n",i);
  TSequence *result = tsequence_make(instants, i, lower_inc, upper_inc,  interp, normalize);
  free(instants);

  return result;
}


/**
 * @brief Simplify the temporal sequence set float/point using the
 * SQUISH-E line simplification algorithm.
 */
static TSequence *
tsequence_simplify_sqe(const TSequence *seq, double dist, bool syncdist,
  uint32_t minpts)
{

  ////elog(NOTICE,"=== SQUISH-E Simplification ===");
  squish_variables SQ_variables;
  init_squish_variables(&SQ_variables);

  assert(MEOS_FLAGS_LINEAR_INTERP(seq->flags));
  assert(seq->temptype == T_TFLOAT || tgeo_type(seq->temptype));
  // Do not try to simplify really short things
  if (seq->count < 3)
    return tsequence_copy(seq);

  do
  {

    sq_iteration( (TInstant * )TSEQUENCE_INST_N(seq,SQ_variables.i)  ,&SQ_variables, dist,  syncdist,minpts);

  }
  while(SQ_variables.i < seq->count);
  ////elog(NOTICE,"=== SQUISH-E Simplification ===");

  TSequence *result = result = construct_simplify_path(&SQ_variables,seq->period.lower_inc,
                    seq->period.upper_inc, LINEAR, false);
  free_squish_variables(&SQ_variables);

  return result;
}
















/**
 * @brief Simplify the temporal sequence set float/point using the
 * SQUISH-E line simplification algorithm.
 * @param[in] ss Temporal point
 * @param[in] dist Distance
 * @param[in] syncdist True when computing the Synchronized Euclidean
 * Distance (SED), false when computing the spatial only distance.
 * @param[in] minpts Minimum number of pointserreur SYSCALL SSL : EOF détecté

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

  ////elog(NOTICE,"=== SQUISH-E Simplification ===");
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

