
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libpq-fe.h>

/* Number of instants to send in batch to the database */
#define NO_INSTANTS_BATCH 1000
/* Number of instants to keep when restarting a sequence, should keep at least one */
#define NO_INSTANTS_KEEP 2
/* Maximum length in characters of a header record in the input CSV file */
#define MAX_LENGTH_HEADER 1024
/* Maximum length in characters of a point in the input data */
#define MAX_LENGTH_POINT 64

#define LOOP 15

/**
 * @brief Function that sends a SQL query to the database
 * @returns 0 if OK -1 on error
 */
static int
exec_sql(PGconn *conn, const char *sql, ExecStatusType status)
{
  int result = 0;
  PGresult *res = PQexec(conn, sql);
  if (PQresultStatus(res) != status)
  {
    fprintf(stderr, "SQL command failed:\n%s %s", sql, PQerrorMessage(conn));
    result = -1;
  }
  PQclear(res);
  return result;
}

/* Main program */
int
main(int argc, char **argv)
{
  const char *conninfo;
  PGconn *conn;
  int res_sql;
  char point_buffer[MAX_LENGTH_POINT];
  char text_buffer[MAX_LENGTH_HEADER];
  /* Iterator variables */
  int i, j;
  /* Exit value initialized to 1 (i.e., error) to quickly exit upon error */
  int exit_value = 1;


  /* Get start time */
  clock_t t;
  t = clock();

  /***************************************************************************
   * Section 1: Connexion to the database
   ***************************************************************************/


  /*
   * If the user supplies a parameter on the command line, use it as the
   * conninfo string; otherwise default to setting dbname=postgres and using
   * environment variables or defaults for all other connection parameters.
   */
  if (argc > 1)
    conninfo = argv[1];
  else
    conninfo = "host=localhost user=postgres dbname=brussels password=1234";

  /* Make a connection to the database */
  conn = PQconnectdb(conninfo);

  /* Check to see that the backend connection was successfully made */
  if (PQstatus(conn) != CONNECTION_OK)
  {
    fprintf(stderr, "%s", PQerrorMessage(conn));
    goto cleanup;
  }


  /* Set always-secure search path, so malicious users can't take control. */
  res_sql = exec_sql(conn, "SELECT pg_catalog.set_config('search_path', '', false)",
      PGRES_TUPLES_OK);
  if (res_sql < 0)
    goto cleanup;

    /* Create the table that will hold the data */
    printf("Creating the table Benchmark in the database\n");
  res_sql = exec_sql(conn, "DROP TABLE IF EXISTS public.Benchmark;",
      PGRES_COMMAND_OK);  if (res_sql < 0)
    goto cleanup;
    res_sql = exec_sql(conn, "CREATE TABLE public.Benchmark("
      "s float, numInstant int,simplified int, elapsed_time float );", PGRES_COMMAND_OK);
    if (res_sql < 0)
      goto cleanup;

  float s = 2;
  for(int loop =0; loop < LOOP; loop++){

  sprintf(text_buffer,
    "SELECT MMSI,public.numInstants(trip) , public.numInstants( public.SquishESimplify(trip,float '%f')) FROM public.AISTrips;",s);

  t = clock();
  PGresult *res = PQexec(conn, text_buffer);
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    fprintf(stderr, "SQL command failed:\n%s %s", text_buffer,
      PQerrorMessage(conn));
    PQclear(res);
    goto cleanup;
  }
  /* Calculate the elapsed time */
  t = clock() - t;
  double time_taken = ((double) t) / CLOCKS_PER_SEC;

  /* State that the program executed successfully */
  exit_value = 0;


  int numrows = PQntuples(res);
  printf("Result of the query '%s'\n\n", text_buffer);
  printf("   mmsi    | numinstants | simplified\n");
  printf("-----------+-------------+-------------\n");
  for (i = 0; i < numrows; i++){
    printf(" %s |     %s |     %s\n", PQgetvalue(res, i, 0), PQgetvalue(res, i, 1), PQgetvalue(res, i, 2));
    sprintf(text_buffer,   "INSERT INTO  public.Benchmark("
                                                          "s, numInstant,simplified, elapsed_time) "
                                                          "VALUES (%f,%s,%s,%f) ",s,PQgetvalue(res, i, 1), PQgetvalue(res, i, 2),time_taken);
    exec_sql(conn,text_buffer,PGRES_COMMAND_OK);
                                                          }
  printf("The program took %f seconds to execute\n", time_taken);
  }
cleanup:
  /* Close the connection to the database and cleanup */
  PQfinish(conn);

  return exit_value;
}
