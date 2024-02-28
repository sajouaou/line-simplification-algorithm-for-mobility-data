
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libpq-fe.h>
#include <meos.h>
/* The expandable functions are in the internal MEOS API */
#include <meos_internal.h>
#include "../SquishE/meos/include/squish_e.h"

/* Number of instants to send in batch to the database */
#define NO_INSTANTS_BATCH 1000
/* Number of instants to keep when restarting a sequence, should keep at least one */
#define NO_INSTANTS_KEEP 2
/* Maximum length in characters of a header record in the input CSV file */
#define MAX_LENGTH_HEADER 1024
/* Maximum length in characters of a point in the input data */
#define MAX_LENGTH_POINT 64


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
  squish_variables SQ_variables;
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
    printf("Creating the table benchResult in the database\n");
    res_sql = exec_sql(conn, "CREATE TABLE IF NOT EXISTS  public.benchResult("
      "i int,lambda float,nbPoint int,duration interval,simpl text);", PGRES_COMMAND_OK);
    if (res_sql < 0)
      goto cleanup;

    float lamba_s[] = {0.01,0.25,0.5,0.75,1};
    int nbpoints[] = {1000000};
    int size_lambda = 1;
    int size_nbpoints = 1;
    int N = 10;

    printf("-- Construction de points --\n");
    sprintf(point_buffer, "SRID=4326;Point(%lf %lf)@%s+00", 10.0,
         10.0, "2000-02-02");
    TInstant **points = malloc(nbpoints[size_nbpoints-1]*sizeof(TInstant *));
    for (int temp= 0; temp < nbpoints[size_nbpoints-1];temp++){
         points[temp] = (TInstant *) tgeogpoint_in(point_buffer);
    }
    printf("-- Fin de construction points --\n");

    for(int n =0; n < size_nbpoints; n++){
        int nbpoint = nbpoints[n];
        for(int l =0; l < size_lambda; l++){
            float lambda = lamba_s[l];
            printf("RUN ALGO FOR SIZE {%i} and lambda %f\n",nbpoint,lambda);
            for (int i = 1; i <= N; i++){

                t = clock();
                /* Calculate the elapsed time */
                init_squish_variables(&SQ_variables);
                do
                  {
                    sq_iteration( points[i] ,&SQ_variables, lambda,  true,4);
                  }
                  while(SQ_variables.i < nbpoint);

                free_squish_variables(&SQ_variables);
                t = clock() - t;
                double time_taken = ((double) t) / CLOCKS_PER_SEC;

                sprintf(text_buffer,   "INSERT INTO  public.benchResult VALUES ("
                                        "%i,%f,%i,make_interval(secs => %f),'SQUISH-E-C') ",i,lambda,nbpoint,time_taken);
                exec_sql(conn,text_buffer,PGRES_COMMAND_OK);
            }
        }
    }


    for (int temp= 0; temp < nbpoints[size_nbpoints-1];temp++){
         free(points[temp]);
    }
    free(points);

  /* State that the program executed successfully */
  exit_value = 0;

cleanup:
  /* Close the connection to the database and cleanup */
  PQfinish(conn);

  return exit_value;
}
