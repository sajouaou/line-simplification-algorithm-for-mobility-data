






PGDLLEXPORT Datum Temporal_simplify_sqe(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(Temporal_simplify_sqe);
/**
 * @ingroup mobilitydb_temporal_analytics_simplify
 * @brief Simplify the temporal sequence (set) float or point using a
 * SQUISH-E line simplification algorithm.
 */
Datum
Temporal_simplify_sqe(PG_FUNCTION_ARGS)
{
  Temporal *temp = PG_GETARG_TEMPORAL_P(0);
  double dist = PG_GETARG_FLOAT8(1);
  bool syncdist = true;
  if (PG_NARGS() > 2 && ! PG_ARGISNULL(2))
    syncdist = PG_GETARG_BOOL(2);

  Temporal * result = temp;
  PG_RETURN_POINTER(result);
}
